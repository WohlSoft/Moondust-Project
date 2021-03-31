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

#ifndef CUSTOM_MUSIC_SETUP_H
#define CUSTOM_MUSIC_SETUP_H

#include <QDialog>

class MainWindow;
namespace Ui {
class CustomMusicSetup;
}

class CustomMusicSetup : public QDialog
{
    Q_OBJECT

    enum MusicType
    {
        Unsupported = 0,
        MIDI,
        GME,
        ADLMIDI
    };

    int     adlDefaultBank = -1;
    int     adlDefaultChips = -1;
    int     adlDefaultVolumeModel = -1;

    int     opnDefaultChips = -1;
    int     opnDefaultVolumeModel = -1;

    int     fluidDefaultPolyphony = 256;

    int     fluidDefaultChorus = -1;
    int     fluidDefaultChorusN = 3;
    double  fluidDefaultChorusLevel = 2.0;
    double  fluidDefaultChorusSpeed = 0.3;
    double  fluidDefaultChorusDepth = 8.0;
    int     fluidDefaultChorusType = 0;

    int     fluidDefaultReverb = -1;
    double  fluidDefaultReverbRoom = 0.2;
    double  fluidDefaultReverbDamp = 0.0;
    double  fluidDefaultReverbWidth = 0.5;
    double  fluidDefaultReverbLevel = 0.9;

    QString m_music;
    QString m_musicName;
    QString m_musicArgs;

    //! Absolute path to the root directory (needed for the banks and/or soundfont browsing)
    QString m_directoryRoot;
    //! Name of the data sub-directory (not a path) at the root directory
    QString m_directoryData;

    MusicType m_type = Unsupported;
    bool m_settingsNeeded = false;

    static MusicType detectType(const QString &music);

    void initSetup();
    void parseSettings();
    void buildSettings();
    void updateVisibiltiy();
public:
    explicit CustomMusicSetup(QWidget *parent = nullptr);
    ~CustomMusicSetup();

    void initLists();

    void setDirectory(const QString &root, const QString &data);
    void setMusicPath(const QString &music);
    void setMusicPlayState(bool checked);
    bool settingsNeeded();
    static bool settingsNeeded(const QString &music);
    QString musicPath();

signals:
    void musicSetupChanged(const QString &music);
    void updateSongPlay();
    void updateSongTempo(double tempo);
    void musicButtonClicked(bool checked);

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

    void on_midiExAdlBrowsebank_clicked();
    void on_midiExAdlCustomBank_editingFinished();
    void on_midiExAdlBank_currentIndexChanged(int index);
    void on_midiExAdlVolumeModel_currentIndexChanged(int index);
    void on_midiExAdlChipsEn_clicked();
    void on_midiExAdlChips_valueChanged(int arg1);
    void on_midiExAdlDeepTremolo_clicked(bool checked);
    void on_midiExAdlDeepVibrato_clicked(bool checked);

    void on_midiExOpnBrowsebank_clicked();
    void on_midiExOpnCustomBank_editingFinished();
    void on_midiExOpnVolumeModel_currentIndexChanged(int index);
    void on_midiExOpnChipsEn_clicked();
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

    void on_playMusicProxy_clicked(bool checked);

    void on_midiExFluidMDragDropped(QModelIndex sourceParent, int sourceStart, int sourceEnd, QModelIndex destinationParent, int destinationRow);
    void on_midiExFluidSFAdd_clicked();
    void on_midiExFluidSFRemove_clicked();

    void on_midiExFluidPolyphony_valueChanged(int arg1);
    void on_midiExFluidReverb_clicked(bool checked);
    void on_midiExFluidRR_valueChanged(double arg1);
    void on_midiExFluidRD_valueChanged(double arg1);
    void on_midiExFluidRW_valueChanged(double arg1);
    void on_midiExFluidRL_valueChanged(double arg1);
    void on_midiExFluidChorus_clicked(bool checked);
    void on_midiExFluidCN_valueChanged(int arg1);
    void on_midiExFluidCL_valueChanged(double arg1);
    void on_midiExFluidCS_valueChanged(double arg1);
    void on_midiExFluidCD_valueChanged(double arg1);
    void on_midiExFluidCT_currentIndexChanged(int index);

private:
    Ui::CustomMusicSetup *ui;
};

#endif // CUSTOM_MUSIC_SETUP_H
