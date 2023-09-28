#include "pleasewait.h"
#include "ui_pleasewait.h"

pleaseWait::pleaseWait(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::pleaseWait)
{
    ui->setupUi(this);
    setWindowFlags(Qt::WindowCloseButtonHint);
}

pleaseWait::~pleaseWait()
{
    delete ui;
}
