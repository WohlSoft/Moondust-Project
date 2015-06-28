#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_server(new PGENET_Server())
{
    ui->setupUi(this);
    connect(m_server.data(), SIGNAL(incomingText(QString)), this, SLOT(addText(QString)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bntStartServer_clicked()
{
    addText("Server has started!\n");
    m_server->start();
}

void MainWindow::addText(QString text)
{
    ui->plainTextEdit->appendPlainText(text);
}
