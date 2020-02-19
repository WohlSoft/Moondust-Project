#include "testbox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TestBox w;
    w.show();
    return a.exec();
}
