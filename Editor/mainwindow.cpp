#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
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

void MainWindow::on_OpenFile_activated()
{
     QFileDialog::getOpenFileName(this,
        trUtf8("Open file"),".",
tr("All SMBX files (*.LVL *.WLD *.TXT *.INI)\n\
SMBX Level (*.LVL)\n\
SMBX World (*.WLD)\n\
SMBX NPC Config (*.TXT *.INI)\n\
All Files (*.*)"));

/*    QFile file(fileName_DATA);
        if (!file.open(QIODevice::ReadOnly)) {

            return;
        }
        */
}

void MainWindow::on_Exit_activated()
{
    MainWindow::close();
    exit(0);
}

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
