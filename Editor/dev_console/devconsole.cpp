#include "devconsole.h"
#include "ui_devconsole.h"

DevConsole::DevConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevConsole)
{
    ui->setupUi(this);
}

DevConsole::~DevConsole()
{
    delete ui;
}
