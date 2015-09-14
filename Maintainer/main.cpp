#include "main_window.h"
#include <QApplication>
#include <common_features/logger.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    LoadLogSettings();

    MaintainerMain w;
    w.show();

    return a.exec();
}
