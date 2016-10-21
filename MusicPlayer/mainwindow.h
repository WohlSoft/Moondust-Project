#pragma once
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#ifndef MUSPLAY_USE_WINAPI
namespace Ui {
class MainWindow;
}
#endif

/*!
 * \brief Class of main application window
 */
class MainWindow
        #ifndef MUSPLAY_USE_WINAPI
        : public QMainWindow
        #endif
{
    #ifndef MUSPLAY_USE_WINAPI
    Q_OBJECT
    #endif

public:
    #ifndef MUSPLAY_USE_WINAPI
    explicit MainWindow(QWidget *parent = 0);
    #else
    enum Commands
    {
        CMD_Open=30000,
        CMD_Play,
        CMD_Stop,
        CMD_Volume,
        CMD_TrackID,
        CMD_TrackIDspin,
        CMD_MidiDevice,
        CMD_RecordWave,
        CMD_Bank,
        CMD_Tremolo,
        CMD_Vibrato,
        CMD_AdLibDrums,
        CMD_ScalableMod,
        CMD_SetDefault,
        GRP_GME,
        GRP_MIDI,
        GRP_ADLMIDI
    };
    static   MainWindow* m_self;
    static   LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static   LRESULT CALLBACK SubCtrlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    explicit MainWindow(HINSTANCE hInstance = 0, int nCmdShow = 0);

    LONG        m_height;

    WNDCLASSEXW m_wc;
    HINSTANCE   m_hInstance;

    LPCWSTR     m_MainWndClass;

    HWND        m_hWnd;

    HWND        m_buttonOpen;
    HWND        m_buttonPlay;
    HWND        m_buttonStop;

    HWND        m_volume;
    HWND        m_formatInfo;

    HWND        m_recordWave;

    HWND        m_labelTitle;
    HWND        m_labelArtist;
    HWND        m_labelAlboom;
    HWND        m_labelCopyright;

    HWND        m_groupGME;
    struct GroupGME
    {
        HWND    m_labelTrack;
        HWND    m_trackNum;
        HWND    m_trackNumUpDown;
    } m_gme;

    HWND        m_groupMIDI;
    struct GroupMIDI
    {
        HWND    m_labelDevice;
        HWND    m_midiDevice;
    } m_midi;

    HWND        m_groupADLMIDI;
    struct GroupADLMIDI
    {
        HWND    m_labelBank;
        HWND    m_bankID;
        HWND    m_tremolo;
        HWND    m_vibrato;
        HWND    m_adlibDrums;
        HWND    m_scalableMod;
        HWND    m_resetToDefault;
    } m_adlmidi;

    void        exec();
    #endif
    ~MainWindow();

    //! Full path to currently opened music
    QString currentMusic;

#ifndef MUSPLAY_USE_WINAPI
public slots:
    /*!
     * \brief Drop event, triggers when some object was dropped to main window
     * \param e Event parameters
     */
    void dropEvent(QDropEvent *e);

    /*!
     * \brief Drop enter event, triggers when mouse came to window with holden objects
     * \param e Event parameters
     */
    void dragEnterEvent(QDragEnterEvent *e);
#endif

    /*!
     * \brief Open music file
     * \param musPath full or relative path to music file to open
     */
    void openMusicByArg(QString musPath);

#ifndef MUSPLAY_USE_WINAPI
private slots:
#endif
    /*!
     * \brief Open button click event
     */
    void on_open_clicked();

    /*!
     * \brief Stop button click event
     */
    void on_stop_clicked();

    /*!
     * \brief Play button click event
     */
    void on_play_clicked();

    /*!
     * \brief Volume slider event
     * \param value Volume level
     */
    void on_volume_valueChanged(int value);

    /*!
     * \brief MIDI Device combo-box index change event
     * \param index New value of index after change
     */
    void on_mididevice_currentIndexChanged(int index);

    /*!
     * \brief FM Bank combo-box index change event
     * \param index New value of index after change
     */
    void on_fmbank_currentIndexChanged(int index);

    /*!
     * \brief Tremolo checkbox click event
     * \param checked New value of Tremolo checkbox
     */
    void on_tremolo_clicked(bool checked);

    /*!
     * \brief Vibrato checkbox click event
     * \param checked New value of Vibrato checkbox
     */
    void on_vibrato_clicked(bool checked);

    /*!
     * \brief Modulation scale checkbox click event
     * \param checked New value of Modulation scale checkbox
     */
    void on_modulation_clicked(bool checked);

    void on_adlibMode_clicked(bool checked);

    /*!
     * \brief Track-ID editing finished event
     */
    void on_trackID_editingFinished();

    /*!
     * \brief Triggers when 'record wav' checkbox was toggled
     * \param checked
     */
    void on_recordWav_clicked(bool checked);

    /*!
     * \brief Changes color of "Recording WAV" label between black and red
     */
    void _blink_red();

    /*!
     * \brief Resets ADLMIDI properties into initial state
     */
    void on_resetDefaultADLMIDI_clicked();

    #ifndef MUSPLAY_USE_WINAPI
    /*!
     * \brief Context menu
     * \param pos Mouse cursor position
     */
    void contextMenu(const QPoint &pos);
    #endif

private:
    #ifndef MUSPLAY_USE_WINAPI
    //! Controlls blinking of the wav-recording label
    QTimer m_blinker;
    #endif
    bool   m_blink_state;

    int    m_prevTrackID;

    #ifndef MUSPLAY_USE_WINAPI
    //! UI form class pointer
    Ui::MainWindow *ui;
    #endif
};

#endif // MAINWINDOW_H
