#pragma once
#ifndef MUSPLAYERBASE_H
#define MUSPLAYERBASE_H

#ifndef MUSPLAY_USE_WINAPI
#include <QMainWindow>
#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTimer>
#else
#include <windows.h>
#include "defines.h"
#endif

class MusPlayerBase
{
    public:
        MusPlayerBase();
        virtual ~MusPlayerBase();

        /*!
         * \brief Open music file
         * \param musPath full or relative path to music file to open
         */
        virtual void openMusicByArg(QString musPath);

        /*!
         * \brief Open button click event
         */
        virtual void on_open_clicked();

        /*!
         * \brief Stop button click event
         */
        virtual void on_stop_clicked();

        /*!
         * \brief Play button click event
         */
        virtual void on_play_clicked();

        /*!
         * \brief Volume slider event
         * \param value Volume level
         */
        virtual void on_volume_valueChanged(int value);

        /*!
         * \brief MIDI Device combo-box index change event
         * \param index New value of index after change
         */
        virtual void on_mididevice_currentIndexChanged(int index);

        /*!
         * \brief FM Bank combo-box index change event
         * \param index New value of index after change
         */
        virtual void on_fmbank_currentIndexChanged(int index);

        /*!
         * \brief Volume model combobox index change event
         * \param index New value of volume model after change
         */
        virtual void on_volumeModel_currentIndexChanged(int index);

        /*!
         * \brief Tremolo checkbox click event
         * \param checked New value of Tremolo checkbox
         */
        virtual void on_tremolo_clicked(bool checked);

        /*!
         * \brief Vibrato checkbox click event
         * \param checked New value of Vibrato checkbox
         */
        virtual void on_vibrato_clicked(bool checked);

        /*!
         * \brief Modulation scale checkbox click event
         * \param checked New value of Modulation scale checkbox
         */
        virtual void on_modulation_clicked(bool checked);

        virtual void on_adlibMode_clicked(bool checked);

        virtual void on_logVolumes_clicked(bool checked);

        /*!
         * \brief Track-ID editing finished event
         */
        virtual void on_trackID_editingFinished();

        /*!
         * \brief Triggers when 'record wav' checkbox was toggled
         * \param checked
         */
        virtual void on_recordWav_clicked(bool checked);

        /*!
         * \brief Changes color of "Recording WAV" label between black and red
         */
        virtual void _blink_red();

        /*!
         * \brief Resets ADLMIDI properties into initial state
         */
        virtual void on_resetDefaultADLMIDI_clicked();

    protected:
        //! Full path to currently opened music
        QString currentMusic;

        bool   m_blink_state;
        int    m_prevTrackID;
};

#endif // MUSPLAYERBASE_H
