#include "mainserverwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainServerWindow w;
    w.show();

    return a.exec();
}
