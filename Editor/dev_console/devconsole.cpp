/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <QSysInfo>
#endif

#include <common_features/app_path.h>
#include <common_features/main_window_ptr.h>
#include <common_features/logger_sets.h>

#include <PGE_File_Formats/pge_x.h>

#include <audio/music_player.h>
#include <networking/engine_intproc.h>

#include "devconsole.h"
#include <ui_devconsole.h>

#ifdef ENABLE_CRASH_TESTS
#include <common_features/crashhandler.h>
#include <editing/_dialogs/itemselectdialog.h>
#endif

#include "../version.h"

DevConsole *DevConsole::currentDevConsole = 0;

void DevConsole::init()
{
    if(currentDevConsole)
        delete currentDevConsole;

    currentDevConsole = new DevConsole();

    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("dev-console");
    currentDevConsole->restoreGeometry(settings.value("geometry", currentDevConsole->saveGeometry()).toByteArray());
    settings.endGroup();

    DevConsole::log("Welcome to Developer Console of PGE Editor!\nType 'help' to get more about console commands");
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

void DevConsole::logMessage(const QString &text, const QString &chan)
{
    logToConsole(text, chan, false);
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
    LogWriter::installConsole(this);

    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_builder.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_builder.ico"));

    QFont font = ui->plainTextEdit->font();
    font.setFamily("Courier New");
    ui->plainTextEdit->setFont(font);

    if(QSysInfo::WindowsVersion >= QSysInfo::WV_VISTA)
    {
        if(QtWin::isCompositionEnabled())
        {
            this->setAttribute(Qt::WA_TranslucentBackground, true);
            QtWin::extendFrameIntoClientArea(this, -1, -1, -1, -1);
            QtWin::enableBlurBehindWindow(this);
        }
        else
        {
            QtWin::resetExtendedFrame(this);
            setAttribute(Qt::WA_TranslucentBackground, false);
        }
    }
    #endif
    if(!hasFocus()) setWindowOpacity(0.9);
}

DevConsole::~DevConsole()
{
    LogWriter::uninstallConsole();
    delete ui;
}

void DevConsole::logToConsole(const QString &logText, const QString &channel, bool raise)
{
    QString target_channel = channel;

    if(channel == "System") //Prevent creation another "system" tab if switched another UI language
        target_channel = ui->tabWidget->tabText(0);

    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(ui->tabWidget->tabText(i) == target_channel)
        {
            QPlainTextEdit *tarEdit = getEditByIndex(i);
            if(!tarEdit)
                return;
            tarEdit->appendPlainText(logText);
            tarEdit->verticalScrollBar()->setValue(tarEdit->verticalScrollBar()->maximum());
            if(raise) ui->tabWidget->setCurrentIndex(i);
            return;
        }
    }
    //create new channel
    QWidget *w = new QWidget();
    auto *l = new QGridLayout(w);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    auto *e = new QPlainTextEdit(w);
    l->addWidget(e, 0, 0, 1, 1);
    auto *p = new QPushButton(w);
    l->addWidget(p, 1, 0, 1, 1);
    p->setFlat(true);
    QObject::connect(p, SIGNAL(clicked()), this, SLOT(clearCurrentLog()));
    p->setText(tr("Clear %1 Log").arg(target_channel));
    e->setReadOnly(true);
    e->setStyleSheet(ui->plainTextEdit->styleSheet());
    e->setFont(ui->plainTextEdit->font());
    e->appendPlainText(logText);
    e->verticalScrollBar()->setValue(e->verticalScrollBar()->maximum());
    ui->tabWidget->addTab(w, target_channel);
}

QPlainTextEdit *DevConsole::getEditByIndex(const int &index)
{
    return qobject_cast<QPlainTextEdit *>((dynamic_cast<QWidgetItem *>(ui->tabWidget->widget(index)->layout()->itemAt(0)))->widget());
}

QPlainTextEdit *DevConsole::getCurrentEdit()
{
    return getEditByIndex(ui->tabWidget->currentIndex());
}

void DevConsole::registerCommand(const QString &commandName, DevConsole::command cmd, const QString &helpText)
{
    commands[commandName.toLower()] = qMakePair<command, QString>(cmd, helpText);
}

void DevConsole::registerCommand(const std::initializer_list<QString> &commandNames, DevConsole::command cmd, const QString &helpText)
{
    for(const QString &tarCmd : commandNames)
        commands[tarCmd.toLower()] = qMakePair<command, QString>(cmd, helpText);
}

void DevConsole::on_button_clearAllLogs_clicked()
{
    for(int i = 0; i < ui->tabWidget->count(); ++i)
    {
        QPlainTextEdit *e = getEditByIndex(i);
        e->clear();
    }
}

void DevConsole::clearCurrentLog()
{
    getCurrentEdit()->clear();
}

void DevConsole::closeEvent(QCloseEvent *event)
{
    QString iniFile = AppPathManager::settingsFile();
    QSettings settings(iniFile, QSettings::IniFormat);

    settings.beginGroup("dev-console");
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
    registerCommand({"?", "help"}, &DevConsole::doHelp, tr("Prints this help text"));
    registerCommand("test", &DevConsole::doTest, tr("Prints a test command"));
    registerCommand("version", &DevConsole::doVersion, tr("Prints the editor version"));
    registerCommand("quit", &DevConsole::doQuit, tr("Quits the program"));
    registerCommand("savesettings", &DevConsole::doSaveSettings, tr("Saves the application settings"));
    registerCommand("md5", &DevConsole::doMd5, tr("Args: {SomeString} Calculates MD5 hash of string"));
    registerCommand("strarr", &DevConsole::doValidateStrArray, tr("Arg: {String array} validates the PGE-X string array"));
#ifdef ENABLE_CRASH_TESTS
    // Debug only commands, must be disabled in releases! (or Static Analyzers will swear!)
    registerCommand("crashme",  &DevConsole::doFakeCrash, tr("Simulates crash signal"));
    registerCommand("flood",    &DevConsole::doFlood, tr("Args: {[Number] Gigabytes} | Floods the memory with megabytes"));
    registerCommand("unhandle", &DevConsole::doThrowUnhandledException, tr("Throws an unhandled exception to crash the editor"));
    registerCommand("segserv",  &DevConsole::doSegmentationViolation, tr("Does a segmentation violation"));
    registerCommand("itemdialogmemleak",  &DevConsole::doMemLeakResearch, tr("Creates and deletes ItemSelectDialog to analyze memory leaking"));
#endif
    registerCommand("pgex", &DevConsole::doPgeXTest, tr("Arg: {Path to file} tests if the file is in the PGE-X file format"));
    registerCommand("playmusic", &DevConsole::doPlayMusic, tr("Args: {Music type (lvl wld spc), Music ID} Play default music by specific ID"));
    registerCommand("engine", &DevConsole::doSendCheat, tr("Args: {engine commands} Send a command or message into the PGE Engine if it's running"));
    registerCommand("paths", &DevConsole::doOutputPaths, tr("Shows various important paths!"));
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

    if(commands.contains(baseCommand.toLower()))
        (this->*(commands[baseCommand].first))(cmdList);
    else
        log("-> Unknown command. Type 'help' to show available command list", ui->tabWidget->tabText(0));
}

void DevConsole::doHelp(const QStringList &/*args*/)
{
    QStringList keys = commands.keys();
    for(int i = 0; i < keys.size(); ++i)
        log(keys[i] + QString(" - " + commands[keys[i]].second));
}

void DevConsole::doTest(const QStringList &/*args*/)
{
    log("-> All good!", ui->tabWidget->tabText(0));
}

void DevConsole::doPlayMusic(const QStringList &args)
{
    if(args.size() != 2)
    {
        log("-> Bad command syntax!", ui->tabWidget->tabText(0));
        return;
    }
    if(args[0] == "lvl")
        LvlMusPlay::setMusic(LvlMusPlay::LevelMusic, args[1].toLong(), "");
    else if(args[0] == "wld")
        LvlMusPlay::setMusic(LvlMusPlay::WorldMusic, args[1].toLong(), "");
    else if(args[0] == "spc")
        LvlMusPlay::setMusic(LvlMusPlay::SpecialMusic, args[1].toLong(), "");
    else
    {
        log("-> Unknown music type: (types are: lvl wld spc)!", ui->tabWidget->tabText(0));
        return;
    }

    MainWinConnect::pMainWin->setMusicButton(true);
    MainWinConnect::pMainWin->setMusic(true);
    log(QString("-> Music is playing: %1").arg(LvlMusPlay::currentMusicPath), ui->tabWidget->tabText(0));
}


void DevConsole::doMd5(const QStringList &args)
{
    QString src;

    for(const QString &s : args)
        src.append(s + (args.indexOf(s) < args.size() - 1 ? " " : ""));

    QString encoded = QString(QCryptographicHash::hash(src.toUtf8(), QCryptographicHash::Md5).toHex());

    log(QString("MD5 hash: %1").arg(encoded), ui->tabWidget->tabText(0));
}

void DevConsole::doValidateStrArray(const QStringList &args)
{
    QString src;

    for(const QString &s : args)
        src.append(s + (args.indexOf(s) < args.size() - 1 ? " " : ""));

    log(QString("%1").arg(src), ui->tabWidget->tabText(0));
    log(QString("String array is: %1").arg(PGEFile::IsStringArray(src) ? "valid" : "wrong"), ui->tabWidget->tabText(0));
}


void DevConsole::doVersion(const QStringList &/*args*/)
{
    log(QString("-> " V_FILE_DESC ", version " V_FILE_VERSION V_FILE_RELEASE), ui->tabWidget->tabText(0));
}

void DevConsole::doQuit(const QStringList &/*args*/)
{
    log("-> Bye-bye!", ui->tabWidget->tabText(0));
    MainWinConnect::pMainWin->close();
}

void DevConsole::doSaveSettings(const QStringList &/*args*/)
{
    MainWinConnect::pMainWin->saveSettings();
    log("-> Application Settings was saved!", ui->tabWidget->tabText(0));
}

#ifdef ENABLE_CRASH_TESTS
void DevConsole::doFakeCrash(const QStringList &/*args*/)
{
    CrashHandler::crashBySIGNAL(0);
}

void DevConsole::doFlood(const QStringList &args)
{
    if(!args.empty())
    {
        bool success;
        int floodSize = args[0].toInt(&success);
        if(!success)
            return;
        log("Flooding with " + QString::number(floodSize * 1024 * 1024) + "bytes");
        char *fl = new char[floodSize * 1024 * 1024];
        Q_UNUSED(fl)
        delete[] fl;//Delete this crap if application has been survived a flood
    }
}

void DevConsole::doThrowUnhandledException(const QStringList &/*args*/)
{
    throw std::runtime_error("Test Exception of Toast!");
}

void DevConsole::doSegmentationViolation(const QStringList&)
{
#ifndef _MSC_VER //Unfortunately MSVC swearing with hard erros on detecting this
    int *my_nullptr = nullptr;
    *my_nullptr = 42; //Answer to the Ultimate Question of Life, the Universe, and Everything will let you app crash >:D
#else
    log("-> This test isn't available in assembly built by MSVC! Please rebuild Editor in the another compiler (like MinGW or Intel C++ compiler)!", ui->tabWidget->tabText(0));
#endif
}

void DevConsole::doMemLeakResearch(const QStringList &args)
{
    if(args.empty())
    {
        log("-> No arguments set!", ui->tabWidget->tabText(0));
        return;
    }
    bool okk = false;
    unsigned int cycles = args[0].toUInt(&okk);
    if(!okk)
    {
        log("-> Invalid arguments!", ui->tabWidget->tabText(0));
        return;
    }

    log("-> Begin spam...", ui->tabWidget->tabText(0));
    for(unsigned int i = 0; i < cycles; i++)
    {
        auto *ddd = new ItemSelectDialog(&MainWinConnect::pMainWin->configs,
                                                     ItemSelectDialog::TAB_BLOCK|
                                                     ItemSelectDialog::TAB_BGO|
                                                     ItemSelectDialog::TAB_NPC|
                                                     ItemSelectDialog::TAB_TILE|
                                                     ItemSelectDialog::TAB_SCENERY|
                                                     ItemSelectDialog::TAB_PATH|
                                                     ItemSelectDialog::TAB_LEVEL|
                                                     ItemSelectDialog::TAB_MUSIC,
                                                     0, 0, 0, 0, 0, 0, 0, 0, 0, MainWinConnect::pMainWin, 0);
        //ddd->show();
        qApp->processEvents();
        //ddd->hide();
        ddd->close();
        delete ddd;
    }
    log("-> Spam completed!", ui->tabWidget->tabText(0));
}
#endif //DEBUG_BUILD


QString ______recourseBuildPGEX_Tree(QList<PGEFile::PGEX_Entry > &entry, int depth = 1)
{
    QString treeView = "";

    for(int i = 0; i < entry.size(); i++)
    {
        for(int k = 0; k < depth; k++) treeView += "--";
        treeView += QString("=============\n");
        for(int k = 0; k < depth; k++) treeView += "--";
        treeView += QString("SubTree name: %1\n").arg(entry[i].name);
        for(int k = 0; k < depth; k++) treeView += "--";
        treeView += QString("Branch type: %1\n").arg(entry[i].type);
        for(int k = 0; k < depth; k++) treeView += "--";
        treeView += QString("Entries: %1\n").arg(entry[i].data.size());
        if(entry[i].subTree.size() > 0)
            treeView += ______recourseBuildPGEX_Tree(entry[i].subTree, depth + 1);
    }

    return treeView;
}



void DevConsole::doPgeXTest(const QStringList &args)
{
    if(!args.isEmpty())
    {
        QString src;

        for(const QString &s : args)
            src.append(s + (args.indexOf(s) < args.size() - 1 ? " " : ""));

        QFile file(src);
        if(!file.open(QIODevice::ReadOnly))
        {
            log(QString("-> Error: Can't open the file!"));
            return;
        }

        QTextStream in(&file);   //Read File
        PGEFile pgeX_Data;
        QString raw = in.readAll();
        pgeX_Data.setRawData(raw);

        if(pgeX_Data.buildTreeFromRaw())
        {
            QString treeView = "";
            for(int i = 0; i < pgeX_Data.dataTree.size(); i++)
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

                if(!pgeX_Data.dataTree[i].subTree.empty())
                    treeView += ______recourseBuildPGEX_Tree(pgeX_Data.dataTree[i].subTree);
            }
            log(QString("-> File read:\nRaw size is %1\n%2").arg(raw.size()).arg(treeView),
                ui->tabWidget->tabText(0));

        }
        else
            log(QString("-> File invalid: %1").arg(pgeX_Data.lastError()), ui->tabWidget->tabText(0));

    }

}

void DevConsole::doSendCheat(const QStringList &args)
{
    QString src;
    for(const QString &s : args)
        src.append(s + (args.indexOf(s) < args.size() - 1 ? " " : ""));

    if(src.isEmpty())
    {
        log(QString("-> Can't run engine command without arguments"), ui->tabWidget->tabText(0));
        return;
    }
    src.replace('\n', ' ');
    src.replace("\\n", "\n");

    if(IntEngine::sendMessageBox(src))
        log(QString("-> command sent"), ui->tabWidget->tabText(0));
    else
        log(QString("-> Fail to send command: engine is not running"), ui->tabWidget->tabText(0));

}

void DevConsole::doOutputPaths(const QStringList &/*args*/)
{
    log(QString("App path: ") + AppPathManager::userAppDir());
    log(QString("Settings file: ") + AppPathManager::settingsFile());
    log(QString("Current log file: ") + LogWriter::DebugLogFile);
}
