#include "connexion.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    connexion w;
    w.show();

    return a.exec();
}
