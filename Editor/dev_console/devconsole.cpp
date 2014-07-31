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
    if(!currentDevConsole)
        init();

    currentDevConsole->showNormal();
}

void DevConsole::log(const QString &logText, const QString &channel)
{
    if(!currentDevConsole)
        init();

    if(currentDevConsole)
        currentDevConsole->logToConsole(logText, channel);
}

DevConsole::DevConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevConsole)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
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
    //create new channel
    QWidget* w = new QWidget();
    QGridLayout *l = new QGridLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    QPlainTextEdit *e = new QPlainTextEdit(w);
    l->addWidget(e,0,0,1,1);
    e->setReadOnly(true);
    e->appendPlainText(logText);
    e->verticalScrollBar()->setValue(e->verticalScrollBar()->maximum());
    ui->tabWidget->addTab(w,channel);
}
