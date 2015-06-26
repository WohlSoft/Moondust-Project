#include "serverapplication.h"
#include "ui_server.h"

ServerApplication::ServerApplication(PgeTcpServer *aServer, QWidget *parent) :
    theServer(aServer), QWidget(parent),
    ui(new Ui::ServerApplicationUi)
{
    ui->setupUi(this);
    ui->infoBox->setText("Server not started");
    ui->portNumber->setValue(5000);

    connect(ui->startServer, SIGNAL(clicked(bool)), this, SLOT(startServer()));
    connect(ui->changePort, SIGNAL(clicked(bool)), this, SLOT(changePort()));
}

ServerApplication::~ServerApplication()
{
    delete ui;
    theServer->close();
}

bool ServerApplication::startServer()
{
    if (theServer->startServer())
    {
        ui->infoBox->setText("Server started");
        return true;
    }

    ui->infoBox->setText("Server failed to start!!");
    return false;
}

void ServerApplication::changePort()
{
    theServer->changePort(ui->portNumber->value());
    ui->infoBox->setText("Port changed to " + QString::number(ui->portNumber->value()));
}
