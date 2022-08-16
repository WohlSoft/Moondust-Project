#include "testing_settings.h"
#include "ui_testing_settings.h"
#include <mainwindow.h>
#include <main_window/global_settings.h>
#include <data_configs/data_configs.h>

#include <QMessageBox>

TestingSettings::TestingSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TestingSettings)
{
    ui->setupUi(this);

    MainWindow* mw = qobject_cast<MainWindow*>(this->parent());
    if(mw)
    {
        QList<obj_player>& plr = mw->configs.main_characters;
        ui->p1_character->clear();
        ui->p2_character->clear();

        for(obj_player& p : plr)
        {
            ui->p1_character->addItem(p.name);
            ui->p2_character->addItem(p.name);
        }

        reloadStates1(0);
        reloadStates2(0);

        QObject::connect(ui->p1_character, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TestingSettings::reloadStates1 );
        QObject::connect(ui->p2_character, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TestingSettings::reloadStates2 );
    }

    refreshVehicleID();

    QObject::connect(ui->p1_vehicleID, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this](int)->void
        {
            refreshVehicleTypes();
        }
    );

    QObject::connect(ui->p2_vehicleID, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        [this](int)->void
        {
            refreshVehicleTypes();
        }
    );

    ui->ex_god->setChecked(GlobalSettings::testing.xtra_god);
    ui->ex_flyup->setChecked(GlobalSettings::testing.xtra_flyup);
    ui->ex_chuck->setChecked(GlobalSettings::testing.xtra_chuck);
    ui->ex_debug->setChecked(GlobalSettings::testing.xtra_debug);
    ui->ex_showFPS->setChecked(GlobalSettings::testing.xtra_showFPS);
    ui->ex_physdebug->setChecked(GlobalSettings::testing.xtra_physdebug);
    ui->ex_freedom->setChecked(GlobalSettings::testing.xtra_worldfreedom);
    ui->ex_starsNum->setValue(GlobalSettings::testing.xtra_starsNum);
    ui->p1_character->setCurrentIndex(GlobalSettings::testing.p1_char-1);
    ui->p1_state->setCurrentIndex(GlobalSettings::testing.p1_state-1);
    ui->p1_vehicleID->setCurrentIndex(GlobalSettings::testing.p1_vehicleID);
    ui->p1_vehicleType->setCurrentIndex(GlobalSettings::testing.p1_vehicleType);
    ui->p2_character->setCurrentIndex(GlobalSettings::testing.p2_char-1);
    ui->p2_state->setCurrentIndex(GlobalSettings::testing.p2_state-1);
    ui->p2_vehicleID->setCurrentIndex(GlobalSettings::testing.p2_vehicleID);
    ui->p2_vehicleType->setCurrentIndex(GlobalSettings::testing.p2_vehicleType);

    switch(GlobalSettings::testing.numOfPlayers)
    {
        case 1:default:
        ui->np_1p->setChecked(true);break;
        case 2:
        ui->np_2p->setChecked(true);break;
    }


    // Hide features are not supported by testing engine if default one enforced
    bool noDefault = ConfStatus::hideNonDefaultEngines;
    auto defEngine = ConfStatus::defaultTestEngine;

    if(noDefault)
    {
        switch(defEngine)
        {
        default:
        case ConfStatus::ENGINE_MOONDUST:
            ui->initialState->setVisible(false); // Not implemented at Moondust Engine yet
            break;

        case ConfStatus::ENGINE_THEXTECH:
            ui->ex_freedom ->setVisible(false);
            ui->ex_chuck ->setVisible(false);
            ui->ex_flyup ->setVisible(false);
            ui->ex_debug ->setVisible(false);
            ui->ex_physdebug ->setVisible(false);
            break;

        case ConfStatus::ENGINE_LUNA:
            ui->initialState->setVisible(false);
            ui->ex_freedom ->setVisible(false);
            ui->ex_chuck ->setVisible(false);
            ui->ex_flyup ->setVisible(false);
            ui->ex_debug ->setVisible(false);
            ui->ex_physdebug ->setVisible(false);
            break;

        case ConfStatus::ENGINE_38A:
            ui->extraSettings->setVisible(false);
            ui->initialState->setVisible(false);
            ui->cheats->setVisible(false);
            ui->debug->setVisible(false);
            break;
        }
    }
}

TestingSettings::~TestingSettings()
{
    delete ui;
}

void TestingSettings::on_buttonBox_accepted()
{
    GlobalSettings::testing.xtra_god = ui->ex_god->isChecked();
    GlobalSettings::testing.xtra_flyup = ui->ex_flyup->isChecked();
    GlobalSettings::testing.xtra_chuck = ui->ex_chuck->isChecked();
    GlobalSettings::testing.xtra_debug = ui->ex_debug->isChecked();
    GlobalSettings::testing.xtra_showFPS = ui->ex_showFPS->isChecked();
    GlobalSettings::testing.xtra_physdebug = ui->ex_physdebug->isChecked();
    GlobalSettings::testing.xtra_worldfreedom = ui->ex_freedom->isChecked();
    GlobalSettings::testing.xtra_starsNum = ui->ex_starsNum->value();

    if(ui->np_1p->isChecked())
        GlobalSettings::testing.numOfPlayers = 1;
    else if(ui->np_2p->isChecked())
        GlobalSettings::testing.numOfPlayers = 2;
    else
        GlobalSettings::testing.numOfPlayers = 1;

    GlobalSettings::testing.p1_char = (ui->p1_character->currentIndex()+1);
    GlobalSettings::testing.p1_state = (ui->p1_state->currentIndex()+1);
    GlobalSettings::testing.p1_vehicleID = (ui->p1_vehicleID->currentIndex());
    GlobalSettings::testing.p1_vehicleType = (ui->p1_vehicleType->currentIndex());
    GlobalSettings::testing.p2_char = (ui->p2_character->currentIndex()+1);
    GlobalSettings::testing.p2_state = (ui->p2_state->currentIndex()+1);
    GlobalSettings::testing.p2_vehicleID = (ui->p2_vehicleID->currentIndex());
    GlobalSettings::testing.p2_vehicleType = (ui->p2_vehicleType->currentIndex());
    this->close();
}

void TestingSettings::reloadStates1(int index)
{
    MainWindow* mw = qobject_cast<MainWindow*>(this->parent());
    if(mw)
    {
        int character = index;
        if(character < 0) return;
        QList<obj_player>& plr = mw->configs.main_characters;
        if(character >= plr.size()) return;
        QList<obj_player_state>& states = plr[character].states;
        int oldIndex = ui->p1_state->currentIndex();
        ui->p1_state->clear();
        for(obj_player_state& p : states)
        {
            ui->p1_state->addItem(p.name);
        }
        ui->p1_state->setCurrentIndex(oldIndex);
    }
}

void TestingSettings::reloadStates2(int index)
{
    MainWindow* mw = qobject_cast<MainWindow*>(this->parent());
    if(mw)
    {
        int character = index;
        if(character < 0) return;
        QList<obj_player>& plr = mw->configs.main_characters;
        if(character >= plr.size()) return;
        QList<obj_player_state>& states = plr[character].states;
        int oldIndex = ui->p2_state->currentIndex();
        ui->p2_state->clear();
        for(obj_player_state& p : states)
        {
            ui->p2_state->addItem(p.name);
        }
        ui->p2_state->setCurrentIndex(oldIndex);
    }
}

void TestingSettings::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    qApp->processEvents();
    emit windowShown();
}

void TestingSettings::refreshVehicleID()
{
    MainWindow* mw = qobject_cast<MainWindow*>(this->parent());
    if(!mw)
        return;

    auto& veh = mw->configs.main_vehicles;

    ui->p1_vehicleID->clear();
    ui->p2_vehicleID->clear();

    ui->p1_vehicleID->addItem(tr("[No vehicle]"));
    ui->p2_vehicleID->addItem(tr("[No vehicle]"));

    for(auto &v : veh)
    {
        ui->p1_vehicleID->addItem(v.name);
        ui->p2_vehicleID->addItem(v.name);
    }

    refreshVehicleTypes();
}

void TestingSettings::refreshVehicleTypes()
{
    MainWindow* mw = qobject_cast<MainWindow*>(this->parent());
    if(!mw)
        return;

    auto& veh = mw->configs.main_vehicles;

    int p1_sel = ui->p1_vehicleID->currentIndex();
    int p2_sel = ui->p2_vehicleID->currentIndex();

    int p1_sel_backup = ui->p1_vehicleType->currentIndex();
    int p2_sel_backup = ui->p2_vehicleType->currentIndex();

    ui->p1_vehicleType->clear();
    ui->p2_vehicleType->clear();

    ui->p1_vehicleType->addItem(tr("[Unknown type]"));
    ui->p2_vehicleType->addItem(tr("[Unknown type]"));

    if(p1_sel > 0 && p1_sel <= veh.size())
    {
        for(auto &t : veh[p1_sel - 1].type_names)
            ui->p1_vehicleType->addItem(t);
    }

    if(p2_sel > 0 && p2_sel <= veh.size())
    {
        for(auto &t : veh[p2_sel - 1].type_names)
            ui->p2_vehicleType->addItem(t);
    }

    if(p1_sel_backup <= ui->p1_vehicleType->count())
        ui->p1_vehicleType->setCurrentIndex(p1_sel_backup);

    if(p2_sel_backup <= ui->p2_vehicleType->count())
        ui->p2_vehicleType->setCurrentIndex(p2_sel_backup);
}
