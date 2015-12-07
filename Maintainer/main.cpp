#include "main_window.h"
#include <QApplication>
#include <common_features/logger.h>
#include <common_features/app_path.h>

#include <QFileInfo>
#include <QDir>

int main(int argc, char *argv[])
{

    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );

    QApplication a(argc, argv);

    AppPathManager::initAppPath();
    LoadLogSettings();

    MaintainerMain w;
    w.show();

    return a.exec();
}
