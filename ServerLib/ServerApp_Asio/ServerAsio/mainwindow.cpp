#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTcpSocket>
#include <iostream>


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

        QByteArray dataToWrite;
        {
            QDataStream sockStream(&dataToWrite, QIODevice::ReadWrite);
            sockStream << (int)0; //packetID
            sockStream << QString("HI"); //username
            sockStream << (int)0; //sessionID
        }

        int lengthOfData = dataToWrite.size();
        sock.write((char*)&lengthOfData, sizeof(lengthOfData));
        sock.write(dataToWrite.data(), lengthOfData);

        sock.flush();


        QThread::currentThread()->sleep(2);

        sock.close();
    });
}


