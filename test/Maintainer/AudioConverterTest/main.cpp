#include "coverter_dialogue.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CoverterDialogue w;
    w.show();
    return a.exec();
}
