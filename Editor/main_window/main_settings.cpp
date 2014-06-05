/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include "../common_features/logger_sets.h"

#include "appsettings.h"

#include "music_player.h"
#include "global_settings.h"

QString GlobalSettings::locale="";
long GlobalSettings::animatorItemsLimit=10000;

QString LvlMusPlay::currentCustomMusic;
long LvlMusPlay::currentMusicId;
bool LvlMusPlay::musicButtonChecked;
bool LvlMusPlay::musicForceReset=false;


void MainWindow::setDefLang()
{
    /*
     * //Small test from https://qt-project.org/wiki/How_to_create_a_multi_language_application
     */
    WriteToLog(QtDebugMsg, QString("Lang->Translator loaging...."));

    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
            GlobalSettings::locale = settings.value("language", defaultLocale).toString();
    settings.endGroup();

    WriteToLog(QtDebugMsg, QString("Lang->config was loaded"));

    WriteToLog(QtDebugMsg, QString("Lang->Setting slot...."));
    connect(ui->menuLanguage, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));
    WriteToLog(QtDebugMsg, QString("Lang->seted"));

       m_langPath = QApplication::applicationDirPath();
       m_langPath.append("/languages");

       langListSync();

       m_currLang = GlobalSettings::locale;
       QLocale locale = QLocale(m_currLang);
       QLocale::setDefault(locale);

       bool ok = m_translator.load(m_langPath + QString("/editor_%1.qm").arg(m_currLang));
                WriteToLog(QtDebugMsg, QString("Translation: %1").arg((int)ok));
       if(ok)
        qApp->installTranslator(&m_translator);

       ok = m_translatorQt.load(m_langPath + QString("/qt_%1.qm").arg(m_currLang));
                WriteToLog(QtDebugMsg, QString("QT Translation: %1").arg((int)ok));
       if(ok)
        qApp->installTranslator(&m_translatorQt);

       ui->retranslateUi(this);
}

void MainWindow::langListSync()
{
    // format systems language
    ui->menuLanguage->clear();

    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("editor_*.qm"));
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

            WriteToLog(QtDebugMsg, QString("Locale: %1 %2").arg(m_langPath).arg(locale));

            ui->menuLanguage->addAction(action);

            if (GlobalSettings::locale == locale)
            {
                action->setChecked(true);
            }
        }

    if(fileNames.size()==0)
    {
        QAction *action = ui->menuLanguage->addAction("[translates was not loaded!]");
        action->setCheckable(false);
        action->setDisabled(true);
    }

}

void MainWindow::slotLanguageChanged(QAction* action)
{
    WriteToLog(QtDebugMsg, QString("Translation->SlotStarted"));
    if(0 != action)
    {
        // load the language dependant on the action content
        GlobalSettings::locale = m_currLang;
        loadLanguage(action->data().toString());
    }
}

bool MainWindow::switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);
    // load the new translator
    bool ok = translator.load(filename);
    WriteToLog(QtDebugMsg, QString("Translation: %1 %2").arg((int)ok).arg(filename));

    if(ok)
        qApp->installTranslator(&translator);
    WriteToLog(QtDebugMsg, QString("Translation-> changed"));
    return ok;
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        GlobalSettings::locale = m_currLang;

        QLocale::setDefault(locale);

        QString languageName = QLocale::languageToString(locale.language());

        bool ok = switchTranslator(m_translatorQt, m_langPath + QString("/qt_%1.qm").arg(m_currLang));
             ok = switchTranslator(m_translator, m_langPath + QString("/editor_%1.qm").arg(m_currLang));

        WriteToLog(QtDebugMsg, QString("Translation->try to retranslate"));

        if(ok)
        {
            ui->retranslateUi(this);
            WriteToLog(QtDebugMsg, QString("Translation-> done"));
        }
        else
               WriteToLog(QtDebugMsg, QString("Translation-> not changed (not okay)"));

        //Sync dynamic menus
        SyncRecentFiles();
        updateWindowMenu();
        langListSync();

        ui->statusBar->showMessage(tr("Current Language changed to %1").arg(languageName));
    }
}

/*
void MainWindow::changeEvent(QEvent* event)
{
    if(NULL != event)
    {
        switch(event->type())
        {
        // this event is send if a translator is loaded
        case QEvent::LanguageChange:
            ui->retranslateUi(this);
            break;
        // this event is send, if the system, language changes
        case QEvent::LocaleChange:
            {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                loadLanguage(locale);
            }
            break;
        default:
            break;
        }
    }

    QMainWindow::changeEvent(event);
}*/




void MainWindow::setDefaults()
{
    setPointer();

    MusicPlayer = new QMediaPlayer;

    LvlOpts.animationEnabled = true;
    LvlOpts.collisionsEnabled = true;

    LastOpenDir = ".";
    lastWinType=0;
    LevelToolBoxVis = true; //Level toolbox
    SectionToolBoxVis = false; //Section Settings
    LevelDoorsBoxVis = false; //Doors box
    LevelLayersBoxVis = false; //Layers box

    WorldToolBoxVis = false;
    autoPlayMusic = false;

    LvlMusPlay::currentCustomMusic = "";
    LvlMusPlay::currentMusicId = 0;
    LvlMusPlay::musicButtonChecked = false;

    cat_blocks="[all]";
    cat_bgos="[all]";
    cat_npcs="[all]";
}

void MainWindow::setUiDefults()
{
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/images/mac/mushroom.icns"));
    #endif

    //Applay objects into tools
    setLevelSectionData();
    setItemBoxes();

    loadSettings();

    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
        this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
        this, SLOT(setActiveSubWindow(QWidget*)));

    ui->actionPlayMusic->setChecked(autoPlayMusic);

    ui->centralWidget->cascadeSubWindows();

    ui->applyResize->setVisible(false);
    ui->cancelResize->setVisible(false);


    ui->WorldToolBox->hide();
    ui->LevelToolBox->hide();


    ui->DoorsToolbox->hide();
    ui->LevelLayers->hide();
    ui->LevelSectionSettings->hide();

    ui->ItemProperties->hide();


    ui->menuView->setEnabled(0);

    ui->menuWindow->setEnabled(1);

    ui->menuLevel->setEnabled(0);
    ui->menuWorld->setEnabled(0);
    ui->LevelObjectToolbar->setVisible(0);

    ui->actionLVLToolBox->setVisible(0);
    ui->actionWarpsAndDoors->setVisible(0);
    ui->actionSection_Settings->setVisible(0);
    ui->actionWarpsAndDoors->setVisible(0);
    ui->actionWLDToolBox->setVisible(0);
    ui->actionGridEn->setChecked(1);

    setAcceptDrops(true);
    ui->centralWidget->cascadeSubWindows();


    //Start event detector
    TickTackLock = false;

    //set timer for event detector loop
    TickTackTimer = new QTimer(this);
    connect(
            TickTackTimer, SIGNAL(timeout()),
            this,
            SLOT( TickTack() ) );

    //start event detection loop
    TickTackTimer->start(1);

    connect(ui->LvlLayerList->model(), SIGNAL(rowsMoved(QModelIndex,int,int,QModelIndex,int)), this, SLOT(DragAndDroppedLayer(QModelIndex,int,int,QModelIndex,int)));
}


//////////Load settings from INI file///////////////
void MainWindow::loadSettings()
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
        LastOpenDir = settings.value("lastpath", ".").toString();
        LevelToolBoxVis = settings.value("level-tb-visible", "true").toBool();
        WorldToolBoxVis = settings.value("world-tb-visible", "true").toBool();
        SectionToolBoxVis = settings.value("section-tb-visible", "false").toBool();
        LevelDoorsBoxVis = settings.value("level-doors-vis", "false").toBool();
        LevelLayersBoxVis = settings.value("level-layers-vis", "false").toBool();

        LvlOpts.animationEnabled = settings.value("animation", "true").toBool();
        LvlOpts.collisionsEnabled = settings.value("collisions", "true").toBool();
        restoreGeometry(settings.value("geometry", saveGeometry() ).toByteArray());
        restoreState(settings.value("windowState", saveState() ).toByteArray());
        autoPlayMusic = settings.value("autoPlayMusic", false).toBool();

        ui->DoorsToolbox->setFloating(settings.value("doors-tool-box-float", true).toBool());
        ui->LevelSectionSettings->setFloating(settings.value("level-section-set-float", true).toBool());
        ui->LevelLayers->setFloating(settings.value("level-layers-float", true).toBool());
        ui->ItemProperties->setFloating(settings.value("item-props-box-float", true).toBool());

        ui->DoorsToolbox->restoreGeometry(settings.value("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry()).toByteArray());
        ui->LevelSectionSettings->restoreGeometry(settings.value("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry()).toByteArray());
        ui->LevelLayers->restoreGeometry(settings.value("level-layers-geometry", ui->LevelLayers->saveGeometry()).toByteArray());
        ui->ItemProperties->restoreGeometry(settings.value("item-props-box-geometry", ui->ItemProperties->saveGeometry()).toByteArray());

        GlobalSettings::animatorItemsLimit = settings.value("animation-item-limit", "10000").toInt();

    settings.endGroup();

    settings.beginGroup("Recent");
        for(int i = 1; i<=10;i++){
            recentOpen.push_back(settings.value("recent"+QString::number(i),"<empty>").toString());
        }
        SyncRecentFiles();
    settings.endGroup();
}

//////////Save settings into INI file///////////////
void MainWindow::saveSettings()
{
    QString inifile = QApplication::applicationDirPath() + "/" + "pge_editor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    settings.setValue("pos", pos());
    settings.setValue("lastpath", LastOpenDir);

    settings.setValue("level-tb-visible", LevelToolBoxVis);
    settings.setValue("world-tb-visible", WorldToolBoxVis);
    settings.setValue("section-tb-visible", SectionToolBoxVis);
    settings.setValue("level-layers-vis", LevelLayersBoxVis);
    settings.setValue("level-doors-vis", LevelDoorsBoxVis);

    settings.setValue("doors-tool-box-float", ui->DoorsToolbox->isFloating());
    settings.setValue("level-section-set-float", ui->LevelSectionSettings->isFloating());
    settings.setValue("level-layers-float", ui->LevelLayers->isFloating());
    settings.setValue("item-props-box-float", ui->ItemProperties->isFloating());

    settings.setValue("doors-tool-box-geometry", ui->DoorsToolbox->saveGeometry());
    settings.setValue("level-section-set-geometry", ui->LevelSectionSettings->saveGeometry());
    settings.setValue("level-layers-geometry", ui->LevelLayers->saveGeometry());
    settings.setValue("item-props-box-geometry", ui->ItemProperties->saveGeometry());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("autoPlayMusic", autoPlayMusic);

    settings.setValue("animation", LvlOpts.animationEnabled);
    settings.setValue("collisions", LvlOpts.collisionsEnabled);
    settings.setValue("animation-item-limit", QString::number(GlobalSettings::animatorItemsLimit));

    settings.setValue("language", GlobalSettings::locale);

    settings.endGroup();

    settings.beginGroup("Recent");
    for(int i = 1; i<=10;i++){
        settings.setValue("recent"+QString::number(i),recentOpen[i-1]);
    }
    settings.endGroup();

    settings.beginGroup("logging");
        settings.setValue("log-path", LogWriter::DebugLogFile);

        if(LogWriter::enabled)
            switch(LogWriter::logLevel)
            {
            case QtDebugMsg:
                settings.setValue("log-level", "4"); break;
            case QtWarningMsg:
                settings.setValue("log-level", "3"); break;
            case QtCriticalMsg:
                settings.setValue("log-level", "2"); break;
            case QtFatalMsg:
                settings.setValue("log-level", "1"); break;
            }
        else
            settings.setValue("log-level", "0");
    settings.endGroup();
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

    appSettings->AnimationItemLimit = GlobalSettings::animatorItemsLimit;

    appSettings->applySettings();

    if(appSettings->exec()==QDialog::Accepted)
    {
        autoPlayMusic = appSettings->autoPlayMusic;
        GlobalSettings::animatorItemsLimit = appSettings->AnimationItemLimit;
        LvlOpts.animationEnabled = appSettings->Animation;
        LvlOpts.collisionsEnabled = appSettings->Collisions;

        ui->actionAnimation->setChecked(LvlOpts.animationEnabled);
        on_actionAnimation_triggered(LvlOpts.animationEnabled);
        ui->actionCollisions->setChecked(LvlOpts.collisionsEnabled);
        on_actionCollisions_triggered(LvlOpts.collisionsEnabled);

        saveSettings();
    }

}
