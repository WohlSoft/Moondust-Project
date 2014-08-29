#include "devconsole.h"
#include "ui_devconsole.h"

#include <QScrollBar>
#include <QSettings>
#include "../version.h"
#include "../common_features/mainwinconnect.h"

DevConsole *DevConsole::currentDevConsole = 0;

void DevConsole::init()
{
    if(currentDevConsole)
        delete currentDevConsole;

    currentDevConsole = new DevConsole();

    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("DevConsole");
    currentDevConsole->restoreGeometry(settings.value("geometry", currentDevConsole->saveGeometry()).toByteArray());
    settings.endGroup();
}

void DevConsole::show()
{
    if(!currentDevConsole)
        init();

    currentDevConsole->showNormal();
}

void DevConsole::retranslate()
{
    if(!currentDevConsole) return;

    currentDevConsole->retranslateP();
}

void DevConsole::retranslateP()
{
    ui->retranslateUi(this);
}

void DevConsole::log(const QString &logText, const QString &channel, bool raise)
{
    if(!currentDevConsole)
        init();

    if(currentDevConsole)
        currentDevConsole->logToConsole(logText, channel, raise);
}

bool DevConsole::isConsoleShown()
{
    if(!currentDevConsole)
        return false;

    return !currentDevConsole->isHidden();
}

DevConsole::DevConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevConsole)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    connect(ui->button_cl_sysLog, SIGNAL(clicked()), this, SLOT(clearCurrentLog()));
}

DevConsole::~DevConsole()
{
    delete ui;
}

void DevConsole::logToConsole(const QString &logText, const QString &channel, bool raise)
{
    for(int i = 0; i < ui->tabWidget->count(); ++i){
        if(ui->tabWidget->tabText(i)==channel){
            QPlainTextEdit* tarEdit = getEditByIndex(i);
            if(!tarEdit)
                return;
            tarEdit->appendPlainText(logText);
            tarEdit->verticalScrollBar()->setValue(tarEdit->verticalScrollBar()->maximum());
            if(raise) ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    //create new channel
    QWidget* w = new QWidget();
    QGridLayout *l = new QGridLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    QPlainTextEdit *e = new QPlainTextEdit(w);
    l->addWidget(e,0,0,1,1);
    QPushButton *p = new QPushButton(w);
    l->addWidget(p,1,0,1,1);
    connect(p, SIGNAL(clicked()), this, SLOT(clearCurrentLog()));
    p->setText(tr("Clear %1 Log").arg(channel));
    e->setReadOnly(true);
    e->appendPlainText(logText);
    e->verticalScrollBar()->setValue(e->verticalScrollBar()->maximum());
    ui->tabWidget->addTab(w,channel);
}

QPlainTextEdit *DevConsole::getEditByIndex(const int &index)
{
    return qobject_cast<QPlainTextEdit*>((dynamic_cast<QWidgetItem*>(ui->tabWidget->widget(index)->layout()->itemAt(0)))->widget());
}

QPlainTextEdit *DevConsole::getCurrentEdit()
{
    return getEditByIndex(ui->tabWidget->currentIndex());
}

void DevConsole::on_button_clearAllLogs_clicked()
{
    for(int i = 0; i < ui->tabWidget->count(); ++i){
        QPlainTextEdit* e = getEditByIndex(i);
        e->clear();
    }
}

void DevConsole::clearCurrentLog()
{
    getCurrentEdit()->clear();
}

void DevConsole::closeEvent(QCloseEvent *event)
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("DevConsole");
    settings.setValue("geometry", this->saveGeometry());
    settings.endGroup();
    event->accept();
}



void DevConsole::on_button_send_clicked()
{
    doCommand();
}

void DevConsole::on_edit_command_returnPressed()
{
    doCommand();
}

void DevConsole::doCommand()
{
    if(ui->edit_command->text().isEmpty()) return;
    ui->tabWidget->setCurrentIndex(0);
    QString cmd = ui->edit_command->text();
    ui->edit_command->clear();

    if(cmd.startsWith("test", Qt::CaseInsensitive))
    {
        log("-> All good!", ui->tabWidget->tabText(0));
    }
    else
    if(cmd.startsWith("version", Qt::CaseInsensitive))
    {
        log(QString("-> "_FILE_DESC", version "_FILE_VERSION _FILE_RELEASE), ui->tabWidget->tabText(0));
    }
    else
    if(cmd.startsWith("quit", Qt::CaseInsensitive))
    {
        log("-> Bye-bye!", ui->tabWidget->tabText(0));
        MainWinConnect::pMainWin->close();
    }
    else
    if(cmd.startsWith("savesettings", Qt::CaseInsensitive))
    {
        MainWinConnect::pMainWin->saveSettings();
        log("-> Application Settings was saved!", ui->tabWidget->tabText(0));
    }
    else
    {
        log("-> Wrong command!", ui->tabWidget->tabText(0));
    }

}
