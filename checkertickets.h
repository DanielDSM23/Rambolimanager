#ifndef CHECKERTICKETS_H
#define CHECKERTICKETS_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSound>

namespace Ui {
class checkerTickets;
}

class checkerTickets : public QMainWindow
{
    Q_OBJECT

public:
    explicit checkerTickets(QWidget *parent = 0);
    ~checkerTickets();
    bool checkConnection();

private slots:

    void on_productList_currentIndexChanged(int index);

    void billetScanned();

    void gotoLast();

private:
    Ui::checkerTickets *ui;
    QSqlDatabase db;
    QMessageBox * msg;
    QSqlQuery * query;
    int selectedProductId;
    QString billetString;
    QSound * ok;
    QSound * scanned;
    QSound * no;
};

#endif // CHECKERTICKETS_H
