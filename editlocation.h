#ifndef EDITLOCATION_H
#define EDITLOCATION_H

#include <QDialog>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QMessageBox>
#include <QPointer>
#include <QTextCharFormat>
#include <QTextList>
#include "pleasewait.h"

namespace Ui {
class editLocation;
}

class editLocation : public QDialog
{
    Q_OBJECT

public:
    explicit editLocation(QWidget *parent = 0);
    ~editLocation();
    void setData(int);

private slots:

    void on_cancel_clicked();

    void on_ok_clicked();

    bool checkConnection();

    void on_bold_clicked();


    void on_italiic_clicked();

    void on_underline_clicked();

    void mergeFormatOnWordOrSelection(const QTextCharFormat &format);

    void on_textSize_activated(const QString &arg1);
    void slotCursorPositionChanged();
    void slotCurrentCharFormatChanged(const QTextCharFormat &format);
    void fontChanged(const QFont &f);


    void on_list_clicked();
    void list(bool checked, QTextListFormat::Style style);

    void on_listnumber_clicked();

    void on_link_clicked(bool checked);

    void on_color_clicked();
    void fgColorChanged(const QColor &c);
    void on_importImage_clicked();

private:
    Ui::editLocation *ui;
    QMessageBox *msg;
    QPixmap * outPixmap;
    QSqlDatabase db;
    bool isBoldDown = false;
    bool isItalicDown = false;
    bool isUnderlineDown = false;
    bool isListDown = false;
    bool isNumberListDown = false;
    QPointer<QTextList> m_lastBlockList;
    QByteArray imageProduct;
    QString nameProduct;
    QString descriptionProduit;
    QString prixProduit;
    QString finalQuery = "INSERT INTO `product_table` (`productID`, `name`, `description`, `prix`, `image`, `type`) VALUES (NULL, :name, :description, :prix, 0x:image, '0');"; //0x before image
    int idProduit = 0;
    pleaseWait * pw;
};

#endif // EDITLOCATION_H
