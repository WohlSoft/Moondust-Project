#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTcpSocket>

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

void MainWindow::on_bntSendDbgText_clicked()
{
    QtConcurrent::run([this](){
        QTcpSocket sock;
        sock.connectToHost("127.0.0.1", 24444);
        sock.waitForConnected();

        sock.write("1234_You see.... How should I get all the text?");
        sock.flush();

        QThread::currentThread()->sleep(2);

        sock.write("Closing!");
        sock.flush();

        sock.close();
    });
}


