#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "levelprops.h"
#include <QFileDialog>
#include <QtGui>
#include <QFile>
#include <QSettings>
#include <QTranslator>
#include <QLocale>
#include <QSplashScreen>
#include "childwindow.h"
#include "leveledit.h"
#include "npcedit.h"
#include "dataconfigs.h"

QString LastOpenDir = ".";

dataconfigs configs;

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    QPixmap splashimg(":/images/splash.png");
    QSplashScreen splash(splashimg);
    splash.show();

    configs.loadconfigs();

    ui->setupUi(this);

    splash.finish(this);


    connect(ui->centralWidget, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));

    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    //resize(settings.value("size", size()).toSize());
    //move(settings.value("pos", pos()).toPoint());
    LastOpenDir = settings.value("lastpath", ".").toString();
    //if(settings.value("maximased", "false")=="true") showMaximized();
    //"lvl-section-view", dockWidgetArea(ui->LevelSectionSettings)
    //dockWidgetArea();

    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());

    ui->centralWidget->cascadeSubWindows();
    ui->WorldToolBox->hide();
    ui->LevelSectionSettings->hide();
    ui->LevelToolBox->hide();

    ui->menuView->setEnabled(0);
    ui->menuWindow->setEnabled(0);
    ui->menuLevel->setEnabled(0);
    ui->menuWorld->setEnabled(0);

    ui->actionLVLToolBox->setVisible(0);
    ui->actionSection_Settings->setVisible(0);
    ui->actionWLDToolBox->setVisible(0);

    setAcceptDrops(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateMenus()
{
    int WinType = activeChildWindow(); // 1 lvledit, 2 npcedit, 3 wldedit
    bool hasSWindow = (WinType != 0);

    ui->actionSave->setEnabled(hasSWindow);
    ui->actionSave_as->setEnabled(hasSWindow);
    ui->actionSave_all->setEnabled(hasSWindow);
    ui->actionClose->setEnabled(hasSWindow);

    ui->menuView->setEnabled( (hasSWindow) && (WinType!=2) );

    ui->actionSelect->setEnabled( (WinType==1) || (WinType==3));
    ui->actionEriser->setEnabled( (WinType==1) || (WinType==3));

    ui->WorldToolBox->setVisible( (WinType==3) );
    ui->menuWorld->setEnabled(( WinType==3) );
    ui->actionWLDToolBox->setVisible( (WinType==3) );


    ui->LevelToolBox->setVisible( (WinType==1) );
    ui->actionLVLToolBox->setVisible( (WinType==1) );
    ui->menuLevel->setEnabled( (WinType==1) );
    ui->actionSection_Settings->setVisible( (WinType==1) );
    ui->LevelSectionSettings->setVisible( (WinType==1) );
    ui->actionLevelProp->setEnabled( (WinType==1) );
    ui->actionLevNoBack->setEnabled( (WinType==1) );
    ui->actionLevOffScr->setEnabled( (WinType==1) );
    ui->actionLevWarp->setEnabled( (WinType==1) );
    ui->actionLevUnderW->setEnabled( (WinType==1) );

    ui->actionSection_1->setEnabled( (WinType==1) );
    ui->actionSection_2->setEnabled( (WinType==1) );
    ui->actionSection_3->setEnabled( (WinType==1) );
    ui->actionSection_4->setEnabled( (WinType==1) );
    ui->actionSection_5->setEnabled( (WinType==1) );
    ui->actionSection_6->setEnabled( (WinType==1) );
    ui->actionSection_7->setEnabled( (WinType==1) );
    ui->actionSection_8->setEnabled( (WinType==1) );
    ui->actionSection_9->setEnabled( (WinType==1) );
    ui->actionSection_10->setEnabled( (WinType==1) );
    ui->actionSection_11->setEnabled( (WinType==1) );
    ui->actionSection_12->setEnabled( (WinType==1) );
    ui->actionSection_13->setEnabled( (WinType==1) );
    ui->actionSection_14->setEnabled( (WinType==1) );
    ui->actionSection_15->setEnabled( (WinType==1) );
    ui->actionSection_16->setEnabled( (WinType==1) );
    ui->actionSection_17->setEnabled( (WinType==1) );
    ui->actionSection_18->setEnabled( (WinType==1) );
    ui->actionSection_19->setEnabled( (WinType==1) );
    ui->actionSection_20->setEnabled( (WinType==1) );
    ui->actionSection_21->setEnabled( (WinType==1) );

    if(WinType==1) SetCurrentLevelSection(0, 1);

    ui->menuWindow->setEnabled( (WinType!=0) );

    /*
    closeAllAct->setEnabled(hasSWindow);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasSWindow);
    nextAct->setEnabled(hasSWindow);
    previousAct->setEnabled(hasSWindow);
    separatorAct->setVisible(hasSWindow);
    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
    */
}

void MainWindow::save()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->save();
        if(WinType==1)
            saved = activeLvlEditWin()->save();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_as()
{
    bool saved=false;
    int WinType = activeChildWindow();
    if (WinType!=0)
    {
        if(WinType==2)
            saved = activeNpcEditWin()->saveAs();
        if(WinType==1)
            saved = activeLvlEditWin()->saveAs();

        if(saved) statusBar()->showMessage(tr("File saved"), 2000);
    }
}

void MainWindow::save_all()
{
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
            ChildWindow0->save();
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
            ChildWindow2->save();
        }

    }
}

void MainWindow::close_sw()
{
    ui->centralWidget->activeSubWindow()->close();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        //qDebug() << "Dropped file:" << fileName;
        OpenFile(fileName);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ui->centralWidget->closeAllSubWindows();


    if (ui->centralWidget->currentSubWindow()) {
        event->ignore();

    }
    else
    {
    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";

    QSettings settings(inifile, QSettings::IniFormat);
    settings.beginGroup("Main");
    if(!isMaximized())
        settings.setValue("size", size());
    settings.setValue("pos", pos());
    settings.setValue("lastpath", LastOpenDir);
    settings.setValue("maximased", isMaximized());
    //settings.setValue("lvl-section-view", dockWidgetArea(ui->LevelSectionSettings) );
    //settings.setValue("wld-toolbox-view", ui->WorldToolBox->isVisible());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.endGroup();
    event->accept();
    }
}

///////////////////////////////////////////////////////


//////////////////////////////////////////////////////

void MainWindow::on_OpenFile_activated()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),LastOpenDir,
        tr("All SMBX files (*.LVL *.WLD npc-*.TXT *.INI)\n"
        "SMBX Level (*.LVL)\n"
        "SMBX World (*.WLD)\n"
        "SMBX NPC Config (npc-*.TXT)\n"
        "All Files (*.*)"));

        if(fileName_DATA==NULL) return;

        OpenFile(fileName_DATA);

}

void MainWindow::OpenFile(QString FilePath)
{

    QFile file(FilePath);


    QMdiSubWindow *existing = findMdiChild(FilePath);
            if (existing) {
                ui->centralWidget->setActiveSubWindow(existing);
                return;
            }


    if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("File open error"),
    tr("Can't open the file."), QMessageBox::Ok);
        return;
    }

    QFileInfo in_1(FilePath);

    LastOpenDir = in_1.absoluteDir().absolutePath();

    if(in_1.suffix() == "lvl")
    {

        LevelData FileData = ReadLevelFile(file); //function in file_formats.cpp
        if( !FileData.ReadFileValid ) return;

        FileData.filename = in_1.baseName();
        FileData.path = in_1.absoluteDir().absolutePath();

        leveledit *child = createChild();
        if (child->loadFile(FilePath, FileData, configs)) {
            statusBar()->showMessage(tr("Level file loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
    else
    if(in_1.suffix() == "wld")
    {
        WorldData FileData = ReadWorldFile(file);
        if( !FileData.ReadFileValid ) return;

        /*
        leveledit *child = createChild();
        if (child->loadFile(FilePath)) {
            statusBar()->showMessage(tr("World map file loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
        */

    }
    else
    if(in_1.suffix() == "txt")
    {
        NPCConfigFile FileData = ReadNpcTXTFile(file);
        if( !FileData.ReadFileValid ) return;

        npcedit *child = createNPCChild();
        if (child->loadFile(FilePath, FileData)) {
            statusBar()->showMessage(tr("NPC Config loaded"), 2000);
            child->show();
        } else {
            child->close();
        }
    }
    else
    {
    QMessageBox::warning(this, tr("Bad file"),
     tr("This file have unknown extension"),
     QMessageBox::Ok);
    return;
    }

/*
QMessageBox::information(this, tr("File checked"),
tr("This file is good"),
QMessageBox::Ok);
*/

}

//Edit NPC
npcedit *MainWindow::createNPCChild()
{
    npcedit *child = new npcedit;
    QMdiSubWindow *npcWindow = new QMdiSubWindow;
    npcWindow->setWidget(child);
    npcWindow->setFixedSize(520,640);
    npcWindow->setAttribute(Qt::WA_DeleteOnClose);
    npcWindow->setWindowFlags(Qt::WindowCloseButtonHint);
    npcWindow->move(rand()%100,rand()%50);
    ui->centralWidget->addSubWindow(npcWindow);

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}



//Edit LEVEL
leveledit *MainWindow::createChild()
{
    leveledit *child = new leveledit;
    ui->centralWidget->addSubWindow(child)->resize(QSize(800, 602));
        return child;
}



int MainWindow::activeChildWindow()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
    {
        if(QString(activeSubWindow->widget()->metaObject()->className())=="leveledit")
            return 1;
        if(QString(activeSubWindow->widget()->metaObject()->className())=="npcedit")
            return 2;
    }

    return 0;
}

npcedit *MainWindow::activeNpcEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<npcedit *>(activeSubWindow->widget());
    return 0;
}

leveledit *MainWindow::activeLvlEditWin()
{
    if (QMdiSubWindow *activeSubWindow = ui->centralWidget->activeSubWindow())
        return qobject_cast<leveledit *>(activeSubWindow->widget());
    return 0;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();
    leveledit *ChildWindow0;
    npcedit *ChildWindow2;

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList())
    {
        if(QString(window->widget()->metaObject()->className())=="leveledit")
        {
        ChildWindow0 = qobject_cast<leveledit *>(window->widget());
        if (ChildWindow0->currentFile() == canonicalFilePath)
            return window;
        }
        else if(QString(window->widget()->metaObject()->className())=="npcedit")
        {
        ChildWindow2 = qobject_cast<npcedit *>(window->widget());
        if (ChildWindow2->currentFile() == canonicalFilePath)
            return window;
        }

    }
    return 0;
}


void MainWindow::setActiveSubWindow(QWidget *window)
{
    if (!window)
        return;
    ui->centralWidget->setActiveSubWindow(qobject_cast<QMdiSubWindow *>(window));
}

/*
void MainWindow::updateMenus()
{
    bool hasMdiChild = (activeMdiChild() != 0);
    saveAct->setEnabled(hasMdiChild);
    saveAsAct->setEnabled(hasMdiChild);
    pasteAct->setEnabled(hasMdiChild);
    closeAct->setEnabled(hasMdiChild);
    closeAllAct->setEnabled(hasMdiChild);
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    bool hasSelection = (activeMdiChild() &&
                         activeMdiChild()->textCursor().hasSelection());
    cutAct->setEnabled(hasSelection);
    copyAct->setEnabled(hasSelection);
}

void MainWindow::updateWindowMenu()
{
    windowMenu->clear();
    windowMenu->addAction(closeAct);
    windowMenu->addAction(closeAllAct);
    windowMenu->addSeparator();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());

    for (int i = 0; i < windows.size(); ++i) {
        MdiChild *child = qobject_cast<MdiChild *>(windows.at(i)->widget());

        QString text;
        if (i < 9) {
            text = tr("&%1 %2").arg(i + 1)
                               .arg(child->userFriendlyCurrentFile());
        } else {
            text = tr("%1 %2").arg(i + 1)
                              .arg(child->userFriendlyCurrentFile());
        }
        QAction *action  = windowMenu->addAction(text);
        action->setCheckable(true);
        action ->setChecked(child == activeMdiChild());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}

*/


void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    int SectionId = SctId;
    int WinType = activeChildWindow();

    if ((open==1)&&(WinType==1)) // Only Set Checked section number without section select
    {
        SectionId = activeLvlEditWin()->LvlData.CurSection;
    }

    ui->actionSection_1->setChecked( (SectionId==0) );
    ui->actionSection_2->setChecked( (SectionId==1) );
    ui->actionSection_3->setChecked( (SectionId==2) );
    ui->actionSection_4->setChecked( (SectionId==3) );
    ui->actionSection_5->setChecked( (SectionId==4) );
    ui->actionSection_6->setChecked( (SectionId==5) );
    ui->actionSection_7->setChecked( (SectionId==6) );
    ui->actionSection_8->setChecked( (SectionId==7) );
    ui->actionSection_9->setChecked( (SectionId==8) );
    ui->actionSection_10->setChecked( (SectionId==9) );
    ui->actionSection_11->setChecked( (SectionId==10) );
    ui->actionSection_12->setChecked( (SectionId==11) );
    ui->actionSection_13->setChecked( (SectionId==12) );
    ui->actionSection_14->setChecked( (SectionId==13) );
    ui->actionSection_15->setChecked( (SectionId==14) );
    ui->actionSection_16->setChecked( (SectionId==15) );
    ui->actionSection_17->setChecked( (SectionId==16) );
    ui->actionSection_18->setChecked( (SectionId==17) );
    ui->actionSection_19->setChecked( (SectionId==18) );
    ui->actionSection_20->setChecked( (SectionId==19) );
    ui->actionSection_21->setChecked( (SectionId==20) );

    if ((WinType==1) && (open==0))
    {
       activeLvlEditWin()->setCurrentSection(SectionId);
    }
}


//////////////////SLOTS///////////////////////////

//Exit from application
void MainWindow::on_Exit_activated()
{
    MainWindow::close();
    exit(0);
}

//Open About box
void MainWindow::on_actionAbout_activated()
{
    aboutDialog about;
    about.setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    about.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, about.size(), qApp->desktop()->availableGeometry()));
    about.exec();
}


// Level tool box show/hide
void MainWindow::on_LevelToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionLVLToolBox->setChecked(true);
    }
     else
    {
        ui->actionLVLToolBox->setChecked(false);
    }
}

void MainWindow::on_actionLVLToolBox_activated()
{
        if(ui->actionLVLToolBox->isChecked())
            ui->LevelToolBox->setVisible(true);
        else
            ui->LevelToolBox->setVisible(false);
}

// World tool box show/hide
void MainWindow::on_WorldToolBox_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionWLDToolBox->setChecked(true);
    }
     else
    {
        ui->actionWLDToolBox->setChecked(false);
    }
}

void MainWindow::on_actionWLDToolBox_activated()
{
    if(ui->actionWLDToolBox->isChecked())
        ui->WorldToolBox->setVisible(true);
    else
        ui->WorldToolBox->setVisible(false);
}

// Level Section tool box show/hide
void MainWindow::on_LevelSectionSettings_visibilityChanged(bool visible)
{
    if(visible)
    {
        //MainWindow::menuBar()->actions()->;
        ui->actionSection_Settings->setChecked(true);
    }
     else
    {
        ui->actionSection_Settings->setChecked(false);
    }
}

void MainWindow::on_actionSection_Settings_activated()
{
    if(ui->actionSection_Settings->isChecked())
        ui->LevelSectionSettings->setVisible(true);
    else
        ui->LevelSectionSettings->setVisible(false);
}


//Open Level Properties
void MainWindow::on_actionLevelProp_activated()
{
    LevelProps LevProps;
    LevProps.exec();
}

void MainWindow::on_pushButton_4_clicked()
{
    //ui->xxxxlistWidget->insertItem(1, "test");
    //MyListItem *itm = new MyListItem;
    //insertItem(1, "test image");
    //itm->setText("My Item");
    //itm->icon_to_be_shown = "./data/graphics/worldmap/tile/tile-1.gif";
    //ui->TestItemBox->addItem(itm);

    //ui->TilesItemBox
    //ui->TilesItemBox->model()->setData(  ui->TilesItemBox->model()->index(1, 1),);

    QString first = "file-1.35.gif";
    QStringList mid1 = first.split(".",QString::SkipEmptyParts);
    QString second = mid1[0] + "m." + mid1[1];

    QMessageBox::information(this, tr("QRegExp test"),
       tr(QString("First: "+first+"\nSecond: "+second).toStdString().c_str()),
     QMessageBox::Ok);


}


void MainWindow::on_actionSave_activated()
{
    save();
}

void MainWindow::on_actionSave_as_activated()
{
    save_as();
}

void MainWindow::on_actionClose_activated()
{
    close_sw();
}

void MainWindow::on_actionSave_all_activated()
{
    save_all();
}

//////////////////////////////////// GoTo Section  ///////////////////////////////////////////////
void MainWindow::on_actionSection_1_activated()
{
    SetCurrentLevelSection(0);
}

void MainWindow::on_actionSection_2_activated()
{
    SetCurrentLevelSection(1);
}

void MainWindow::on_actionSection_3_activated()
{
    SetCurrentLevelSection(2);
}

void MainWindow::on_actionSection_4_activated()
{
    SetCurrentLevelSection(3);
}

void MainWindow::on_actionSection_5_activated()
{
    SetCurrentLevelSection(4);
}

void MainWindow::on_actionSection_6_activated()
{
    SetCurrentLevelSection(5);
}

void MainWindow::on_actionSection_7_activated()
{
    SetCurrentLevelSection(6);
}

void MainWindow::on_actionSection_8_activated()
{
    SetCurrentLevelSection(7);
}

void MainWindow::on_actionSection_9_activated()
{
    SetCurrentLevelSection(8);
}

void MainWindow::on_actionSection_10_activated()
{
    SetCurrentLevelSection(9);
}

void MainWindow::on_actionSection_11_activated()
{
    SetCurrentLevelSection(10);
}

void MainWindow::on_actionSection_12_activated()
{
    SetCurrentLevelSection(11);
}


void MainWindow::on_actionSection_13_activated()
{
    SetCurrentLevelSection(12);
}

void MainWindow::on_actionSection_14_activated()
{
    SetCurrentLevelSection(13);
}

void MainWindow::on_actionSection_15_activated()
{
    SetCurrentLevelSection(14);
}

void MainWindow::on_actionSection_16_activated()
{
    SetCurrentLevelSection(15);
}

void MainWindow::on_actionSection_17_activated()
{
    SetCurrentLevelSection(16);
}

void MainWindow::on_actionSection_18_activated()
{
    SetCurrentLevelSection(17);
}

void MainWindow::on_actionSection_19_activated()
{
    SetCurrentLevelSection(18);
}

void MainWindow::on_actionSection_20_activated()
{
    SetCurrentLevelSection(19);
}

void MainWindow::on_actionSection_21_activated()
{
    SetCurrentLevelSection(20);
}

void MainWindow::on_actionLoad_configs_activated()
{
    configs.loadconfigs();
}
