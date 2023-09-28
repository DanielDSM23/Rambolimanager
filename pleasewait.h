#ifndef PLEASEWAIT_H
#define PLEASEWAIT_H

#include <QDialog>

namespace Ui {
class pleaseWait;
}

class pleaseWait : public QDialog
{
    Q_OBJECT

public:
    explicit pleaseWait(QWidget *parent = 0);
    ~pleaseWait();
private:
    Ui::pleaseWait *ui;
};

#endif // PLEASEWAIT_H
