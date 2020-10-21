#include "demo.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Demo w;
    w.show();
    return a.exec();
}
