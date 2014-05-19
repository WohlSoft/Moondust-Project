#include "newlayerbox.h"
#include "ui_tonewlayerbox.h"
#include <QMessageBox>

ToNewLayerBox::ToNewLayerBox(LevelData * lData, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToNewLayerBox)
{
    LvlData = lData;
    ui->setupUi(this);
}

ToNewLayerBox::~ToNewLayerBox()
{
    delete ui;
}

void ToNewLayerBox::on_buttonBox_accepted()
{
    lName = ui->layerName->text();
    lLocked = ui->Lock->isChecked();
    lHidden = ui->hide->isChecked();

    bool AlreadyExist=false;
    foreach(LevelLayers layer, LvlData->layers)
    {
        if( layer.name==lName )
        {
            AlreadyExist=true;
            break;
        }
    }
    if(AlreadyExist)
    {
        valid=false;
        QMessageBox::warning(this,tr("Layer exists"), tr("Layer \"%1\" is exist, please, set other name.").arg(lName), QMessageBox::Ok);
    }
    else
    {
        valid=true;
        accept();
    }
}
