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

#include <common_features/app_path.h>
#include <common_features/themes.h>
#include <common_features/spash_screen.h>
#include <data_configs/config_manager.h>
#include <main_window/dock/toolboxes.h>
#include <common_features/logger_sets.h>

#include <ui_mainwindow.h>
#include "mainwindow.h"

#include <QtConcurrent>

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    continueLoad = false;

    this->setAttribute(Qt::WA_QuitOnClose, true);
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    this->hide();
    setDefaults(); // Apply default common settings

    WriteToLog(QtDebugMsg, QString("Set UI..."));
    ui->setupUi(this);

    WriteToLog(QtDebugMsg, QString("Setting Lang..."));
    setDefLang();

    setUiDefults(); //Apply default UI settings

#ifdef Q_OS_MACX
    ui->menuBar->setEnabled(false);
#endif

    //Create empty config directory if not exists
    if(!QDir(AppPathManager::userAppDir() + "/" +  "configs").exists())
        QDir().mkdir(AppPathManager::userAppDir() + "/" +  "configs");


    // Config manager
    ConfigManager *cmanager;
    cmanager = new ConfigManager(this);
    cmanager->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    cmanager->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, cmanager->size(), qApp->desktop()->availableGeometry()));
    QString configPath = cmanager->isPreLoaded();
    currentConfigDir = configPath;

    askConfigAgain = cmanager->askAgain;

    QString tPack = cmanager->themePack;
    //If application runned first time or target configuration is not exist
    if( (askConfigAgain) || ( configPath.isEmpty() ) )
    {
        //Ask for configuration
        if(cmanager->exec()==QDialog::Accepted)
        {
            configPath = cmanager->currentConfigPath;
        }
        else
        {
            delete cmanager;
            WriteToLog(QtWarningMsg, "<Configuration is not selected>");
            continueLoad = false;
            return;
        }
    }
    //continueLoad = true;
    askConfigAgain = cmanager->askAgain;
    currentConfigDir = cmanager->currentConfigPath;

    delete cmanager;

    configs.setConfigPath(configPath);

    configs.loadBasics();
    Themes::loadTheme(tPack);

    /*********************Splash Screen**********************/
    QPixmap splashimg(configs.splash_logo.isEmpty()?
                      Themes::Image(Themes::splash) :
                      configs.splash_logo);

    EditorSpashScreen splash(splashimg);
    splash.setCursor(Qt::ArrowCursor);
    splash.setDisabled(true);
    splash.setWindowFlags( splash.windowFlags() |  Qt::WindowStaysOnTopHint );


    for(int a=0; a<configs.animations.size();a++)
    {
        //QPoint pt(416,242);
        QPoint pt(configs.animations[a].x, configs.animations[a].y);
        //QPixmap img = QPixmap("coin.png");
        splash.addAnimation(pt,
                            configs.animations[a].img,
                            configs.animations[a].frames,
                            configs.animations[a].speed);
    }

    /*********************Loading of config pack**********************/
    // Do the loading in a thread
    QFuture<bool> isOk = QtConcurrent::run(&this->configs, &dataconfigs::loadconfigs);
    /*********************Loading of config pack**********************/

    /*********************Splash Screen**********************/
    // And meanwhile load the settings in the main thread
    loadSettings();

    #ifndef Q_OS_ANDROID
    splash.show();
    #else
    splash.showFullScreen();
    #endif
    splash.startAnimations();

    // Now wait until the config load in finished.
    while(!isOk.isFinished()) { qApp->processEvents(); QThread::msleep(64);}

    /*********************Splash Screen end**********************/
    splash.finish(this);
    /*********************Splash Screen end**********************/
    if(!isOk.result())
    {
        QMessageBox::critical(this, tr("Configuration error"),
                              tr("Configuration can't be loaded.\nSee in %1 for more information.").arg(LogWriter::DebugLogFile), QMessageBox::Ok);
        WriteToLog(QtFatalMsg, "<Error, application closed>");
        continueLoad = false;
        return;
    }

    if(configs.check())
    {
        QMessageBox::warning(this, tr("Configuration error"),
            tr("Configuration package is loaded with errors."), QMessageBox::Ok);
        on_actionCurConfig_triggered();
    }

    continueLoad = true;

    applyTheme(Themes::currentTheme().isEmpty() ? ConfStatus::defaultTheme : Themes::currentTheme());

#ifdef Q_OS_MACX
    ui->menuBar->setEnabled(true);
#endif

    //Apply objects into tools
    dock_LvlSectionProps->initDefaults();
    dock_LvlItemBox->setLvlItemBoxes();
    dock_WldItemBox->setWldItemBoxes();
    dock_LvlEvents->reloadSoundsList();
    dock_WldItemProps->WldLvlExitTypeListReset();
    dock_TilesetBox->setTileSetBox(true);
}

MainWindow::~MainWindow()
{
#ifdef Q_OS_WIN
    if(pge_thumbbar)
        delete pge_thumbbar;
#endif
    delete ui;
}



//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_triggered()
{
    if(!MainWindow::close())
        return;
}

void MainWindow::formatErrorMsgBox(QString filePath, QString errorMessage, int lineNum, QString lineContents)
{
    QMessageBox::warning(this, QTranslator::tr("Bad File"),
                PGESTRING( QTranslator::tr("Bad file format\nFile: %1\n").arg(filePath)  //Print Bad data string
                        +( errorMessage.isEmpty()?"":errorMessage+"\n" ) //Print special error message
                        +( lineNum < 0 ? "":QTranslator::tr("Line Number: %1\n").arg(lineNum))         //Print Line With error
                        +( lineContents.isEmpty()?"":QTranslator::tr("Line Data: %1").arg(lineContents)) ),
                         QMessageBox::Ok);
}

//Toolbar context menu
void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
    WriteToLog(QtDebugMsg, QString("Main Menu's context menu called! %1 %2").arg(pos.x()).arg(pos.y()));

    QMenu *cu = new QMenu(this);
    QAction *test= cu->addAction("Nothing");
    test->setEnabled(false);
    QAction *test2= cu->addAction("Nothing");
    test2->setEnabled(false);
    QAction *test3= cu->addAction("Nothing");
    test3->setEnabled(false);

    cu->exec(pos);
}


void MainWindow::showStatusMsg(QString msg, int time)
{
    statusBar()->showMessage(msg, time);
}

void MainWindow::on_actionRefresh_menu_and_toolboxes_triggered()
{
    updateMenus(true);
}
