#include "mainwindow.h"
#include <QApplication>
#include <common_features/app_path.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppPathManager::initAppPath();

    MainWindow w;
    w.show();

    return a.exec();
}
