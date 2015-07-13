#include "mainwindow.h"
#include <QApplication>
#include <common_features/app_path.h>
#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppPathManager::initAppPath();

    ManagerSettings::load();
    MainWindow w;
    w.show();
    ManagerSettings::save();

    return a.exec();
}
