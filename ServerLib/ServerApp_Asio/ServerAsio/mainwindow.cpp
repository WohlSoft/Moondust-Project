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

    ThreadedLogger::initStatic();
    connect(gThreadedLogger.data(), SIGNAL(newIncomingMsg(ThreadedLogger::LoggerLevel,QString)), this, SLOT(newLoggingText(ThreadedLogger::LoggerLevel,QString)), Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::newLoggingText(ThreadedLogger::LoggerLevel level, QString msg)
{
    addText(QString("[") + ThreadedLogger::LoggerLevelToString(level) + "] " + msg);
}

void MainWindow::on_bntStartServer_clicked()
{
    m_server->start();
    gThreadedLogger->logInfo("Server has started!");
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
            // Write Header
            QDataStream sockStream(&dataToWrite, QIODevice::ReadWrite);
            sockStream << (int)PacketID::PGENET_PacketUserAuth; //packetID
            sockStream << QString(""); //username
            sockStream << (int)0; //sessionID

            sockStream << QString("Kevsoft");
            sockStream << (int)PGENET_Global::NetworkVersion;

        }

        int lengthOfData = dataToWrite.size();
        sock.write((char*)&lengthOfData, sizeof(lengthOfData));
        sock.write(dataToWrite.data(), lengthOfData);


        sock.flush();


        QThread::currentThread()->sleep(2);

        sock.close();
    });
}




void MainWindow::closeEvent(QCloseEvent *e)
{
    e->accept();
    ThreadedLogger::destoryStatic();
}
