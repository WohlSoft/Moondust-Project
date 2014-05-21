/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "ui_mainwindow.h"
#include "mainwindow.h"

#include "npc_dialog/npcdialog.h"
#include "main_window/appsettings.h"


MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    thread1 = new QThread;

    setDefaults(); // Apply default common settings

    QPixmap splashimg(":/splash.png");
    QSplashScreen splash(splashimg);
    splash.setCursor(Qt::ArrowCursor);
    splash.show();

    if(!configs.loadconfigs())
    {
        QMessageBox::critical(this, "Configuration error", "Configuration can't be load.\nSee in debug_log.txt for more information.", QMessageBox::Ok);
        splash.finish(this);
        WriteToLog(QtFatalMsg, "<Application emergency closed>");
        exit(EXIT_FAILURE);
        return;
    }

    splash.finish(this);

    ui->setupUi(this);

    /*
     * //Small test from https://qt-project.org/wiki/How_to_create_a_multi_language_application
     *
       QString defaultLocale = QLocale::system().name();
       defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

       m_langPath = QApplication::applicationDirPath();
       m_langPath.append("/languages");
       QDir dir(m_langPath);
       QStringList fileNames = dir.entryList(QStringList("pge_editor_*.qm"));

       for (int i = 0; i < fileNames.size(); ++i)
           {
               // get locale extracted by filename
               QString locale;
               locale = fileNames[i];                  // "TranslationExample_de.qm"
               locale.truncate(locale.lastIndexOf('.'));   // "TranslationExample_de"
               locale.remove(0, locale.indexOf('_') + 1);   // "de"

               QString lang = QLocale::languageToString(QLocale(locale).language());
               QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

               QAction *action = new QAction(ico, lang, this);
               action->setCheckable(true);
               action->setData(locale);

               ui->menuLanguage->addAction(action);

               // set default translators and language checked
               if (defaultLocale == locale)
               {
                   action->setChecked(true);
               }
           }

       m_currLang = "ru";
       QLocale locale = QLocale(m_currLang);
       QLocale::setDefault(locale);

       if(m_translator.load("pge_editor_ru.qm"))
        qApp->installTranslator(&m_translator);

       ui->retranslateUi(this);
    */

    setUiDefults(); //Apply default UI settings

}

//Scene Event Detector
void MainWindow::TickTack()
{
    if(TickTackLock) return;

    TickTackLock = true;

    try
    {
        if(activeChildWindow()==1)
        {
            if(activeLvlEditWin()->sceneCreated)
            {
                //Capturing flags from active Window
                /*if(activeLvlEditWin()->scene->wasPasted)
                {
                    activeLvlEditWin()->changeCursor(0);
                    activeLvlEditWin()->scene->wasPasted=false;
                    activeLvlEditWin()->scene->disableMoveItems=false;
                }
                else
                if(activeLvlEditWin()->scene->doCut)
                {
                    on_actionCut_triggered();
                    activeLvlEditWin()->scene->doCut=false;
                }
                else
                if(activeLvlEditWin()->scene->doCopy)
                {
                    on_actionCopy_triggered();
                    activeLvlEditWin()->scene->doCopy=false;
                }
                else*/
                if(activeLvlEditWin()->scene->historyChanged)
                {
                    ui->actionUndo->setEnabled( activeLvlEditWin()->scene->canUndo() );
                    ui->actionRedo->setEnabled( activeLvlEditWin()->scene->canRedo() );
                    activeLvlEditWin()->scene->historyChanged = false;
                }
                /*
                else
                if(activeLvlEditWin()->scene->resetPosition)
                {
                    on_actionReset_position_triggered();
                    activeLvlEditWin()->scene->resetPosition = false;
                }
                else
                if(activeLvlEditWin()->scene->SyncLayerList)
                {
                    setLayersBox();
                    activeLvlEditWin()->scene->SyncLayerList = false;
                }
                else
                if(activeLvlEditWin()->scene->resetResizingSection)
                {
                    ui->ResizeSection->setVisible(true);
                    ui->applyResize->setVisible(false);
                    ui->cancelResize->setVisible(false);
                    activeLvlEditWin()->scene->resetResizingSection = false;
                }*/
            }
        }
        /*
        else
        if(activeChildWindow()==2)
        {
            if(activeNpcEditWin()->NpcData.ReadFileValid);
        }*/
    }
    catch(int e)
    {
        WriteToLog(QtWarningMsg, QString("CLASS TYPE MISMATCH IN TIMER ON WINDOWS SWITCH: %1").arg(e));
    }

    TickTackLock = false;
}

MainWindow::~MainWindow()
{
    TickTackLock = false;
    delete ui;
    WriteToLog(QtDebugMsg, "--> Application closed <--");
}



//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_triggered()
{
    MainWindow::close();
    exit(0);
}

//Open About box
void MainWindow::on_actionAbout_triggered()
{
    aboutDialog about;
    about.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    about.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, about.size(), qApp->desktop()->availableGeometry()));
    about.exec();
}

//Application settings
void MainWindow::on_actionApplication_settings_triggered()
{
    AppSettings * appSettings = new AppSettings;
    appSettings->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    appSettings->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, appSettings->size(), qApp->desktop()->availableGeometry()));

    appSettings->autoPlayMusic = autoPlayMusic;
    appSettings->Animation = LvlOpts.animationEnabled;
    appSettings->Collisions = LvlOpts.collisionsEnabled;
    appSettings->AnimationItemLimit = animatorItemsLimit;

    appSettings->applySettings();

    if(appSettings->exec()==QDialog::Accepted)
    {
        autoPlayMusic = appSettings->autoPlayMusic;
        animatorItemsLimit = appSettings->AnimationItemLimit;
        LvlOpts.animationEnabled = appSettings->Animation;
        LvlOpts.collisionsEnabled = appSettings->Collisions;

        ui->actionAnimation->setChecked(LvlOpts.animationEnabled);
        on_actionAnimation_triggered(LvlOpts.animationEnabled);
        ui->actionCollisions->setChecked(LvlOpts.collisionsEnabled);
        on_actionCollisions_triggered(LvlOpts.collisionsEnabled);

        saveSettings();
    }

}


////////////////////////New files templates///////////////////////////

void MainWindow::on_actionNewNPC_config_triggered()
{

    NpcDialog * npcList = new NpcDialog(&configs);
    npcList->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    npcList->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, npcList->size(), qApp->desktop()->availableGeometry()));
    npcList->setState(0, 1);
    npcList->setWindowTitle("Create new NPC.txt configuration file");
    if(npcList->exec()==QDialog::Accepted)
    {
        npcedit *child = createNPCChild();
        child->newFile( npcList->selectedNPC);
        child->show();
    }

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
