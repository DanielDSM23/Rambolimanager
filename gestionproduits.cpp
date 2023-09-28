#include "gestionproduits.h"
#include "ui_gestionproduits.h"
#include <QTcpSocket>
#include <QListView>
#include <shlobj.h>
#include <shlwapi.h>


gestionProduits::gestionProduits(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::gestionProduits)
{
    ui->setupUi(this);
    setWindowModality(Qt::ApplicationModal);
    ui->editButtonBilleterie->setEnabled(false);
    ui->deleteButtonBilleterie->setEnabled(false);
    ui->editButtonLocation->setEnabled(false);
    ui->deleteButtonLocation->setEnabled(false);
    ui->seeButtonBilleterie->setEnabled(false);
    ui->seeButtonLocation->setEnabled(false);
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
    //locations
    qry = new QSqlQuery(db);
    qry->prepare("SELECT productID, name, description, prix, image FROM `product_table` WHERE type = 0;");
    qry->exec();
    ui->locationList->setColumnCount(4);
    QStringList labelsLocation;
    labelsLocation << "ID" <<"Nom" << "Description" << "Prix (en €/jour)";
    ui->locationList->setHeaderLabels(labelsLocation);
    QString tableLocation[qry->size()][5];
    int line = -1;
    while (qry->next()) {
       line++;
       for(int i=0; i < 5; i++){
        tableLocation[line][i]= qry->value(i).toString();
        qDebug() << tableLocation[line][i];
       }
    }
    qDebug() << qry->size();
    locationTree = new QTreeWidgetItem[qry->size()];
    for(int i=0; i<qry->size(); i++){
        for(int z=0; z<5; z++){
            //locationTree[i].QTreeWidgetItem(ui->locationList);
            locationTree[i].setText(z, tableLocation[i][z]);
            ui->locationList->addTopLevelItem(& locationTree[i]);
        }
    }
    ui->locationList->setColumnWidth(2, 700);
    ui->locationList->hideColumn(0);
    //Spectacles
    qry->prepare("SELECT productID, name, description, prix, image FROM `product_table` WHERE type = 1;");
    qry->exec();
    ui->billeterieList->setColumnCount(4);
    QStringList labelsSpectacles;
    labelsSpectacles << "ID" <<"Nom" << "Description" << "Prix (en €/jour)";
    ui->billeterieList->setHeaderLabels(labelsLocation);
    QString tableSpectacles[qry->size()][5];
    line = -1;
    while (qry->next()) {
       line++;
       for(int i=0; i < 5; i++){
        tableSpectacles[line][i]= qry->value(i).toString();
        qDebug() << tableSpectacles[line][i];
       }
    }
    qDebug() << qry->size();
    billeterieTree = new QTreeWidgetItem[qry->size()];
    for(int i=0; i<qry->size(); i++){
        for(int z=0; z<5; z++){
            //locationTree[i].QTreeWidgetItem(ui->locationList);
            billeterieTree[i].setText(z, tableSpectacles[i][z]);
            ui->billeterieList->addTopLevelItem(& billeterieTree[i]);
        }
    }
    ui->billeterieList->setColumnWidth(2, 700);
    ui->billeterieList->hideColumn(0);


}

gestionProduits::~gestionProduits()
{
    delete msg;
    delete ui;
}

bool gestionProduits::checkConnection()
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

void gestionProduits::on_locationList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    if(!ui->editButtonLocation->isEnabled() && previous != 0x0){
        ui->editButtonLocation->setEnabled(true);
        ui->deleteButtonLocation->setEnabled(true);
        ui->seeButtonLocation->setEnabled(true);
    }
    selectedIdLocation = current->text(0).toInt();
    nameLocationSelected = current->text(1);
}

void gestionProduits::on_editButtonLocation_clicked()
{
    editLoc = new editLocation;
    editLoc->setData(selectedIdLocation);
    editLoc->open();
}



void gestionProduits::on_addButtonLocation_clicked()
{
    editLoc = new editLocation;
    editLoc->open();
}

void gestionProduits::on_seeButtonLocation_clicked()
{
    QString url = "https://rambolitalents.daniel-monteiro.fr/location/"+QString::number(selectedIdLocation);
    QByteArray urlBa = url.toLocal8Bit();
    char * urlChar = urlBa.data();
    ShellExecute(NULL, "open", urlChar, NULL, NULL, SW_HIDE);

}

void gestionProduits::on_locationList_doubleClicked(const QModelIndex &index)
{
    pw = new pleaseWait;
    pw->open();
    editLoc = new editLocation;
    editLoc->setData(selectedIdLocation);
    pw->close();
    pw->~pleaseWait();
    editLoc->open();
}

void gestionProduits::on_deleteButtonLocation_clicked()
{
    QMessageBox box;
    box.setWindowTitle(tr("Confirmation - Rambolitalents Manager"));
    box.setIcon(QMessageBox::Information);
    box.setText(tr("Souhaitez-vous réellement supprimer la location \""+nameLocationSelected.toLocal8Bit()+"\"."));

    QPushButton *Oui = box.addButton(tr("Oui, Supprimer"),
                                        QMessageBox::YesRole);
    QPushButton *Non = box.addButton(tr("Non"),
                                       QMessageBox::NoRole);

    box.exec();
    if (box.clickedButton() == Oui){
        qry = new QSqlQuery(db);
        qry->prepare("UPDATE `product_table` SET `isSaleable`=0 WHERE productID = :productId;");
        qry->bindValue(":productId", selectedIdLocation);
        qry->exec();
    }


}
