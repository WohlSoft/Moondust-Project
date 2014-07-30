#include "devconsole.h"
#include "ui_devconsole.h"

#include <QScrollBar>

DevConsole *DevConsole::currentDevConsole = 0;

void DevConsole::init()
{
    currentDevConsole = new DevConsole();
}

void DevConsole::show()
{
    currentDevConsole->showNormal();
}

void DevConsole::log(const QString &logText, const QString &channel)
{
    if(currentDevConsole)
        currentDevConsole->logToConsole(logText, channel);
}

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

void DevConsole::logToConsole(const QString &logText, const QString &channel)
{
    for(int i = 0; i < ui->tabWidget->count(); ++i){
        if(ui->tabWidget->tabText(i)==channel){
            QPlainTextEdit* tarEdit = qobject_cast<QPlainTextEdit*>((dynamic_cast<QWidgetItem*>(ui->tabWidget->widget(i)->layout()->itemAt(0)))->widget());
            if(!tarEdit)
                return;
            tarEdit->appendPlainText(logText);
            tarEdit->verticalScrollBar()->setValue(tarEdit->verticalScrollBar()->maximum());
            return;
        }
    }
}
