#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTcpSocket>
#include <iostream>

#include <QJsonDocument>



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

        QJsonObject header
        {
            {"packetID", (int)PacketID::PGENET_PacketUserAuth},
            {"username", ""},
            {"sessionID", 0}
        };

        QJsonObject packet
        {
            {"username", "Kevsoft"},
            {"networkVersionNumber", (int)PGENET_Global::NetworkVersion}
        };

        QJsonObject fullPacket
        {
            {"header", header},
            {"packet", packet}
        };

        QJsonDocument doc(fullPacket);
        QByteArray dataToWrite;
        dataToWrite = doc.toJson(QJsonDocument::Compact);

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
