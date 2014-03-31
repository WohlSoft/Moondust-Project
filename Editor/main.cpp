#include <QtGui/QApplication>
#include <QFile>
#include <QTranslator>
#include <QLocale>
#include <QFileDialog>
//#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
