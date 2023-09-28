#include "accueil.h"
#include "ui_accueil.h"

accueil::accueil(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::accueil)
{
    ui->setupUi(this);
}

accueil::~accueil()
{
    delete ui;
}

void accueil::setName(QString fullName){
    ui->welcomeMessage->setAlignment(Qt::AlignCenter);
    ui->welcomeMessage->setText("Bienvenue "+fullName);
}

void accueil::on_actionGestion_Produits_triggered()
{
    productWindow = new gestionProduits;
    productWindow->exec();
}

void accueil::on_actionVerifier_Billet_triggered()
{
    checkerWindow = new checkerTickets;
    checkerWindow->show();
}
