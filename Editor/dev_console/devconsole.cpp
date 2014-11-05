#include "devconsole.h"
#include "ui_devconsole.h"

#include <QScrollBar>
#include <QSettings>
#include <QCryptographicHash>

#include "../version.h"

#include "../common_features/app_path.h"

#include "../common_features/mainwinconnect.h"


#include "../file_formats/file_formats.h"

DevConsole *DevConsole::currentDevConsole = 0;

void DevConsole::init()
{
    if(currentDevConsole)
        delete currentDevConsole;

    currentDevConsole = new DevConsole();

    QString inifile = ApplicationPath + "/" + "pge_editor.ini";
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
    registerCommands();
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

void DevConsole::registerCommand(const QString commandName, DevConsole::command cmd, const QString helpText)
{
    commands[commandName.toLower()] = qMakePair<command, QString>(cmd, helpText);
}

void DevConsole::registerCommand(const std::initializer_list<QString> commandNames, DevConsole::command cmd, const QString helpText)
{
    foreach(QString tarCmd, commandNames){
        commands[tarCmd.toLower()] = qMakePair<command, QString>(cmd, helpText);
    }
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
    QString inifile = ApplicationPath + "/" + "pge_editor.ini";
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

void DevConsole::registerCommands()
{
    registerCommand({"?", "help"}, &DevConsole::doHelp, tr("Prints the command help"));
    registerCommand("test", &DevConsole::doTest, tr("Prints a test command"));
    registerCommand("version", &DevConsole::doVersion, tr("Prints the version"));
    registerCommand("quit", &DevConsole::doQuit, tr("Quits the program"));
    registerCommand("savesettings", &DevConsole::doSavesettings, tr("Saves the application settings"));
    registerCommand("md5", &DevConsole::doMd5, tr("Args: {SomeString} Calculating MD5 hash of string"));
    registerCommand("strarr", &DevConsole::doValidateStrArray, tr("Args: {String array} validating the PGE-X string array"));
    registerCommand("flood", &DevConsole::doFlood, tr("Args: {[Number] Gigabytes} | Floods the memory with megabytes"));
    registerCommand("unhandle", &DevConsole::doThrowUnhandledException, tr("Throws an unhandled exception to crash the editor"));
}

void DevConsole::doCommand()
{
    if(ui->edit_command->text().isEmpty()) return;
    ui->tabWidget->setCurrentIndex(0);
    QString cmd = ui->edit_command->text();
    if(cmd.isEmpty())
        return;
    ui->edit_command->clear();

    QStringList cmdList = cmd.split(" ");
    QString baseCommand = cmdList[0];
    cmdList.pop_front();

    if(commands.contains(baseCommand.toLower())){
        (this->*(commands[baseCommand].first))(cmdList);
    }
}

void DevConsole::doHelp(QStringList /*args*/)
{
    QStringList keys = commands.keys();
    for(int i = 0; i < keys.size(); ++i){
        log(keys[i] + QString(" - " + commands[keys[i]].second));
    }
}

void DevConsole::doTest(QStringList /*args*/)
{
    log("-> All good!", ui->tabWidget->tabText(0));
}

void DevConsole::doMd5(QStringList args)
{
    QString src;

    foreach(QString s, args)
        src.append(s+(args.indexOf(s)<args.size()-1 ? " " : ""));

    QString encoded = QString(QCryptographicHash::hash(src.toUtf8(), QCryptographicHash::Md5).toHex());

    log(QString("MD5 hash: %1").arg(encoded), ui->tabWidget->tabText(0));
}

void DevConsole::doValidateStrArray(QStringList args)
{
    QString src;

    foreach(QString s, args)
        src.append(s+(args.indexOf(s)<args.size()-1 ? " " : ""));

    log(QString("%1").arg(src), ui->tabWidget->tabText(0));
    log(QString("String array is: %1").arg(PGEFile::IsStringArray(src)?"valid":"wrong" ), ui->tabWidget->tabText(0));
}


void DevConsole::doVersion(QStringList /*args*/)
{
    log(QString("-> " _FILE_DESC ", version " _FILE_VERSION _FILE_RELEASE), ui->tabWidget->tabText(0));
}

void DevConsole::doQuit(QStringList /*args*/)
{
    log("-> Bye-bye!", ui->tabWidget->tabText(0));
    MainWinConnect::pMainWin->close();
}

void DevConsole::doSavesettings(QStringList /*args*/)
{
    MainWinConnect::pMainWin->saveSettings();
    log("-> Application Settings was saved!", ui->tabWidget->tabText(0));
}

void DevConsole::doFlood(QStringList args)
{
    if(args.size() > 0){
        bool succ;
        int floodSize = args[0].toInt(&succ);
        if(!succ)
            return;
        log("Flooding with " + QString::number(floodSize*1024*1024) + "bytes");
        char* fl = new char[floodSize*1024*1024];
        if(fl == 0)
            log("No memory assigned");
        Q_UNUSED(fl)
    }
}

void DevConsole::doThrowUnhandledException(QStringList /*args*/)
{
    throw std::runtime_error("Test Exception of Toast!");
}

