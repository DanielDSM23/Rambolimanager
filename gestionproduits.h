#ifndef GESTIONPRODUITS_H
#define GESTIONPRODUITS_H
#include <QMessageBox>
#include <QDialog>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include "pleasewait.h"
#include "editlocation.h"
#include <QMessageBox>

namespace Ui {
class gestionProduits;
}

class gestionProduits : public QDialog
{
    Q_OBJECT

public:
    explicit gestionProduits(QWidget *parent = 0);
    ~gestionProduits();
    bool checkConnection();

private slots:
    void on_locationList_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_editButtonLocation_clicked();

    void on_addButtonLocation_clicked();

    void on_seeButtonLocation_clicked();

    void on_locationList_doubleClicked(const QModelIndex &index);

    void on_deleteButtonLocation_clicked();

private:
    Ui::gestionProduits *ui;
    QMessageBox *msg;
    QSqlDatabase db;
    QTreeWidgetItem * locationTree;
    QTreeWidgetItem * billeterieTree;
    pleaseWait * pw;
    editLocation * editLoc;
    int selectedIdLocation;
    int slectedIdBilleterie;
    QSqlQuery * qry;
    QString nameLocationSelected;

};

#endif // GESTIONPRODUITS_H
