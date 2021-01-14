/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <cstdlib>

#include "custom_music_setup.h"
#include "ui_custom_music_setup.h"



CustomMusicSetup::MusicType CustomMusicSetup::detectType(const QString &music)
{
    const QStringList midiSuffix =
    {
        "mid", "midi", "kar", "rmi", "mus", "xmi"
    };

    const QStringList adlSuffix =
    {
        "imf", "cmf", "gmf"
    };

    const QStringList gmeSuffix =
    {
        "nsf", "nsfe", "vgm", "vgz", "gbs", "gym", "kss", "hes", "spc", "ay", "sap"
    };

    QString fName;
    if(music.contains('|'))
        fName = music.split('|').front();
    else
        fName = music;

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

    ui->midiExAdlBank->setCurrentIndex(adlDefaultBank);
    ui->midiExAdlVolumeModel->setCurrentIndex(adlDefaultVolumeModel);
    if(adlDefaultChips > 0)
        ui->midiExAdlChips->setValue(adlDefaultChips);
    ui->midiExAdlChipsEn->setChecked(adlDefaultChips > 0);
    ui->midiExAdlChips->setEnabled(ui->midiExAdlChipsEn->isChecked());

    ui->midiExAdlDeepTremolo->setCheckState(Qt::PartiallyChecked);
    ui->midiExAdlDeepVibrato->setCheckState(Qt::PartiallyChecked);

    ui->midiExOpnVolumeModel->setCurrentIndex(opnDefaultVolumeModel);

    if(opnDefaultChips > 0)
        ui->midiExOpnChips->setValue(opnDefaultChips);
    ui->midiExOpnChipsEn->setChecked(opnDefaultChips > 0);
    ui->midiExOpnChips->setEnabled(ui->midiExOpnChipsEn->isChecked());

    blockSignals(false);
}

static Qt::CheckState intToCheckstate(int v)
{
    switch(v)
    {
    default:
    case -1:
        return Qt::PartiallyChecked;
    case 0:
        return Qt::Unchecked;
    case 1:
        return Qt::Checked;
    }
}

static void setCurrentData(QComboBox*b, int d)
{
    int c = b->count();
    for(int i = 0; i < c; ++i)
    {
        if(b->itemData(i).toInt() == d)
        {
            b->setCurrentIndex(i);
            break;
        }
    }
}

void CustomMusicSetup::parseSettings()
{
    m_type = detectType(m_music);
    m_settingsNeeded = m_type != Unsupported;

    if(!m_music.contains('|'))
    {
        m_musicName = m_music;
        m_musicArgs.clear();
        return; // No arguments
    }
    auto s = m_music.split('|');
    m_musicName = s[0];
    m_musicArgs = s[1];

#define ARG_BUFFER_SIZE    1024
    char arg[ARG_BUFFER_SIZE];
    char type = '-';
    size_t maxlen = 0;
    bool keepLoop = true;
    size_t i, j = 0;
    /* first value is an integer without prefox sign. So, begin the scan with opened value state */
    int value_opened = 1;
    std::string argsStr = m_musicArgs.toStdString();
    const char *args = argsStr.c_str();

    if(m_musicArgs.isEmpty())
        return;

    blockSignals(true);

    maxlen = argsStr.size() + 1;

    switch(m_type)
    {
    case GME:
        value_opened = 1;

        for(i = 0; i < maxlen && keepLoop; i++)
        {
            char c = args[i];
            if(value_opened == 1)
            {
                if((c == ';') || (c == '\0'))
                {
                    int value;
                    arg[j] = '\0';
                    value = SDL_atoi(arg);
                    switch(type)
                    {
                    case '-':
                        ui->gmeTrackNumber->setValue(value);
                        break;
                    case 't':
                        if(arg[0] == '=')
                        {
                            double tempo = QString(arg + 1).toDouble();
                            if(tempo <= 0.0)
                                tempo = 1.0;
                            ui->gmeTempoAbs->setValue(tempo);
                            ui->gmeTempo->setValue(tempo * 100);
                        }
                        break;
                    case 'g':
                        if(arg[0] == '=')
                        {
                            double gain = QString(arg + 1).toDouble();
                            if(gain <= 0.0)
                                gain = 1.0;
                            ui->gmeGainAbs->setValue(gain);
                            ui->gmeGain->setValue(gain * 100);
                        }
                        break;
                    case '\0':
                        break;
                    default:
                        break;
                    }
                    value_opened = 0;
                }
                arg[j++] = c;
            }
            else
            {
                if(c == '\0')
                {
                    keepLoop = false;
                    break;
                }
                type = c;
                value_opened = 1;
                j = 0;
            }
        }
        break;

    case MIDI:
    case ADLMIDI:

        value_opened = 0;
        for(i = 0; i < maxlen && keepLoop; i++)
        {
            char c = args[i];
            if(value_opened == 1)
            {
                if((c == ';') || (c == '\0'))
                {
                    int value = 0;
                    arg[j] = '\0';
                    if(type != 'x')
                        value = SDL_atoi(arg);
                    switch(type)
                    {
                    case 's':
                        setCurrentData(ui->midiSynth, value);
                        break;
                    case 'c':
                        if(ui->midiSynth->currentData().toInt() == MIDI_ADLMIDI)
                        {
                            ui->midiExAdlChips->setValue(value);
                            ui->midiExAdlChipsEn->setChecked(true);
                        }
                        else if(ui->midiSynth->currentData().toInt() == MIDI_OPNMIDI)
                        {
                            ui->midiExOpnChips->setValue(value);
                            ui->midiExOpnChipsEn->setChecked(true);
                        }
                        break;
                    case 'f':
                        // setup->four_op_channels = value;
                        break;
                    case 'b':
                        setCurrentData(ui->midiExAdlBank, value);
                        break;
                    case 't':
                        if(arg[0] == '=')
                        {
                            double tempo = QString(arg + 1).toDouble();
                            if(tempo <= 0.0)
                                tempo = 1.0;
                            ui->midiTempoAbs->setValue(tempo);
                            ui->midiTempo->setValue(tempo * 100);
                        }
                        else
                            ui->midiExAdlDeepTremolo->setCheckState(intToCheckstate(value));
                        break;
                    case 'g':
                        if(arg[0] == '=')
                        {
                            double gain = QString(arg + 1).toDouble();
                            if(gain <= 0.0)
                                gain = 1.0;
                            ui->midiGainAbs->setValue(gain);
                            ui->midiGain->setValue(gain * 100);
                        }
                        break;
                    case 'v':
                        ui->midiExAdlDeepVibrato->setCheckState(intToCheckstate(value));
                        break;
                    case 'a':
                        /* Deprecated and useless */
                        break;
                    case 'l':
                        if(ui->midiSynth->currentData().toInt() == MIDI_ADLMIDI)
                            setCurrentData(ui->midiExAdlVolumeModel, value);
                        else if(ui->midiSynth->currentData().toInt() == MIDI_OPNMIDI)
                            setCurrentData(ui->midiExOpnVolumeModel, value);
                        break;
                    case 'r':
                        // setup->full_brightness_range = value;
                        break;
                    case 'p':
                        // setup->soft_pan = value;
                        break;
                    case 'e':
                        // setup->emulator = value;
                        break;
                    case 'x':
//                        if(arg[0] == '=')
//                            SDL_strlcpy(setup->custom_bank_path, arg + 1, (ARG_BUFFER_SIZE - 1));
                        break;
                    case '\0':
                        break;
                    default:
                        break;
                    }
                    value_opened = 0;
                }
                arg[j++] = c;
            }
            else
            {
                if(c == '\0')
                {
                    keepLoop = false;
                    break;
                }
                type = c;
                value_opened = 1;
                j = 0;
            }
        }

        break;

    default:
        break;
    }
#undef ARG_BUFFER_SIZE

    blockSignals(false);
}

static int checkboxToInt(QCheckBox *c)
{
    switch(c->checkState())
    {
    case Qt::PartiallyChecked:
    default:
        return -1;
    case Qt::Checked:
        return 1;
    case Qt::Unchecked:
        return 0;
    }
}

void CustomMusicSetup::buildSettings()
{
    m_musicArgs.clear();
    if(m_type == MIDI || m_type == ADLMIDI)
    {
        int v;

        v = ui->midiSynth->currentData().toInt();
        m_musicArgs += QString("s%1;").arg(v);

        switch(v)
        {
        case MIDI_ADLMIDI:
            v = ui->midiExAdlBank->currentData().toInt();
            if(v != adlDefaultBank)
                m_musicArgs += QString("b%1;").arg(ui->midiExAdlBank->currentData().toInt());

            v = ui->midiExAdlChips->value();
            if(ui->midiExAdlChipsEn->isChecked())
                m_musicArgs += QString("c%1;").arg(v);

            v = ui->midiExAdlVolumeModel->currentData().toInt();
            if(v != adlDefaultVolumeModel)
                m_musicArgs += QString("l%1;").arg(v);

            v = checkboxToInt(ui->midiExAdlDeepTremolo);
            if(v >= 0)
                m_musicArgs += QString("t%1;").arg(v);

            v = checkboxToInt(ui->midiExAdlDeepVibrato);
            if(v >= 0)
                m_musicArgs += QString("v%1;").arg(v);
            break;

        case MIDI_OPNMIDI:
            v = ui->midiExOpnVolumeModel->currentData().toInt();
            if(v != opnDefaultVolumeModel)
                m_musicArgs += QString("l%1;").arg(v);

            v = ui->midiExOpnChips->value();
            if(ui->midiExOpnChipsEn->isChecked())
                m_musicArgs += QString("c%1;").arg(v);

            break;
        }

        if(int(ui->midiTempoAbs->value() * 100) != 100)
            m_musicArgs += QString("t=%1;").arg(ui->midiTempoAbs->value());

        if(int(ui->midiGainAbs->value() * 100) != 200)
            m_musicArgs += QString("g=%1;").arg(ui->midiGainAbs->value());

        m_music = m_musicName + (m_musicArgs.isEmpty() ? QString() : "|" + m_musicArgs);
    }
    else if(m_type == GME)
    {
        if(int(ui->gmeTempoAbs->value() * 100) != 100)
            m_musicArgs += ";t=" + QString::number(ui->gmeTempoAbs->value());

        if(int(ui->gmeGainAbs->value() * 100) != 100)
            m_musicArgs += ";g=" + QString::number(ui->gmeGainAbs->value());

        if(ui->gmeTrackNumber->value() != 0 || !m_musicArgs.isEmpty())
            m_musicArgs.insert(0, QString::number(ui->gmeTrackNumber->value()));

        m_music = m_musicName + (m_musicArgs.isEmpty() ? QString() : "|" + m_musicArgs);
    }
    else
        m_music = m_musicName;

    emit musicSetupChanged(m_music);
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

void CustomMusicSetup::setMusicPlayState(bool checked)
{
    ui->playMusicProxy->setChecked(checked);
}

CustomMusicSetup::CustomMusicSetup(QWidget *parent) :
    QDialog(parent),
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
        on_midiTempoAbs_valueChanged(ui->midiTempoAbs->value());
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
        on_gmeTempoAbs_valueChanged(ui->gmeTempoAbs->value());
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
    const char *const *banks = Mix_ADLMIDI_getBankNames();
    ui->midiExAdlBank->clear();

    for(int i = 0; i < totalBanks; ++i)
        ui->midiExAdlBank->addItem(QString("%1: %2").arg(i).arg(banks[i]), i);

    blockSignals(false);

    adlDefaultBank = Mix_ADLMIDI_getBankID();
    adlDefaultChips = Mix_ADLMIDI_getChipsCount();
    adlDefaultVolumeModel = Mix_ADLMIDI_getVolumeModel();

    opnDefaultChips = Mix_OPNMIDI_getChipsCount();
    opnDefaultVolumeModel = Mix_OPNMIDI_getVolumeModel();

    retranslateLists();
}

void CustomMusicSetup::setMusicPath(const QString &music)
{
    initSetup();
    m_music = music;
    parseSettings();
    updateVisibiltiy();
}

bool CustomMusicSetup::settingsNeeded()
{
    return m_settingsNeeded;
}

bool CustomMusicSetup::settingsNeeded(const QString &music)
{
    return detectType(music) != Unsupported;
}

QString CustomMusicSetup::musicPath()
{
    return m_music;
}

void CustomMusicSetup::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch(e->type())
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

void CustomMusicSetup::on_midiSynth_currentIndexChanged(int)
{
    updateVisibiltiy();
    if(!signalsBlocked())
        buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiGain_valueChanged(int)
{}

void CustomMusicSetup::on_midiGainAbs_valueChanged(double)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiGainReset_clicked()
{
    ui->midiGain->setValue(200);
    ui->midiGainAbs->setValue(2.0);
    if(!signalsBlocked())
        buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiTempo_valueChanged(int)
{}

void CustomMusicSetup::on_midiTempoAbs_valueChanged(double arg1)
{
    buildSettings();
    emit updateSongTempo(arg1);
}

void CustomMusicSetup::on_midiTempoReset_clicked()
{
    ui->midiTempo->setValue(100);
    ui->midiTempoAbs->setValue(1.0);
    if(!signalsBlocked())
        buildSettings();
    emit updateSongTempo(ui->midiTempoAbs->value());
}

void CustomMusicSetup::on_midiExAdlBank_currentIndexChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExAdlVolumeModel_currentIndexChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExAdlChipsEn_clicked()
{
    if(!signalsBlocked())
        buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExAdlChips_valueChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExAdlDeepTremolo_clicked(bool)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExAdlDeepVibrato_clicked(bool)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExOpnVolumeModel_currentIndexChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExOpnChipsEn_clicked()
{
    if(!signalsBlocked())
        buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_midiExOpnChips_valueChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_gmePrevTrack_clicked()
{
    int val = ui->gmeTrackNumber->value();
    if(val > 0)
        ui->gmeTrackNumber->setValue(val - 1);
    else
        ui->gmeTrackNumber->setValue(0);
}

void CustomMusicSetup::on_gmeNextTrack_clicked()
{
    int val = ui->gmeTrackNumber->value();
    ui->gmeTrackNumber->setValue(val + 1);
}

void CustomMusicSetup::on_gmeTrackNumber_valueChanged(int)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_gmeGoToFirst_clicked()
{
    ui->gmeTrackNumber->setValue(0);
}

void CustomMusicSetup::on_gmeGain_valueChanged(int)
{}

void CustomMusicSetup::on_gmeGainAbs_valueChanged(double)
{
    buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_gmeGainReset_clicked()
{
    ui->gmeGain->setValue(100);
    ui->gmeGainAbs->setValue(1.0);
    if(!signalsBlocked())
        buildSettings();
    emit updateSongPlay();
}

void CustomMusicSetup::on_gmeTempo_valueChanged(int)
{}

void CustomMusicSetup::on_gmeTempoAbs_valueChanged(double arg1)
{
    buildSettings();
    emit updateSongTempo(arg1);
}

void CustomMusicSetup::on_gmeTempoReset_clicked()
{
    ui->gmeTempo->setValue(100);
    ui->gmeTempoAbs->setValue(1.0);
    if(!signalsBlocked())
        buildSettings();
    emit updateSongTempo(ui->gmeTempoAbs->value());
}

void CustomMusicSetup::on_playMusicProxy_clicked(bool checked)
{
    emit musicButtonClicked(checked);
}
