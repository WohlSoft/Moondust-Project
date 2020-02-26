#include "window.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ImageSelectTest w;
    w.show();
    return a.exec();
}
