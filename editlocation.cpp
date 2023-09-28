#include "editlocation.h"
#include "ui_editlocation.h"
#include <QTcpSocket>
#include <QPixmap>
#include <QInputDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <string>
editLocation::editLocation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::editLocation)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowSystemMenuHint);
    msg = new QMessageBox;
    pw = new pleaseWait;
    if(this->checkConnection()){

        outPixmap = new QPixmap();
        db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("xxx"); //definition de l'hostname de la base de données
        db.setUserName("xxx"); //definition du User
        db.setPassword("xxx"); //definition du MDP de la BDD
        db.setDatabaseName("rambolitalents_siteasso"); //definition du nom de la BDD
        db.open();
        if(!db.isOpen()){
            this->show();
            msg->warning(this, tr("Erreur - Rambolitalents Manager"),tr("La liaison avec la base de données est impossible."));
            exit(0);
        }
    }
    else{
        this->show();
        msg->warning(this, tr("Erreur - Rambolitalents Manager"),tr("Verifiez votre connexion internet et réessayez de lancer Rambolitalents Manager."));
        exit(0);
    }
    for(int i = 2; i <=18; i++){
        ui->textSize->addItem(QString::number(i));
    }
    ui->textSize->setCurrentIndex(10);
    connect(ui->description, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            this,     SLOT(slotCurrentCharFormatChanged(QTextCharFormat)));
    connect(ui->description, SIGNAL(cursorPositionChanged()),
            this,     SLOT(slotCursorPositionChanged()));
    m_lastBlockList = nullptr;
}

editLocation::~editLocation()
{

    delete outPixmap;
    delete msg;
    delete pw;
    delete ui;
}


void editLocation::on_cancel_clicked()
{
    this->~editLocation();
}

void editLocation::on_ok_clicked()
{
    if(!ui->description->toPlainText().isEmpty() || !ui->name->text().isEmpty() || ui->prix->value() != 0.00){
        QSqlQuery* query = new QSqlQuery(db);
        finalQuery.replace(":image",imageProduct);
        query->prepare(finalQuery);
        query->bindValue(":id", idProduit);
        query->bindValue(":name", ui->name->text());
        descriptionProduit = ui->description->toHtml();
        descriptionProduit.replace("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head>", "");
        descriptionProduit.replace("<body", "<div");
        descriptionProduit.replace("</body", "</div");
        descriptionProduit.replace("-qt-block-indent:0;", "");
        descriptionProduit.replace("-qt-block-indent:1;", "");
        descriptionProduit.replace("</html>", "");
        query->bindValue(":description", descriptionProduit);
        query->bindValue(":prix", QString::number(ui->prix->value()));
        qDebug() << query->executedQuery();
        qDebug() << descriptionProduit;
        pw->open();
        this->close();
        query->exec();
        this->~editLocation();
    }
    else{
        msg->warning(this, tr("Remplir les champs"), tr("Veuillez remplir les champs"));
    }

}

void editLocation::setData(int id){
    idProduit = id;
    finalQuery = "UPDATE `product_table` SET `name`=:name,`description`=:description,`prix`=:prix,`type`=0 WHERE product_table.productID = "+QString::number(id)+";";
    QSqlQuery* query = new QSqlQuery(db);
    query->prepare("SELECT name, description, prix FROM `product_table` WHERE type = 0 AND productID=:id;");
    query->bindValue(":id", id);
    query->exec();
    while (query->next()) {
        ui->name->setText(query->value(0).toString());
        ui->description->insertHtml(query->value(1).toString());
        ui->prix->setValue(query->value(2).toDouble());
    }
    query->prepare( "SELECT image FROM `product_table` WHERE type = 0 AND productID=:id" );
    query->bindValue(":id", id);
    query->exec();
    query->next();
    imageProduct = query->value(0).toByteArray().toHex();
    //imageProduct = QString::fromStdString(imageHex);
    outPixmap->loadFromData(query->value(0).toByteArray());
    ui->imageLabel->setPixmap(*outPixmap);
    ui->imageLabel->setScaledContents(true);
    ui->importImage->setText("Changer image...");
    setWindowTitle("Modifier "+ui->name->text()+" - Rambolitalents Manager");


}


bool editLocation::checkConnection()
{
    QTcpSocket* sock = new QTcpSocket(this);
    sock->connectToHost("www.google.com", 80);
    bool connected = sock->waitForConnected(1000);//ms

    if (!connected)
    {
        sock->abort();
        delete sock;
        return false;
    }
    sock->close();
    delete sock;
    return true;
}

void editLocation::on_bold_clicked()
{
    isBoldDown = !isBoldDown;
    ui->bold->setDown(isBoldDown);
    ui->bold->isDown() ? ui->description->setFontWeight(QFont::Bold) :
                         ui->description->setFontWeight(QFont::Normal);
}

void editLocation::on_italiic_clicked()
{
    isItalicDown = !isItalicDown;
    ui->italiic->setDown(isItalicDown);
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->italiic->isDown());
    mergeFormatOnWordOrSelection(fmt);
}



void editLocation::on_underline_clicked()
{
    isUnderlineDown = !isUnderlineDown;
    ui->underline->setDown(isUnderlineDown);
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->underline->isDown());
    mergeFormatOnWordOrSelection(fmt);
}


void editLocation::mergeFormatOnWordOrSelection(const QTextCharFormat &format) {
    QTextCursor cursor = ui->description->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
        }
    cursor.mergeCharFormat(format);
    ui->description->mergeCurrentCharFormat(format);
    ui->description->setFocus(Qt::TabFocusReason);
}

void editLocation::on_textSize_activated(const QString &arg1)
{
    qreal pointSize = arg1.toDouble();
    if (arg1.toFloat() > 0) {
        QTextCharFormat fmt;
        fmt.setFontPointSize(pointSize);
        mergeFormatOnWordOrSelection(fmt);
    }
}


void editLocation::slotCurrentCharFormatChanged(const QTextCharFormat &format) {
    fontChanged(format.font());
    //bgColorChanged((format.background().isOpaque()) ? format.background().color() : QColor());
    fgColorChanged((format.foreground().isOpaque()) ? format.foreground().color() : QColor());
    ui->link->setDown(format.isAnchor());
}


void editLocation::slotCursorPositionChanged() {
    QTextList *l = ui->description->textCursor().currentList();
    if (m_lastBlockList && (l == m_lastBlockList || (l != nullptr && m_lastBlockList != nullptr
                                 && l->format().style() == m_lastBlockList->format().style()))) {
        return;
        }
    m_lastBlockList = l;
    if (l) {
        QTextListFormat lfmt = l->format();
        if (lfmt.style() == QTextListFormat::ListDisc) {
            ui->list->setDown(true);
            ui->listnumber->setDown(false);
          } else if (lfmt.style() == QTextListFormat::ListDecimal) {
            ui->list->setDown(false);
            ui->listnumber->setDown(true);
          } else {
            ui->list->setDown(false);
            ui->listnumber->setDown(false);
            }
      } else {
        ui->list->setDown(false);
        ui->listnumber->setDown(false);
        }
}



void editLocation::fontChanged(const QFont &f) {
    ui->textSize->setCurrentIndex(ui->textSize->findText(QString::number(f.pointSize())));
    ui->bold->setDown(f.bold());
    ui->italiic->setDown(f.italic());
    ui->underline->setDown(f.underline());
    //f_strikeout->setChecked(f.strikeOut());   TODO
    if (ui->description->textCursor().currentList()) {
        QTextListFormat lfmt = ui->description->textCursor().currentList()->format();
        if (lfmt.style() == QTextListFormat::ListDisc) {
            ui->list->setDown(true);
            ui->listnumber->setChecked(false);
          } else if (lfmt.style() == QTextListFormat::ListDecimal) {
            ui->list->setDown(false);
           ui->listnumber->setDown(true);
          } else {
            ui->list->setDown(false);
            ui->listnumber->setDown(false);
            }
      } else {
        ui->list->setDown(false);
        ui->listnumber->setDown(false);
      }
}


void editLocation::on_list_clicked()
{
    isListDown = !isListDown;
    ui->list->setDown(isListDown);
    if (isListDown) {
        ui->list->setDown(isListDown);
       }
    list(isListDown, QTextListFormat::ListDisc);
}

void editLocation::list(bool checked, QTextListFormat::Style style) {
    QTextCursor cursor = ui->description->textCursor();
    cursor.beginEditBlock();
    if (!checked) {
        QTextBlockFormat obfmt = cursor.blockFormat();
        QTextBlockFormat bfmt;
        bfmt.setIndent(obfmt.indent());
        cursor.setBlockFormat(bfmt);
      } else {
        QTextListFormat listFmt;
        if (cursor.currentList()) {
            listFmt = cursor.currentList()->format();
            }
        listFmt.setStyle(style);
        cursor.createList(listFmt);
        }
    cursor.endEditBlock();
}

void editLocation::on_listnumber_clicked()
{
    isNumberListDown = !isNumberListDown;
    ui->listnumber->setDown(isNumberListDown);
    if (isNumberListDown) {
        ui->listnumber->setDown(isNumberListDown);
        }
    list(isNumberListDown, QTextListFormat::ListDecimal);
}

void editLocation::on_link_clicked(bool checked)
{
    bool unlink = false;
    QTextCharFormat fmt;
    if (checked) {
        QString url = ui->description->currentCharFormat().anchorHref();
        bool ok;
        QString newUrl = QInputDialog::getText(this, tr("Créer un lien"),
                                        tr("Lien :"), QLineEdit::Normal,
                                        url,
                                        &ok);
        if (ok) {
            fmt.setAnchor(true);
            fmt.setAnchorHref(newUrl);
            fmt.setForeground(QApplication::palette().color(QPalette::Link));
            fmt.setFontUnderline(true);
          } else {
            unlink = true;
            }
      } else {
        unlink = true;
        }
    if (unlink) {
        fmt.setAnchor(false);
        fmt.setForeground(QApplication::palette().color(QPalette::Text));
        fmt.setFontUnderline(false);
        }
    mergeFormatOnWordOrSelection(fmt);
}

void editLocation::on_color_clicked()
{
    QColor col = QColorDialog::getColor(ui->description->textColor(), this);
    QTextCursor cursor = ui->description->textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::WordUnderCursor);
        }
    QTextCharFormat fmt = cursor.charFormat();
    if (col.isValid()) {
        fmt.setForeground(col);
      } else {
        fmt.clearForeground();
        }
    cursor.setCharFormat(fmt);
    ui->description->setCurrentCharFormat(fmt);
    fgColorChanged(col);
}

void editLocation::fgColorChanged(const QColor &c) {
    QPixmap pix(16, 16);
    if (c.isValid()) {
        pix.fill(c);
      } else {
        pix.fill(QApplication::palette().windowText().color());
        }
    ui->color->setIcon(pix);
}

void editLocation::on_importImage_clicked()
{
    QString path = QFileDialog::getOpenFileName(this, "Ouvrir", "", "Images (*.png *.jpg)");
    if(path != ""){
        if(idProduit != 0){
            finalQuery += "UPDATE `product_table` SET `image`=0x:image WHERE productID = "+QString::number(idProduit)+";";
        }
        outPixmap->load(path);
        ui->imageLabel->setPixmap(*outPixmap);
        QFile* imageFile = new QFile(path);
        imageFile->open(QIODevice::ReadOnly);
        imageProduct = imageFile->readAll().toHex();
        ui->imageLabel->setScaledContents(true);
        //std::string imageStr = ba.toHex().toStdString();
        //imageProduct = QString::fromStdString(imageStr);
    }


}
