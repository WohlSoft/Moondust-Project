#include "mainserverwindow.h"
#include "ui_mainserverwindow.h"

#include <stdexcept>
#include <packet/packet.h>

#include <QTime>

/////////////////////// STATIC ///////////////////////////
MainServerWindow* MainServerWindow::INSTANCE = 0;

void MainServerWindow::msgHandler(QtMsgType type, const QMessageLogContext & context, const QString & msg)
{
    if(MainServerWindow::INSTANCE)
        MainServerWindow::INSTANCE->log(type, context, msg);
}

/////////////////////// STATIC END ///////////////////////////

MainServerWindow::MainServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainServerWindow)
{
    if(INSTANCE)
        throw new std::runtime_error("One instance is already running!");
    ui->setupUi(this);
    INSTANCE = this;
    qInstallMessageHandler(MainServerWindow::msgHandler);
    qDebug() << "Installed Message Handler";
}

MainServerWindow::~MainServerWindow()
{
    delete ui;
}

void MainServerWindow::start()
{
    m_server = new PGEServer();
}

void MainServerWindow::shutdown()
{}



void MainServerWindow::log(QtMsgType type, const QMessageLogContext & context, const QString& msg)
{
    QString outText = QTime::currentTime().toString("[HH:mm:ss] ");
    switch (type) {
         case QtDebugMsg:
             outText += "[DEBUG] ";
             break;
         case QtWarningMsg:
             outText += "[WARNING] ";
             break;
         case QtCriticalMsg:
             outText += "[CRITICAL] ";
             break;
         case QtFatalMsg:
             outText += "[FATAL] ";
         }
    outText += msg;
    ui->txtEditOutput->append(outText);
    QString additionalInfo = "";
    switch (type) {
        case QtWarningMsg:
        case QtCriticalMsg:
        case QtFatalMsg:
            additionalInfo += "File: " + QString(context.file) + "\n";
            additionalInfo += "Function: " + QString(context.function) + " (L" + QString::number(context.line) + ")\n";
            ui->txtEditOutput->append(additionalInfo);
        default:
        break;
    }
}

