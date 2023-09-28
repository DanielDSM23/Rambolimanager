#include "checkertickets.h"
#include "ui_checkertickets.h"
#include <QTcpSocket>
#include <QMovie>

QString base64_decode(QString string){
    QByteArray ba;
    ba.append(string);
    return QByteArray::fromBase64(ba);
}

checkerTickets::checkerTickets(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::checkerTickets)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    msg = new QMessageBox;
    if(this->checkConnection()){
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
    query = new QSqlQuery;
    query->prepare("SELECT `productID`, `name` FROM `product_table` WHERE type = 1");
    query->exec();
    while(query->next()){
        ui->productList->addItem(query->value(1).toString(), query->value(0).toString());
    }
    connect(ui->billet, SIGNAL(returnPressed()),this ,SLOT(billetScanned()));
    ok = new QSound("sound/ok.wav");
    scanned = new QSound("sound/scanned.wav");
    no = new QSound("sound/no.wav");
    ui->billet->setStyleSheet("QLineEdit { background: rgba(255, 255, 255, 0); selection-background-color: rgba(255, 255, 255, 0); border: 0px solid white; color :rgba(0,0,0,0);caret-color: rgba(0,0,0,0);}");
    QMovie *gif = new QMovie("images/select.gif");
    ui->state->setMovie(gif);
    gif->start();
    ui->state->setScaledContents(true);
    ui->Name_2->setText("Sélectionner Produit");
    ui->Name_2->setAlignment(Qt::AlignCenter);
    ui->Name_2->setStyleSheet("font-weight: bold;");
    //ui->billet->text

}

void checkerTickets::gotoLast(){
    ui->billet->setFocus();
}

checkerTickets::~checkerTickets()
{
    delete ui;
}

bool checkerTickets::checkConnection()
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



void checkerTickets::on_productList_currentIndexChanged(int index)
{
    if(ui->productList->itemData(index).toInt() != 0){
        ui->billet->setEnabled(true);
        selectedProductId = ui->productList->itemData(index).toInt();
        QMovie *gif = new QMovie("images/qrcode.gif");
        ui->state->setMovie(gif);
        gif->start();
        ui->state->setScaledContents(true);
        ui->Name_2->setText("Scanner");
        ui->Name_2->setAlignment(Qt::AlignCenter);
        ui->Name_2->setStyleSheet("font-weight: bold;");
        this->gotoLast();
    }
    else{
        ui->billet->setEnabled(false);
        QMovie *gif = new QMovie("images/select.gif");
        ui->state->setMovie(gif);
        gif->start();
        ui->state->setScaledContents(true);
        ui->Name_2->setText("Séléctionner Produit");
        ui->Name_2->setAlignment(Qt::AlignCenter);
        ui->Name_2->setStyleSheet("font-weight: bold;");
        ui->Name->clear();
    }
}


void checkerTickets::billetScanned(){
    ui->state->clear();
    ui->Name->clear();
    ui->Name_2->clear();
    QMovie *gif = new QMovie();
    QString clientId, nomClient, adresse;
    bool isScanned;
    billetString = base64_decode(ui->billet->text());
    ui->billet->clear();
    query->prepare("SELECT `clientID`, `isScanned` FROM `billets_spectacles_table` WHERE numeroBillet = :stringBillet AND productId = :productId");
    query->bindValue(":stringBillet", billetString);
    query->bindValue(":productId", selectedProductId);
    query->exec();
    if(query->size() == 1){
        while(query->next()){
            clientId = query->value(0).toString();
            isScanned = query->value(1).toBool();
            qDebug() << query->value(1);
        }
        query->prepare("SELECT `first_name`, `last_name`, `adress`, `zip`, `city` FROM `client_table` WHERE clientID = :clientId;");
        query->bindValue(":clientId", clientId);
        query->exec();
        while(query->next()){
            nomClient = query->value(0).toString()+" "+query->value(1).toString();
            adresse = query->value(2).toString()+", "+query->value(3).toString()+", "+query->value(4).toString();
        }
        ui->Name->setText("Nom : "+nomClient+"\nAdresse : "+adresse);
        ui->Name->setAlignment(Qt::AlignCenter);
        qDebug() << isScanned;
        if(!isScanned){
            query->prepare("UPDATE `billets_spectacles_table` SET `isScanned`=1 WHERE numeroBillet = :stringBillet;");
            query->bindValue(":stringBillet", billetString);
            query->exec();
            ok->play();
            ui->Name_2->setText("Billet Valide");
            ui->Name_2->setAlignment(Qt::AlignCenter);
            ui->Name_2->setStyleSheet("color:#90EE90;font-weight: bold;font-size: 1.2em;");
            gif->setFileName("images/ok.gif");
            ui->state->setMovie(gif);
            gif->start();
            ui->state->setScaledContents(true);

        }
        else{
            scanned->play();
            ui->Name_2->setText("Billet Valide, Déjà Scanné");
            ui->Name_2->setAlignment(Qt::AlignCenter);
            ui->Name_2->setStyleSheet("color:#FFA300;font-weight: bold;font-size: 1.2em;");
            gif->setFileName("images/scanned.gif");
            ui->state->setMovie(gif);
            gif->start();
            ui->state->setScaledContents(true);
        }
    }
    else{
        no->play();
        ui->Name_2->setText("Billet Non Valide");
        ui->Name_2->setAlignment(Qt::AlignCenter);
        ui->Name_2->setStyleSheet("color:#FF6347;font-weight: bold;font-size: 1.2em;");
        gif->setFileName("images/no.gif");
        ui->state->setMovie(gif);
        gif->start();
        ui->state->setScaledContents(true);
    }
}
