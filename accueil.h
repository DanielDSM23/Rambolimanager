#ifndef ACCUEIL_H
#define ACCUEIL_H

#include <QMainWindow>
#include "gestionproduits.h"
#include "checkertickets.h"


namespace Ui {
class accueil;
}

class accueil : public QMainWindow
{
    Q_OBJECT

public:
    explicit accueil(QWidget *parent = 0);
    ~accueil();
    void setName(QString);
private slots:
    void on_actionGestion_Produits_triggered();

    void on_actionVerifier_Billet_triggered();

private:
    Ui::accueil *ui;
    gestionProduits * productWindow;
    checkerTickets * checkerWindow;
};

#endif // ACCUEIL_H
