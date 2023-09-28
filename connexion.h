#ifndef CONNEXION_H
#define CONNEXION_H
#include <QMainWindow>
#include "accueil.h"
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>

namespace Ui {
class connexion;
}

class connexion : public QMainWindow
{
    Q_OBJECT

public:
    explicit connexion(QWidget *parent = 0);
    ~connexion();
    bool checkConnection();

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::connexion *ui;
    accueil *home;
    QSqlDatabase db;
    QSqlQuery * query;
    QMessageBox * msg;
};

#endif // CONNEXION_H
