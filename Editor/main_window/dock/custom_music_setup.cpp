/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SDL2/SDL_mixer_ext.h>
#include <QFileInfo>

#include "custom_music_setup.h"
#include "ui_custom_music_setup.h"



CustomMusicSetup::MusicType CustomMusicSetup::detectType()
{
    const QStringList midiSuffix = {
        "mid", "midi", "kar", "rmi", "mus", "xmi"
    };

    const QStringList adlSuffix = {
        "imf", "cmf", "gmf"
    };

    const QStringList gmeSuffix = {
        "nsf", "nsfe", "vgm", "vgz", "gbs", "gym", "kss", "hes", "spc", "ay", "sap"
    };

    QString fName;
    if(m_music.contains('|'))
        fName = m_music.split('|').front();
    else
        fName = m_music;

    QFileInfo f(fName);
    QString suffix = f.suffix().toLower();

    if(midiSuffix.contains(suffix))
        return MIDI;
    else if(adlSuffix.contains(suffix))
        return ADLMIDI;
    else if(gmeSuffix.contains(suffix))
        return GME;
    else
        return Unsupported;
}

void CustomMusicSetup::initSetup()
{
    blockSignals(true);

    ui->midiSynth->setCurrentIndex(0);

    on_midiGainReset_clicked();
    on_midiTempoReset_clicked();
    on_gmeGainReset_clicked();
    on_gmeTempoReset_clicked();

    ui->gmeTrackNumber->setValue(0);

    ui->midiExAdlBank->setCurrentIndex(Mix_ADLMIDI_getBankID());
    ui->midiExAdlVolumeModel->setCurrentIndex(Mix_ADLMIDI_getVolumeModel());
    int chips = Mix_ADLMIDI_getChipsCount();
    if(chips > 0)
        ui->midiExAdlChips->setValue(chips);
    ui->midiExAdlChipsEn->setChecked(chips > 0);
    ui->midiExAdlChips->setEnabled(ui->midiExAdlChipsEn->isChecked());

    ui->midiExAdlDeepTremolo->setCheckState(Qt::PartiallyChecked);
    ui->midiExAdlDeepVibrato->setCheckState(Qt::PartiallyChecked);

    ui->midiExOpnVolumeModel->setCurrentIndex(Mix_OPNMIDI_getVolumeModel());
    chips = Mix_OPNMIDI_getChipsCount();
    if(chips > 0)
        ui->midiExOpnChips->setValue(chips);
    ui->midiExOpnChipsEn->setChecked(chips > 0);
    ui->midiExOpnChips->setEnabled(ui->midiExOpnChipsEn->isChecked());

    blockSignals(false);
}

void CustomMusicSetup::parseSettings()
{
    m_type = detectType();
    m_settingsNeeded = m_type != Unsupported;

    if(!m_music.contains('|'))
        return; // No arguments
    QString name;
    QString args;

}

void CustomMusicSetup::buildSettings()
{

}

void CustomMusicSetup::updateVisibiltiy()
{
    int synType = ui->midiSynth->currentData().toInt();

    ui->setupZone->removeTab(0);
    ui->setupZone->removeTab(0);

    if(m_type == MIDI || m_type == ADLMIDI)
        ui->setupZone->insertTab(0, ui->tabMidi, tr("MIDI"));
    if(m_type == GME)
        ui->setupZone->insertTab(0, ui->tabChiptune, tr("Chiptune"));

    ui->midiSetupADL->setVisible((m_type == MIDI && synType == MIDI_ADLMIDI) ||
                                 m_type == ADLMIDI);
    ui->midiExAdlBank->setEnabled(m_type == MIDI);
    ui->midiSetupOPN->setVisible(m_type == MIDI && synType == MIDI_OPNMIDI);

    bool hasTempo = synType == MIDI_ADLMIDI || synType == MIDI_OPNMIDI;

    ui->midiTempoLabel->setVisible(hasTempo);
    ui->midiTempo->setVisible(hasTempo);
    ui->midiTempoAbs->setVisible(hasTempo);
    ui->midiTempoReset->setVisible(hasTempo);

    bool hasGain = hasTempo;
    ui->midiGainLabel->setVisible(hasGain);
    ui->midiGain->setVisible(hasGain);
    ui->midiGainAbs->setVisible(hasGain);
    ui->midiGainReset->setVisible(hasGain);

    update();
}

CustomMusicSetup::CustomMusicSetup(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::CustomMusicSetup)
{
    ui->setupUi(this);
    retranslateLists();

    QObject::connect(ui->midiGainAbs, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [this](double v)->void
    {
        ui->midiGain->blockSignals(true);
        ui->midiGain->setValue(v * 100);
        ui->midiGain->blockSignals(false);
    });

    QObject::connect(ui->midiGain, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged),
                     [this](int v)->void
    {
        ui->midiGainAbs->blockSignals(true);
        ui->midiGainAbs->setValue(double(v) / 100.0);
        ui->midiGainAbs->blockSignals(false);
        on_midiGainAbs_valueChanged(ui->midiGainAbs->value());
    });


    QObject::connect(ui->midiTempoAbs, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [this](double v)->void
    {
        ui->midiTempo->blockSignals(true);
        ui->midiTempo->setValue(v * 100);
        ui->midiTempo->blockSignals(false);
    });

    QObject::connect(ui->midiTempo, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged),
                     [this](int v)->void
    {
        ui->midiTempoAbs->blockSignals(true);
        ui->midiTempoAbs->setValue(double(v) / 100.0);
        ui->midiTempoAbs->blockSignals(false);
        on_midiTempoAbs_valueChanged(ui->midiGainAbs->value());
    });


    QObject::connect(ui->gmeGainAbs, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [this](double v)->void
    {
        ui->gmeGain->blockSignals(true);
        ui->gmeGain->setValue(v * 100);
        ui->gmeGain->blockSignals(false);
    });

    QObject::connect(ui->gmeGain, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged),
                     [this](int v)->void
    {
        ui->gmeGainAbs->blockSignals(true);
        ui->gmeGainAbs->setValue(double(v) / 100.0);
        ui->gmeGainAbs->blockSignals(false);
        on_gmeGainAbs_valueChanged(ui->midiGainAbs->value());
    });


    QObject::connect(ui->gmeTempoAbs, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     [this](double v)->void
    {
        ui->gmeTempo->blockSignals(true);
        ui->gmeTempo->setValue(v * 100);
        ui->gmeTempo->blockSignals(false);
    });

    QObject::connect(ui->gmeTempo, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged),
                     [this](int v)->void
    {
        ui->gmeTempoAbs->blockSignals(true);
        ui->gmeTempoAbs->setValue(double(v) / 100.0);
        ui->gmeTempoAbs->blockSignals(false);
        on_gmeTempoAbs_valueChanged(ui->midiGainAbs->value());
    });
}

CustomMusicSetup::~CustomMusicSetup()
{
    delete ui;
}

void CustomMusicSetup::initLists()
{
    blockSignals(true);

    int totalBanks = Mix_ADLMIDI_getTotalBanks();
    const char *const* banks = Mix_ADLMIDI_getBankNames();
    ui->midiExAdlBank->clear();

    for(int i = 0; i < totalBanks; ++i)
    {
        ui->midiExAdlBank->addItem(banks[i], i);
    }

    blockSignals(false);

    retranslateLists();
}

void CustomMusicSetup::setMusicPath(const QString &music)
{
    m_music = music;
    initSetup();
    parseSettings();
    updateVisibiltiy();
}

bool CustomMusicSetup::settingsNeeded()
{
    return m_settingsNeeded;
}

QString CustomMusicSetup::musicPath()
{
    return m_music;
}

void CustomMusicSetup::changeEvent(QEvent *e)
{
    QDockWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        retranslateLists();
        break;
    default:
        break;
    }
}

void CustomMusicSetup::retranslateLists()
{
    blockSignals(true);

    int cache;

    cache = ui->midiSynth->currentIndex();
    ui->midiSynth->clear();
    ui->midiSynth->addItem(tr("libADLMIDI (OPL3 Synth emulation)"), MIDI_ADLMIDI);
    ui->midiSynth->addItem(tr("libOPNMIDI (YM2612 Synth emulation)"), MIDI_OPNMIDI);
    ui->midiSynth->addItem(tr("Timidity (needed a bank)"), MIDI_Timidity);
    ui->midiSynth->addItem(tr("Native MIDI (Not recommended, buggy)"), MIDI_Native);
    ui->midiSynth->setCurrentIndex(cache);

    cache = ui->midiExAdlVolumeModel->currentIndex();
    ui->midiExAdlVolumeModel->clear();
    ui->midiExAdlVolumeModel->addItem(tr("[Auto]"), ADLMIDI_VM_AUTO);
    ui->midiExAdlVolumeModel->addItem(tr("Generic", "Volume model for libADLMIDI"), ADLMIDI_VM_GENERIC);
    ui->midiExAdlVolumeModel->addItem(tr("Native OPL3", "Volume model for libADLMIDI"), ADLMIDI_VM_CMF);
    ui->midiExAdlVolumeModel->addItem("DMX", ADLMIDI_VM_DMX);
    ui->midiExAdlVolumeModel->addItem("Apogee", ADLMIDI_VM_APOGEE);
    ui->midiExAdlVolumeModel->addItem("Win9x", ADLMIDI_VM_9X);
    ui->midiExAdlVolumeModel->addItem("DMX (Fixed)", ADLMIDI_VM_DMX_FIXED);
    ui->midiExAdlVolumeModel->addItem("Apogee (Fixed)", ADLMIDI_VM_APOGEE_FIXED);
    ui->midiExAdlVolumeModel->addItem("Audio Library Interface", ADLMIDI_VM_AIL);
    ui->midiExAdlVolumeModel->addItem("Win9x Generic FM", ADLMIDI_VM_9X_GENERIC_FM);
    ui->midiExAdlVolumeModel->addItem("HMI Sound Operating System", ADLMIDI_VM_HMI);
    ui->midiExAdlVolumeModel->addItem("HMI Sound Operating System (Old)", ADLMIDI_VM_HMI_OLD);
    ui->midiExAdlVolumeModel->setCurrentIndex(cache);

    cache = ui->midiExOpnVolumeModel->currentIndex();
    ui->midiExOpnVolumeModel->clear();
    ui->midiExOpnVolumeModel->addItem(tr("[Auto]"), 0);
    ui->midiExOpnVolumeModel->addItem(tr("Generic", "Volume model for libADLMIDI"), 1);
    ui->midiExOpnVolumeModel->addItem(tr("Native OPL3", "Volume model for libADLMIDI"), 2);
    ui->midiExOpnVolumeModel->addItem("DMX", 3);
    ui->midiExOpnVolumeModel->addItem("Apogee", 4);
    ui->midiExOpnVolumeModel->addItem("Win9x", 5);
    ui->midiExOpnVolumeModel->setCurrentIndex(cache);

    blockSignals(false);
}

void CustomMusicSetup::on_midiSynth_currentIndexChanged(int index)
{
    updateVisibiltiy();
}

void CustomMusicSetup::on_midiGain_valueChanged(int value)
{

}

void CustomMusicSetup::on_midiGainAbs_valueChanged(double arg1)
{

}

void CustomMusicSetup::on_midiGainReset_clicked()
{
    ui->midiGain->setValue(200);
    ui->midiGainAbs->setValue(2.0);
}

void CustomMusicSetup::on_midiTempo_valueChanged(int value)
{

}

void CustomMusicSetup::on_midiTempoAbs_valueChanged(double arg1)
{

}

void CustomMusicSetup::on_midiTempoReset_clicked()
{
    ui->midiTempo->setValue(100);
    ui->midiTempoAbs->setValue(1.0);
}

void CustomMusicSetup::on_midiExAdlBank_currentIndexChanged(int index)
{

}

void CustomMusicSetup::on_midiExAdlVolumeModel_currentIndexChanged(int index)
{

}

void CustomMusicSetup::on_midiExAdlChips_valueChanged(int arg1)
{

}

void CustomMusicSetup::on_midiExAdlDeepTremolo_clicked(bool checked)
{

}

void CustomMusicSetup::on_midiExAdlDeepVibrato_clicked(bool checked)
{

}

void CustomMusicSetup::on_midiExOpnVolumeModel_currentIndexChanged(int index)
{

}

void CustomMusicSetup::on_midiExOpnChips_valueChanged(int arg1)
{

}

void CustomMusicSetup::on_gmePrevTrack_clicked()
{
    int val = ui->gmeTrackNumber->value();
    if(val <= 0)
        ui->gmeTrackNumber->setValue(val - 1);
    else
        ui->gmeTrackNumber->setValue(0);
}

void CustomMusicSetup::on_gmeNextTrack_clicked()
{
    int val = ui->gmeTrackNumber->value();
    ui->gmeTrackNumber->setValue(val + 1);
}

void CustomMusicSetup::on_gmeTrackNumber_valueChanged(int arg1)
{

}

void CustomMusicSetup::on_gmeGoToFirst_clicked()
{
    ui->gmeTrackNumber->setValue(0);
}

void CustomMusicSetup::on_gmeGain_valueChanged(int value)
{

}

void CustomMusicSetup::on_gmeGainAbs_valueChanged(double arg1)
{

}

void CustomMusicSetup::on_gmeGainReset_clicked()
{
    ui->gmeGain->setValue(200);
    ui->gmeGainAbs->setValue(2.0);
}

void CustomMusicSetup::on_gmeTempo_valueChanged(int value)
{

}

void CustomMusicSetup::on_gmeTempoAbs_valueChanged(double arg1)
{

}

void CustomMusicSetup::on_gmeTempoReset_clicked()
{
    ui->gmeTempo->setValue(100);
    ui->gmeTempoAbs->setValue(1.0);
}
