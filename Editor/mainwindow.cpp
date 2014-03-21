#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "levelprops.h"
#include <QFileDialog>
#include <QtGui>
#include <QFile>
#include <QTranslator>
#include <QLocale>

MainWindow::MainWindow(QMdiArea *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///////////////////////////////////////////////////////


//////////////////////////////////////////////////////

void MainWindow::on_OpenFile_activated()
{
     QString fileName_DATA = QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),".",
        tr("All SMBX files (*.LVL *.WLD *.TXT *.INI)\n"
        "SMBX Level (*.LVL)\n"
        "SMBX World (*.WLD)\n"
        "SMBX NPC Config (*.TXT *.INI)\n"
        "All Files (*.*)"));

        if(fileName_DATA==NULL) return;

        QFile file(fileName_DATA);

        if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("File open error"),
        tr("Can't open the file."), QMessageBox::Ok);
            return;
        }

        QFileInfo in_1(fileName_DATA);

        if(in_1.suffix() == "lvl")
        {
            if( ReadLevelFile(file) ) return;
        }
        else
        if(in_1.suffix() == "txt")
        {
            if( ReadNpcTXTFile(file) ) return;
        }
        else
        {
        QMessageBox::warning(this, tr("Bad file"),
         tr("This file have unknown extension"),
         QMessageBox::Ok);
        return;
        }

QMessageBox::information(this, tr("File checked"),
    tr("This file is good"),
    QMessageBox::Ok);

}



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

