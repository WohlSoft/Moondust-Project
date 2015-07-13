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
