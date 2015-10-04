#include "main_window.h"
#include <QApplication>
#include <common_features/logger.h>
#include <common_features/app_path.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AppPathManager::initAppPath();
    LoadLogSettings();

    MaintainerMain w;
    w.show();

    return a.exec();
}
