#include "testing_settings.h"
#include "ui_testing_settings.h"
#include "../global_settings.h"

TestingSettings::TestingSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestingSettings)
{
    ui->setupUi(this);
    ui->ex_god->setChecked(GlobalSettings::testing.xtra_god);
    ui->ex_flyup->setChecked(GlobalSettings::testing.xtra_flyup);
    ui->ex_chuck->setChecked(GlobalSettings::testing.xtra_chuck);
    ui->ex_debug->setChecked(GlobalSettings::testing.xtra_debug);
    ui->ex_physdebug->setChecked(GlobalSettings::testing.xtra_physdebug);
    ui->ex_freedom->setChecked(GlobalSettings::testing.xtra_worldfreedom);
    switch(GlobalSettings::testing.numOfPlayers)
    {
        case 1:default:
        ui->np_1p->setChecked(true);break;
        case 2:
        ui->np_2p->setChecked(true);break;
    }
}

TestingSettings::~TestingSettings()
{
    delete ui;
}

void TestingSettings::on_buttonBox_accepted()
{
    GlobalSettings::testing.xtra_god=ui->ex_god->isChecked();
    GlobalSettings::testing.xtra_flyup=ui->ex_flyup->isChecked();
    GlobalSettings::testing.xtra_chuck=ui->ex_chuck->isChecked();
    GlobalSettings::testing.xtra_debug=ui->ex_debug->isChecked();
    GlobalSettings::testing.xtra_physdebug=ui->ex_physdebug->isChecked();
    GlobalSettings::testing.xtra_worldfreedom=ui->ex_freedom->isChecked();
    if(ui->np_1p->isChecked())
        GlobalSettings::testing.numOfPlayers=1;
    else if(ui->np_1p->isChecked())
        GlobalSettings::testing.numOfPlayers=2;
    else
        GlobalSettings::testing.numOfPlayers=1;
    this->close();
}
