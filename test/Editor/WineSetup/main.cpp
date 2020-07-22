#include "../../../Editor/testing/wine/wine_setup.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WineSetup w;
    w.show();
    return a.exec();
}
