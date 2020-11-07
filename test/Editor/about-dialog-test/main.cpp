#include "../../../Editor/main_window/about_dialog/aboutdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    aboutDialog w;
    w.show();
    return a.exec();
}
