#include "fontsetuptest.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FontSetupTest w;
    w.show();
    return a.exec();
}
