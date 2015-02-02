/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QScrollBar>
#include <QSettings>
#include <QCryptographicHash>
#include <stdexcept>
#ifdef Q_OS_WIN
#include <QtWin>
#include <windows.h>
#endif

#include <common_features/app_path.h>
#include <common_features/mainwinconnect.h>
#include <file_formats/pge_x.h>

#include "devconsole.h"
#include <ui_devconsole.h>

#include "../version.h"

DevConsole *DevConsole::currentDevConsole = 0;

void DevConsole::init()
{
    if(currentDevConsole)
        delete currentDevConsole;

    currentDevConsole = new DevConsole();

    QString inifile = AppPathManager::settingsFile();
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

void DevConsole::closeIfPossible()
{
    if(isConsoleShown())
        currentDevConsole->close();
}

DevConsole::DevConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DevConsole)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
    connect(ui->button_cl_sysLog, SIGNAL(clicked()), this, SLOT(clearCurrentLog()));
    connect(ui->tabWidget, SIGNAL(tabBarClicked(int)), this, SLOT(update()));
    registerCommands();

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    if(QtWin::isCompositionEnabled())
    {
        this->setAttribute(Qt::WA_TranslucentBackground, true);
        QtWin::extendFrameIntoClientArea(this, -1,-1,-1, -1);
        QtWin::enableBlurBehindWindow(this);
    }
    else
    {
        QtWin::resetExtendedFrame(this);
        setAttribute(Qt::WA_TranslucentBackground, false);
    }
    #endif
    if(!hasFocus()) setWindowOpacity(0.9);
}

DevConsole::~DevConsole()
{
    delete ui;
}

void DevConsole::logToConsole(const QString &logText, const QString &channel, bool raise)
{
    if(MainWinConnect::pMainWin->isMinimized())
        return;

    QString target_channel = channel;

    if(channel=="System") //Prevent creation another "system" tab if switched another UI language
        target_channel = ui->tabWidget->tabText(0);

    for(int i = 0; i < ui->tabWidget->count(); ++i){
        if(ui->tabWidget->tabText(i)==target_channel){
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
    p->setText(tr("Clear %1 Log").arg(target_channel));
    e->setReadOnly(true);
    e->appendPlainText(logText);
    e->verticalScrollBar()->setValue(e->verticalScrollBar()->maximum());
    ui->tabWidget->addTab(w,target_channel);
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
    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("DevConsole");
    settings.setValue("geometry", this->saveGeometry());
    settings.endGroup();
    event->accept();
}


void DevConsole::focusInEvent(QFocusEvent *)
{
     setWindowOpacity(1.0);
}

void DevConsole::focusOutEvent(QFocusEvent *)
{
     setWindowOpacity(0.9);
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
    registerCommand("strarr", &DevConsole::doValidateStrArray, tr("Arg: {String array} validating the PGE-X string array"));
    registerCommand("flood", &DevConsole::doFlood, tr("Args: {[Number] Gigabytes} | Floods the memory with megabytes"));
    registerCommand("unhandle", &DevConsole::doThrowUnhandledException, tr("Throws an unhandled exception to crash the editor"));
    registerCommand("segserv", &DevConsole::doSegmentationViolation, tr("Does a segmentation violation"));
    registerCommand("pgex", &DevConsole::doPgeXTest, tr("Arg: {Path to file} testing of PGE-X file format"));
    registerCommand("smbxtest", &DevConsole::doSMBXTest, tr("[WIP] Attempt to test the level in the SMBX Level Editor!"));
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

void DevConsole::doSegmentationViolation(QStringList)
{
    int* my_nullptr = 0;
    *my_nullptr = 42; //Answer to the Ultimate Question of Life, the Universe, and Everything will let you app crash >:D
}


QString ______recourseBuildPGEX_Tree(QList<PGEFile::PGEX_Entry > &entry, int depth=1)
{
    QString treeView="";

    for(int i=0; i<entry.size(); i++)
    {
        for(int k=0;k<depth;k++) treeView += "--";
            treeView += QString("=============\n");
        for(int k=0;k<depth;k++) treeView += "--";
            treeView += QString("SubTree name: %1\n").arg(entry[i].name);
        for(int k=0;k<depth;k++) treeView += "--";
            treeView += QString("Branch type: %1\n").arg(entry[i].type);
        for(int k=0;k<depth;k++) treeView += "--";
            treeView += QString("Entries: %1\n").arg(entry[i].data.size());
        if(entry[i].subTree.size()>0)
        {
            treeView += ______recourseBuildPGEX_Tree(entry[i].subTree, depth+1);
        }
    }

    return treeView;
}



void DevConsole::doPgeXTest(QStringList args)
{
    if(!args.isEmpty())
    {
        QString src;

        foreach(QString s, args)
            src.append(s+(args.indexOf(s)<args.size()-1 ? " " : ""));

        QFile file(src);
        if (!file.open(QIODevice::ReadOnly))
        {
            log(QString("-> Error: Can't open the file."));
            return;
        }

        QTextStream in(&file);   //Read File
        PGEFile pgeX_Data;
        QString raw = in.readAll();
        pgeX_Data.setRawData( raw );

        if( pgeX_Data.buildTreeFromRaw() )
        {
            QString treeView="";
            for(int i=0; i<pgeX_Data.dataTree.size(); i++)
            {
                treeView += QString("=============\n");
                treeView += QString("Section Name %1\n").arg(pgeX_Data.dataTree[i].name);
                treeView += QString("Section type: %1\n").arg(pgeX_Data.dataTree[i].type);
                treeView += QString("Entries %1\n").arg(pgeX_Data.dataTree[i].data.size());

//                foreach(PGEFile::PGEX_Item x, pgeX_Data.dataTree[i].data)
//                {
//                    if(x.values.size()>0)
//                    {
//                        treeView += x.values.first().marker + ":"+x.values.first().value + ";\n";
//                    }
//                }

                if(pgeX_Data.dataTree[i].subTree.size()>0)
                {
                    treeView += ______recourseBuildPGEX_Tree(pgeX_Data.dataTree[i].subTree);
                }
            }
            log(QString("-> File read:\nRaw size is %1\n%2").arg(raw.size()).arg(treeView),
                ui->tabWidget->tabText(0));

        }
        else
        {
            log(QString("-> File invalid: %1").arg(pgeX_Data.lastError()), ui->tabWidget->tabText(0));
        }

    }

}

void DevConsole::doSMBXTest(QStringList args)
{
#ifdef Q_OS_WIN
    COPYDATASTRUCT* cds = new COPYDATASTRUCT;
    cds->cbData = 1;
    cds->dwData = (ULONG_PTR)0xDEADC0DE;
    cds->lpData = NULL;

    HWND smbxWind = FindWindowA("ThunderRT6MDIForm", NULL);
    if(smbxWind){
        SendMessageA(smbxWind, WM_COPYDATA, (WPARAM)this->winId(), (LPARAM)cds);
        SetForegroundWindow(smbxWind);
        log("Sent Message (Hopefully it worked)");
    }else{
        log("Failed to find SMBX Window");
    }

#else
    log("Requires Windows OS!");
#endif
}

