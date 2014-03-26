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
#include "childwindow.h"
#include "leveledit.h"
#include "npcedit.h"

QString LastOpenDir = ".";

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    /*
    connect(mdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)),
            this, SLOT(updateMenus()));

    windowMapper = new QSignalMapper(this);

    connect(windowMapper, SIGNAL(mapped(QWidget*)),
            this, SLOT(setActiveSubWindow(QWidget*)));
            */

    QString inifile = QApplication::applicationDirPath() + "/" + "plweditor.ini";
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    resize(settings.value("size", size()).toSize());

    move(settings.value("pos", pos()).toPoint());
    LastOpenDir = settings.value("lastpath", ".").toString();
    if(settings.value("maximased", "false")=="true") showMaximized();
    settings.endGroup();

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

}

MainWindow::~MainWindow()
{
    delete ui;
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

        leveledit *child = createChild();
        if (child->loadFile(FilePath, FileData)) {
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


npcedit *MainWindow::createNPCChild()
{
    npcedit *child = new npcedit;
    ui->centralWidget->addSubWindow(child);

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}

leveledit *MainWindow::createChild()
{
    leveledit *child = new leveledit;
    ui->centralWidget->addSubWindow(child)->resize(QSize(800, 602));;

 /*   connect(child, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(child, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
 */

    return child;
}

QMdiSubWindow *MainWindow::findMdiChild(const QString &fileName)
{
    QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    foreach (QMdiSubWindow *window, ui->centralWidget->subWindowList()) {
        leveledit *mdiChild = qobject_cast<leveledit *>(window->widget());
        if (mdiChild->currentFile() == canonicalFilePath)
            return window;
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
    about.setWindowFlags (about.windowFlags() & ~Qt::WindowContextHelpButtonHint);
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
}

