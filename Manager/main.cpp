#include "mainwindow.h"
#include <QApplication>
#include <common_features/app_path.h>
#include "settings.h"

#include <QDir>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    QApplication a(argc, argv);

    AppPathManager::initAppPath();

    ManagerSettings::load();
    MainWindow w;
    w.show();
    return a.exec();
}
