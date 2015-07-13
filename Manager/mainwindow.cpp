#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "config_packs_repos.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionRepositories_triggered()
{
    ConfPacksRepos box;
    box.exec();
}

void MainWindow::on_refresh_clicked()
{
    //disable "update" button

    //run QtConcurrent updateConfigPacksList()
}


void MainWindow::updateConfigPacksList()
{
    //Try to get index file from each repo

    //try to get each index file of each config pack (invalid or not exist config packs must be skiped)

    //find already downloaded config packs and compare update time

    //enable "update" button
}
