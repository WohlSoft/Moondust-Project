#include "testing_settings.h"
#include "ui_testing_settings.h"
#include "../global_settings.h"

#include <QMessageBox>

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
    ui->p1_character->setCurrentIndex(GlobalSettings::testing.p1_char-1);
    ui->p1_state->setCurrentIndex(GlobalSettings::testing.p1_state-1);
    ui->p2_character->setCurrentIndex(GlobalSettings::testing.p2_char-1);
    ui->p2_state->setCurrentIndex(GlobalSettings::testing.p2_state-1);
    switch(GlobalSettings::testing.numOfPlayers)
    {
        case 1:default:
        ui->np_1p->setChecked(true);break;
        case 2:
        ui->np_2p->setChecked(true);break;
    }
    // TEMPORARY NOTIFICATION
    connect(this, SIGNAL(windowShown()), SLOT(showNotifyTmp()), Qt::QueuedConnection);
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
    else if(ui->np_2p->isChecked())
        GlobalSettings::testing.numOfPlayers=2;
    else
        GlobalSettings::testing.numOfPlayers=1;
    GlobalSettings::testing.p1_char = (ui->p1_character->currentIndex()+1);
    GlobalSettings::testing.p1_state = (ui->p1_state->currentIndex()+1);
    GlobalSettings::testing.p2_char = (ui->p2_character->currentIndex()+1);
    GlobalSettings::testing.p2_state = (ui->p2_state->currentIndex()+1);
    this->close();
}

void TestingSettings::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    qApp->processEvents();
    emit windowShown();
}


void TestingSettings::showNotifyTmp()
{
    QMessageBox::information(this, tr("WIP"),
                             tr("Hello!\nThis dialog is not finished yet.\n"
                                "Those preferences will makes no effect on a testing "
                                "process before it will be fininshed. Sorry."));
}
