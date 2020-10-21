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

#ifndef CUSTOM_MUSIC_SETUP_H
#define CUSTOM_MUSIC_SETUP_H

#include <QDockWidget>

namespace Ui {
class CustomMusicSetup;
}

class CustomMusicSetup : public QDockWidget
{
    Q_OBJECT

    enum MusicType
    {
        Unsupported = 0,
        MIDI,
        GME,
        ADLMIDI
    };

    QString m_music;
    MusicType m_type = Unsupported;
    bool m_settingsNeeded = false;

    MusicType detectType();

    void initSetup();
    void parseSettings();
    void buildSettings();
    void updateVisibiltiy();
public:
    explicit CustomMusicSetup(QWidget *parent = nullptr);
    ~CustomMusicSetup();

    void initLists();

    void setMusicPath(const QString &music);
    bool settingsNeeded();
    QString musicPath();

signals:
    void musicSetupChanged(const QString &music);

protected:
    void changeEvent(QEvent *e);
    void retranslateLists();

private slots:
    void on_midiSynth_currentIndexChanged(int index);

    void on_midiGain_valueChanged(int value);
    void on_midiGainAbs_valueChanged(double arg1);
    void on_midiGainReset_clicked();

    void on_midiTempo_valueChanged(int value);
    void on_midiTempoAbs_valueChanged(double arg1);
    void on_midiTempoReset_clicked();

    void on_midiExAdlBank_currentIndexChanged(int index);
    void on_midiExAdlVolumeModel_currentIndexChanged(int index);
    void on_midiExAdlChips_valueChanged(int arg1);
    void on_midiExAdlDeepTremolo_clicked(bool checked);
    void on_midiExAdlDeepVibrato_clicked(bool checked);

    void on_midiExOpnVolumeModel_currentIndexChanged(int index);
    void on_midiExOpnChips_valueChanged(int arg1);

    void on_gmePrevTrack_clicked();
    void on_gmeNextTrack_clicked();
    void on_gmeTrackNumber_valueChanged(int arg1);
    void on_gmeGoToFirst_clicked();

    void on_gmeGain_valueChanged(int value);
    void on_gmeGainAbs_valueChanged(double arg1);
    void on_gmeGainReset_clicked();

    void on_gmeTempo_valueChanged(int value);
    void on_gmeTempoAbs_valueChanged(double arg1);
    void on_gmeTempoReset_clicked();

private:
    Ui::CustomMusicSetup *ui;
};

#endif // CUSTOM_MUSIC_SETUP_H
