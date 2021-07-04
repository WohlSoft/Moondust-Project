#include "echo_tune.h"
#include "ui_echo_tune.h"
#include <QSettings>
#include "../Player/mus_player.h"
#include "../Effects/spc_echo.h"

EchoTune::EchoTune(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EchoTune)
{
    ui->setupUi(this);

    QObject::connect(ui->echo_fir0, &QSlider::valueChanged, [this](int val)->void{
        ui->fir0_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir1, &QSlider::valueChanged, [this](int val)->void{
        ui->fir1_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir2, &QSlider::valueChanged, [this](int val)->void{
        ui->fir2_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir3, &QSlider::valueChanged, [this](int val)->void{
        ui->fir3_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir4, &QSlider::valueChanged, [this](int val)->void{
        ui->fir4_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir5, &QSlider::valueChanged, [this](int val)->void{
        ui->fir5_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir6, &QSlider::valueChanged, [this](int val)->void{
        ui->fir6_val->setText(QString("%1").arg(val));
    });
    QObject::connect(ui->echo_fir7, &QSlider::valueChanged, [this](int val)->void{
        ui->fir7_val->setText(QString("%1").arg(val));
    });
}

EchoTune::~EchoTune()
{
    delete ui;
}

void EchoTune::saveSetup()
{
    QSettings set;

    set.beginGroup("spc-echo");

    set.setValue("eon", ui->echo_eon->isChecked());
    set.setValue("edl", ui->echo_edl->value());
    set.setValue("efb", ui->echo_efb->value());

    set.setValue("mvoll", ui->echo_mvoll->value());
    set.setValue("mvolr", ui->echo_mvolr->value());
    set.setValue("evoll", ui->echo_evoll->value());
    set.setValue("evolr", ui->echo_evolr->value());

    set.setValue("fir0", ui->echo_fir0->value());
    set.setValue("fir1", ui->echo_fir1->value());
    set.setValue("fir2", ui->echo_fir2->value());
    set.setValue("fir3", ui->echo_fir3->value());
    set.setValue("fir4", ui->echo_fir4->value());
    set.setValue("fir5", ui->echo_fir5->value());
    set.setValue("fir6", ui->echo_fir6->value());
    set.setValue("fir7", ui->echo_fir7->value());
    set.endGroup();

    set.sync();
}

void EchoTune::loadSetup()
{
    QSettings set;

    set.beginGroup("spc-echo");

    ui->echo_eon->blockSignals(true);
    ui->echo_eon->setChecked(set.value("eon", PGE_MusicPlayer::echoGetReg(ECHO_EON) != 0).toBool());
    ui->echo_eon->blockSignals(false);

    ui->echo_edl->blockSignals(true);
    ui->echo_edl->setValue(set.value("edl", PGE_MusicPlayer::echoGetReg(ECHO_EDL)).toInt());
    ui->echo_edl->blockSignals(false);

    ui->echo_efb->blockSignals(true);
    ui->echo_efb->setValue(set.value("efb", PGE_MusicPlayer::echoGetReg(ECHO_EFB)).toInt());
    ui->echo_efb->blockSignals(false);

    ui->echo_mvoll->blockSignals(true);
    ui->echo_mvoll->setValue(set.value("mvoll", PGE_MusicPlayer::echoGetReg(ECHO_MVOLL)).toInt());
    ui->echo_mvoll->blockSignals(false);

    ui->echo_mvolr->blockSignals(true);
    ui->echo_mvolr->setValue(set.value("mvolr", PGE_MusicPlayer::echoGetReg(ECHO_MVOLR)).toInt());
    ui->echo_mvolr->blockSignals(false);

    ui->echo_evoll->blockSignals(true);
    ui->echo_evoll->setValue(set.value("evoll", PGE_MusicPlayer::echoGetReg(ECHO_EVOLL)).toInt());
    ui->echo_evoll->blockSignals(false);

    ui->echo_evolr->blockSignals(true);
    ui->echo_evolr->setValue(set.value("evolr", PGE_MusicPlayer::echoGetReg(ECHO_EVOLR)).toInt());
    ui->echo_evolr->blockSignals(false);

    ui->echo_fir0->setValue(set.value("fir0", PGE_MusicPlayer::echoGetReg(ECHO_FIR0)).toInt());
    ui->echo_fir1->setValue(set.value("fir1", PGE_MusicPlayer::echoGetReg(ECHO_FIR1)).toInt());
    ui->echo_fir2->setValue(set.value("fir2", PGE_MusicPlayer::echoGetReg(ECHO_FIR2)).toInt());
    ui->echo_fir3->setValue(set.value("fir3", PGE_MusicPlayer::echoGetReg(ECHO_FIR3)).toInt());
    ui->echo_fir4->setValue(set.value("fir4", PGE_MusicPlayer::echoGetReg(ECHO_FIR4)).toInt());
    ui->echo_fir5->setValue(set.value("fir5", PGE_MusicPlayer::echoGetReg(ECHO_FIR5)).toInt());
    ui->echo_fir6->setValue(set.value("fir6", PGE_MusicPlayer::echoGetReg(ECHO_FIR6)).toInt());
    ui->echo_fir7->setValue(set.value("fir7", PGE_MusicPlayer::echoGetReg(ECHO_FIR7)).toInt());

    set.endGroup();
}

void EchoTune::sendAll()
{
    PGE_MusicPlayer::echoSetReg(ECHO_EON, ui->echo_eon->isChecked() ? 1 : 0);
    PGE_MusicPlayer::echoSetReg(ECHO_EDL, ui->echo_edl->value());
    PGE_MusicPlayer::echoSetReg(ECHO_EFB, ui->echo_efb->value());
    PGE_MusicPlayer::echoSetReg(ECHO_MVOLL, ui->echo_mvoll->value());
    PGE_MusicPlayer::echoSetReg(ECHO_MVOLR, ui->echo_mvolr->value());
    PGE_MusicPlayer::echoSetReg(ECHO_EVOLL, ui->echo_evoll->value());
    PGE_MusicPlayer::echoSetReg(ECHO_EVOLR, ui->echo_evolr->value());

    PGE_MusicPlayer::echoSetReg(ECHO_FIR0, ui->echo_fir0->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR1, ui->echo_fir1->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR2, ui->echo_fir2->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR3, ui->echo_fir3->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR4, ui->echo_fir4->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR5, ui->echo_fir5->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR6, ui->echo_fir6->value());
    PGE_MusicPlayer::echoSetReg(ECHO_FIR7, ui->echo_fir7->value());
}

void EchoTune::on_save_clicked()
{
    saveSetup();
}

void EchoTune::on_sendAll_clicked()
{
    sendAll();
}

void EchoTune::on_echo_reload_clicked()
{
    ui->echo_eon->blockSignals(true);
    ui->echo_eon->setChecked(PGE_MusicPlayer::echoGetReg(ECHO_EON) != 0);
    ui->echo_eon->blockSignals(false);

    ui->echo_edl->blockSignals(true);
    ui->echo_edl->setValue(PGE_MusicPlayer::echoGetReg(ECHO_EDL));
    ui->echo_edl->blockSignals(false);

    ui->echo_efb->blockSignals(true);
    ui->echo_efb->setValue(PGE_MusicPlayer::echoGetReg(ECHO_EFB));
    ui->echo_efb->blockSignals(false);

    ui->echo_mvoll->blockSignals(true);
    ui->echo_mvoll->setValue(PGE_MusicPlayer::echoGetReg(ECHO_MVOLL));
    ui->echo_mvoll->blockSignals(false);

    ui->echo_mvolr->blockSignals(true);
    ui->echo_mvolr->setValue(PGE_MusicPlayer::echoGetReg(ECHO_MVOLR));
    ui->echo_mvolr->blockSignals(false);

    ui->echo_evoll->blockSignals(true);
    ui->echo_evoll->setValue(PGE_MusicPlayer::echoGetReg(ECHO_EVOLL));
    ui->echo_evoll->blockSignals(false);

    ui->echo_evolr->blockSignals(true);
    ui->echo_evolr->setValue(PGE_MusicPlayer::echoGetReg(ECHO_EVOLR));
    ui->echo_evolr->blockSignals(false);

    ui->echo_fir0->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR0));
    ui->echo_fir1->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR1));
    ui->echo_fir2->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR2));
    ui->echo_fir3->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR3));
    ui->echo_fir4->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR4));
    ui->echo_fir5->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR5));
    ui->echo_fir6->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR6));
    ui->echo_fir7->setValue(PGE_MusicPlayer::echoGetReg(ECHO_FIR7));
}

void EchoTune::on_reset_clicked()
{
    PGE_MusicPlayer::echoResetDefaults();
    on_echo_reload_clicked();
}

void EchoTune::on_echo_eon_clicked(bool checked)
{
    PGE_MusicPlayer::echoSetReg(ECHO_EON, checked ? 1 : 0);
}

void EchoTune::on_echo_edl_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_EDL, arg1);
}

void EchoTune::on_echo_efb_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_EFB, arg1);
}

void EchoTune::on_echo_mvoll_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_MVOLL, arg1);
}

void EchoTune::on_echo_mvolr_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_MVOLR, arg1);
}

void EchoTune::on_echo_evoll_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_EVOLL, arg1);
}

void EchoTune::on_echo_evolr_valueChanged(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_EVOLR, arg1);
}

void EchoTune::on_echo_fir0_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR0, arg1);
}

void EchoTune::on_echo_fir1_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR1, arg1);
}

void EchoTune::on_echo_fir2_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR2, arg1);
}

void EchoTune::on_echo_fir3_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR3, arg1);
}

void EchoTune::on_echo_fir4_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR4, arg1);
}

void EchoTune::on_echo_fir5_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR5, arg1);
}

void EchoTune::on_echo_fir6_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR6, arg1);
}

void EchoTune::on_echo_fir7_sliderMoved(int arg1)
{
    PGE_MusicPlayer::echoSetReg(ECHO_FIR7, arg1);
}

void EchoTune::on_resetFir_clicked()
{
    PGE_MusicPlayer::echoResetFir();
    on_echo_reload_clicked();
}
