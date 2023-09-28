#include "connexion.h"
#include "ui_connexion.h"
#include <QDebug>
#include <QTcpSocket>
#include <QPushButton>
#include <shlobj.h>
#include <shlwapi.h>

connexion::connexion(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::connexion)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowSystemMenuHint);
    connect(ui->password, SIGNAL(returnPressed()),ui->buttonBox ,SIGNAL(accepted()));
    connect(ui->id, SIGNAL(returnPressed()),ui->buttonBox ,SIGNAL(accepted()));
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
            this->~connexion();
        }
    }
    else{
        this->show();
        //msg->warning(this, tr("Erreur - Rambolitalents Manager"),tr("Verifiez votre connexion internet et réessayez de lancer Rambolitalents Manager."));
        //this->~connexion();
        QMessageBox box;
        box.setWindowTitle(tr("Erreur - Rambolitalents Manager"));
        box.setIcon(QMessageBox::Warning);
        box.setText(tr("Verifiez votre connexion internet et réessayez de lancer Rambolitalents Manager."));

        QPushButton *restart = box.addButton(tr("Relancer"),
                                            QMessageBox::YesRole);
        QPushButton *close = box.addButton(tr("Fermer"),
                                           QMessageBox::NoRole);

        box.exec();
        if (box.clickedButton() == restart){
            this->~connexion();
            ShellExecute(NULL, "open", "\""+qApp->applicationDirPath().toUtf8()+"/rambolitalentsManager.exe\"", NULL, NULL, SW_NORMAL);
            //system("\""+qApp->applicationDirPath().toUtf8()+"/rambolitalentsManager.exe\" && exit");
            exit(0);
        }

        if (box.clickedButton() == close){
            exit(0);
        }

    }

}

connexion::~connexion()
{

    //delete msg;
    delete ui;
}

void connexion::on_buttonBox_rejected()
{
    exit(0);
}

void connexion::on_buttonBox_accepted()
{
    QSqlQuery * query =new QSqlQuery;
    query->prepare("SELECT admin_table.fullName FROM `admin_table` WHERE login = :id;");
    query->bindValue(":id", ui->id->text());
    query->exec();
    if(query->size() == 1){
        query->prepare("SELECT admin_table.fullName FROM `admin_table` WHERE login = :id AND password = MD5(:password);");
        query->bindValue(":id", ui->id->text());
        query->bindValue(":password", ui->password->text());
        query->exec();
        qDebug() << query->value(0);
        QString fullName;
        while (query->next()) {
          fullName = query->value(0).toString();
        }
        if(query->size() == 1){
            delete query;
            home = new accueil;
            home->setName(fullName);
            home->show();
            this->~connexion();
        }
        else{
            ui->password->setStyleSheet("border: 1px solid red");
            ui->label->setStyleSheet("color:red;");
            ui->label->setAlignment(Qt::AlignCenter);
            ui->label->setText("Mot de passe incorrect. Réessayez.");
        }
    }
    else{
        ui->id->setStyleSheet("border: 1px solid red");
        ui->password->setStyleSheet("border: 1px solid red");
        ui->label->setStyleSheet("color:red;");
        ui->label->setAlignment(Qt::AlignCenter);
        ui->label->setText("identifiant incorrect. Réessayez.");
    }
}


bool connexion::checkConnection()
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
