#include "coverter_dialogue.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setOrganizationName("Moondust Team");
    QApplication::setOrganizationDomain("wohlsoft.ru");
    QApplication::setApplicationName("TEST: Audio Converter");
    QApplication a(argc, argv);

    CoverterDialogue w;
    w.show();

    return a.exec();
}
