#include "clientapplication.h"
#include "ui_clientapplication.h"

ClientApplication::ClientApplication(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientApplicationUi)
{
    QTcpSocket *socket = new QTcpSocket();
    connection = new ClientConnection(socket, this);

    ui->setupUi(this);

    connect(socket, SIGNAL(connected()), this, SLOT(enableButtons()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disableButtons()));

    connect(connection, SIGNAL(messageReceived(QByteArray)), this, SLOT(displayMessage(QByteArray)));

    connect(ui->connectButton, SIGNAL(clicked(bool)), this, SLOT(connectToServer()));
    connect(ui->disconnectButton, SIGNAL(clicked(bool)), this, SLOT(disconnectFromServer()));
    connect(ui->sendButton, SIGNAL(clicked(bool)), this, SLOT(dataToSend()));
    connect(ui->textToSend, SIGNAL(returnPressed()), this, SLOT(dataToSend()));
}

ClientApplication::~ClientApplication()
{
    delete ui;
}

void ClientApplication::enableButtons()
{
    qDebug() << "success connect";
    ui->disconnectButton->setEnabled(true);
    ui->sendButton->setEnabled(true);
    ui->textToSend->setEnabled(true);
    ui->connectButton->setEnabled(false);
}

void ClientApplication::disableButtons()
{
    qDebug() << "disconnected from server";
    ui->disconnectButton->setEnabled(false);
    ui->sendButton->setEnabled(false);
    ui->textToSend->setEnabled(false);
    ui->connectButton->setEnabled(true);
}

void ClientApplication::connectToServer()
{
    connection->connectToServer(ui->ipAddress->text(), ui->portNumber->value());
}

void ClientApplication::disconnectFromServer()
{
    connection->disconnectFromServer();
}

void ClientApplication::displayMessage(QByteArray bytes)
{
    ui->infoBox->setText(bytes.data());
}

void ClientApplication::dataToSend()
{
    connection->sendData(ui->textToSend->text().toUtf8());
}
