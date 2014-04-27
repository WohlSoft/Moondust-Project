#include "loadingprocess.h"
#include "ui_loadingprocess.h"

LoadingProcess::LoadingProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoadingProcess)
{
    setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
}

LoadingProcess::~LoadingProcess()
{
    delete ui;
}
