#include "mainwindow.h"
#ifndef MUSPLAY_USE_WINAPI
#include "ui_mainwindow.h"
#endif
#include "AssocFiles/assoc_files.h"
#include "Effects/reverb.h"
#include <math.h>

#ifndef MUSPLAY_USE_WINAPI
#include <QtDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#define DebugLog(msg) qDebug() << msg;
#else
#define DebugLog(msg)
#include <assert.h>
#include <windows.h>
#include <commctrl.h>
#endif

#include "version.h"

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main

#include "wave_writer.h"

#ifdef MUSPLAY_USE_WINAPI
std::string Wstr2Str(const std::wstring &in)
{
    std::string out;
    out.resize(in.size()*2);
    int res = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)in.c_str(), in.size(), (char*)out.c_str(), out.size(), 0, 0);
    out.resize(res);
    return out;
}

std::wstring Str2Wstr(const std::string &in)
{
    std::wstring out;
    out.resize(in.size());
    int res = MultiByteToWideChar(CP_UTF8, 0, (char*)in.c_str(), in.size(), (wchar_t*)out.c_str(), out.size());
    out.resize(res);
    return out;
}
#endif

/*!
 *  SDL Mixer wrapper
 */
namespace PGE_MusicPlayer
{
    Mix_Music *play_mus = NULL;
    Mix_MusicType type  = MUS_NONE;
    bool reverbEnabled = false;

    const char* musicTypeC()
    {
        return (
        type == MUS_NONE?"No Music":
        type == MUS_CMD?"CMD":
        type == MUS_WAV?"PCM Wave":
        type == MUS_MOD?"MidMod":
        type == MUS_MODPLUG?"ModPlug":
        type == MUS_MID?"MIDI/IMF":
        type == MUS_OGG?"OGG":
        type == MUS_MP3?"MP3 (SMPEG)":
        type == MUS_MP3_MAD?"MP3 (LibMAD)":
        type == MUS_FLAC?"FLAC":
        type == MUS_SPC?"Game Music Emulator": "<Unknown>" );
    }
    QString musicType()
    {
        return QString( musicTypeC() );
    }

    /*!
     * \brief Spawn warning message box with specific text
     * \param msg text to spawn in message box
     */
    static void error(QString msg)
    {
        #ifndef MUSPLAY_USE_WINAPI
        QMessageBox::warning(nullptr, "SDL2 Mixer ext error", msg, QMessageBox::Ok);
        #else
        MessageBoxA(NULL, msg.c_str(), "SDL2 Mixer ext error", MB_OK|MB_ICONWARNING);
        #endif
    }

    /*!
     * \brief Stop music playing
     */
    void MUS_stopMusic()
    {
        Mix_HaltMusic();
    }

    #ifndef MUSPLAY_USE_WINAPI
    /*!
     * \brief Get music title of current track
     * \return music title of current music file
     */
    QString MUS_getMusTitle()
    {
        if(play_mus)
            return QString(Mix_GetMusicTitle(play_mus));
        else
            return QString("[No music]");
    }

    /*!
     * \brief Get music artist tag text of current music track
     * \return music artist tag text of current music track
     */
    QString MUS_getMusArtist()
    {
        if(play_mus)
            return QString(Mix_GetMusicArtistTag(play_mus));
        else
            return QString("[Unknown Artist]");
    }

    /*!
     * \brief Get music album tag text of current music track
     * \return music ablum tag text of current music track
     */
    QString MUS_getMusAlbum()
    {
        if(play_mus)
            return QString(Mix_GetMusicAlbumTag(play_mus));
        else
            return QString("[Unknown Album]");
    }

    /*!
     * \brief Get music copyright tag text of current music track
     * \return music copyright tag text of current music track
     */
    QString MUS_getMusCopy()
    {
        if(play_mus)
            return QString(Mix_GetMusicCopyrightTag(play_mus));
        else
            return QString("");
    }
    #else
    /*!
     * \brief Get music title of current track
     * \return music title of current music file
     */
    const char* MUS_getMusTitle()
    {
        if(play_mus)
            return Mix_GetMusicTitle(play_mus);
        else
            return "[No music]";
    }

    /*!
     * \brief Get music artist tag text of current music track
     * \return music artist tag text of current music track
     */
    const char* MUS_getMusArtist()
    {
        if(play_mus)
            return Mix_GetMusicArtistTag(play_mus);
        else
            return "[Unknown Artist]";
    }

    /*!
     * \brief Get music album tag text of current music track
     * \return music ablum tag text of current music track
     */
    const char* MUS_getMusAlbum()
    {
        if(play_mus)
            return Mix_GetMusicAlbumTag(play_mus);
        else
            return "[Unknown Album]";
    }

    /*!
     * \brief Get music copyright tag text of current music track
     * \return music copyright tag text of current music track
     */
    const char* MUS_getMusCopy()
    {
        if(play_mus)
            return Mix_GetMusicCopyrightTag(play_mus);
        else
            return "";
    }
    #endif

    /*!
     * \brief Start playing of currently opened music track
     */
    void MUS_playMusic()
    {
        if(play_mus)
        {
            if(Mix_PlayMusic(play_mus, -1)==-1)
            {
                error(QString("Mix_PlayMusic: ") + Mix_GetError());
                // well, there's no music, but most games don't break without music...
            }
            //QString("Music is %1\n").arg(Mix_PlayingMusic()==1?"Playing":"Silence");
        }
        else
        {
            error(QString("Play nothing: Mix_PlayMusic: ") + Mix_GetError());
        }
        //qDebug() << QString("Check Error of SDL: %1\n").arg(Mix_GetError());
    }

    /*!
     * \brief Sets volume level of current music stream
     * \param volume level of volume from 0 tp 128
     */
    void MUS_changeVolume(int volume)
    {
        Mix_VolumeMusic(volume);
        DebugLog(QString("Mix_VolumeMusic: %1\n").arg(volume));
    }

    /*!
     * \brief Open music file
     * \param musFile Full path to music file
     * \return true if music file was successfully opened, false if loading was failed
     */
    bool MUS_openFile(QString musFile)
    {
        type = MUS_NONE;
        if(play_mus != NULL) { Mix_FreeMusic(play_mus); play_mus=NULL; }
        #ifndef MUSPLAY_USE_WINAPI
        play_mus = Mix_LoadMUS( musFile.toUtf8().data() );
        #else
        play_mus = Mix_LoadMUS( musFile.c_str() );
        #endif
        if(!play_mus) {
            error(QString("Mix_LoadMUS(\"" + QString(musFile) + "\"): ") + Mix_GetError());
            return false;
        }
        type = Mix_GetMusicType( play_mus );
        DebugLog(QString("Music type: %1").arg( musicType() ));
        return true;
    }

    static bool wavOpened=false;

    // make a music play function
    // it expects udata to be a pointer to an int
    void myMusicPlayer(void */*udata*/, Uint8 *stream, int len)
    {
        wave_write( (short*)stream, len/2 );
    }

    void startWavRecording(QString target)
    {
        if(wavOpened) return;
        if(!play_mus) return;

        /* Record 20 seconds to wave file */
        #ifndef MUSPLAY_USE_WINAPI
        wave_open( 44100, target.toLocal8Bit().data() );
        #else
        wave_open( 44100, target.c_str() );
        #endif
        wave_enable_stereo();
        Mix_SetPostMix(myMusicPlayer, NULL);
        wavOpened=true;
    }

    void stopWavRecording()
    {
        if(!wavOpened) return;
        wave_close();
        Mix_SetPostMix(NULL, NULL);
        wavOpened=false;
    }

}

#ifndef MUSPLAY_USE_WINAPI
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    #ifdef Q_OS_MAC
    this->setWindowIcon(QIcon(":/cat_musplay.icns"));
    #endif
    #ifdef Q_OS_WIN
    this->setWindowIcon(QIcon(":/cat_musplay.ico"));
    #endif

    ui->fmbank->clear();
    int totalBakns = MIX_ADLMIDI_getTotalBanks();
    const char*const* names = MIX_ADLMIDI_getBankNames();
    for(int i=0; i<totalBakns; i++)
    {
        ui->fmbank->addItem(QString("%1 = %2").arg(i).arg(names[i]));
    }

    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->gme_setup->setVisible(false);
    ui->centralWidget->window()->setWindowFlags(
                Qt::WindowTitleHint|
                Qt::WindowSystemMenuHint|
                Qt::WindowCloseButtonHint|
                Qt::WindowMinimizeButtonHint);
    this->updateGeometry();
    this->window()->resize(100, 100);
    this->connect(&m_blinker, SIGNAL(timeout()), this, SLOT(_blink_red()));
    this->connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));

    QApplication::setOrganizationName(_COMPANY);
    QApplication::setOrganizationDomain(_PGE_URL);
    QApplication::setApplicationName("PGE Music Player");
    QSettings setup;
    restoreGeometry(setup.value("Window-Geometry").toByteArray());
    ui->mididevice->setCurrentIndex(setup.value("MIDI-Device", 0).toInt());
    switch(ui->mididevice->currentIndex())
    {
        case 0: MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
        case 1: MIX_SetMidiDevice(MIDI_Timidity);
        ui->adlmidi_xtra->setVisible(false);
        break;
        case 2: MIX_SetMidiDevice(MIDI_Native);
        ui->adlmidi_xtra->setVisible(false);
        break;
        default: MIX_SetMidiDevice(MIDI_ADLMIDI);
        ui->adlmidi_xtra->setVisible(true);
        break;
    }
    ui->fmbank->setCurrentIndex(setup.value("ADLMIDI-Bank-ID", 58).toInt());
    MIX_ADLMIDI_setBankID( ui->fmbank->currentIndex() );
    ui->tremolo->setChecked(setup.value("ADLMIDI-Tremolo", true).toBool());
    MIX_ADLMIDI_setTremolo((int)ui->tremolo->isChecked());
    ui->vibrato->setChecked(setup.value("ADLMIDI-Vibrato", true).toBool());
    MIX_ADLMIDI_setVibrato((int)ui->vibrato->isChecked());
    ui->adlibMode->setChecked(setup.value("ADLMIDI-AdLib-Drums-Mode", false).toBool());
    MIX_ADLMIDI_setAdLibMode((int)ui->adlibMode->isChecked());
    ui->modulation->setChecked(setup.value("ADLMIDI-Scalable-Modulation", false).toBool());
    MIX_ADLMIDI_setScaleMod((int)ui->modulation->isChecked());
    ui->volume->setValue(setup.value("Volume", 128).toInt());

    m_prevTrackID = ui->trackID->value();
#else

MainWindow* MainWindow::m_self = nullptr;

LRESULT MainWindow::MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_HSCROLL:
        {
            if(m_self->m_volume == (HWND)lParam)
            {
                switch(LOWORD(wParam))
                {
                case TB_ENDTRACK:
                case TB_THUMBPOSITION:
                case TB_THUMBTRACK:
                case SB_LEFT:
                case SB_RIGHT:
                    DWORD dwPos;// current position of slider
                    dwPos = SendMessageW(m_self->m_volume, TBM_GETPOS, 0, 0);
                    SendMessageW(m_self->m_volume, TBM_SETPOS,
                                (WPARAM)TRUE,               //redraw flag
                                (LPARAM)dwPos);
                    m_self->on_volume_valueChanged(dwPos);
                    return 0;
                default:
                    break;
                }
            }
            break;
        }
        case WM_COMMAND:
        {
            switch(HIWORD(wParam))
            {
                case BN_CLICKED:
                {
                    switch( LOWORD(wParam) )
                    {
                    case CMD_Open:
                        m_self->on_open_clicked();
                        return 0;
                    case CMD_Play:
                        m_self->on_play_clicked();
                        return 0;
                    case CMD_Stop:
                        m_self->on_stop_clicked();
                        return 0;
                    case CMD_SetDefault:
                        m_self->on_resetDefaultADLMIDI_clicked();
                        return 0;
                    case CMD_RecordWave:
                    {
                        BOOL checked = IsDlgButtonChecked(hWnd, CMD_RecordWave);
                        if (checked)
                        {
                            CheckDlgButton(hWnd, CMD_RecordWave, BST_UNCHECKED);
                            m_self->on_recordWav_clicked(false);
                        } else {
                            CheckDlgButton(hWnd, CMD_RecordWave, BST_CHECKED);
                            m_self->on_recordWav_clicked(true);
                        }
                    }
                    default:
                        break;
                    }
                    break;
                }
            }
            break;
        }
        case WM_DROPFILES:
        {
            HDROP hDropInfo = (HDROP)wParam;
            wchar_t sItem[MAX_PATH];
            memset(sItem, 0, MAX_PATH*sizeof(wchar_t));
            if(DragQueryFileW(hDropInfo, 0, (LPWSTR)sItem, sizeof(sItem)))
            {
                m_self->openMusicByArg(Wstr2Str(sItem));
            }
            DragFinish(hDropInfo);
            return 0;
        }
        //Инфо о минимальном и максимальном размере окна
        case WM_GETMINMAXINFO:
        {
            MINMAXINFO *minMax = (MINMAXINFO*)lParam;
            minMax->ptMinTrackSize.x = 350;
            minMax->ptMinTrackSize.y = m_self->m_height;
            return 0;
        }
        case WM_CREATE:
        {
            return 0;
        }
        //Окно было закрыто
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::SubCtrlProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static WNDPROC OrigBtnProc = reinterpret_cast<WNDPROC>(static_cast<LONG_PTR>(GetWindowLongPtr(hWnd, GWLP_USERDATA)));
    switch (msg)
    {
        case WM_COMMAND:
        {
            switch( LOWORD(wParam) )
            {
            case CMD_TrackID:
            {
                switch(HIWORD(wParam))
                {
                case  EN_UPDATE:
                    {
                        char trackIDtext[1024];
                        memset(trackIDtext, 0, 1024);
                        GetWindowTextA(m_self->m_gme.m_trackNum, trackIDtext, 1024);
                        //int trackID = atoi(trackIDtext);
                        SetWindowTextA(m_self->m_gme.m_labelTrack, trackIDtext);
                        m_self->on_trackID_editingFinished();
                        goto defaultWndProc;
                    }
                }
                goto defaultWndProc;
            }
            case CMD_MidiDevice:
            {
                switch(HIWORD(wParam))
                {
                    case CBN_SELCHANGE:
                    {
                        LRESULT index = SendMessageW(m_self->m_midi.m_midiDevice, CB_GETCURSEL, 0, 0);
                        if(index != CB_ERR)
                            m_self->on_mididevice_currentIndexChanged(index);
                        goto defaultWndProc;
                    }
                    break;
                    default:
                    break;
                }
                goto defaultWndProc;
            }
            case CMD_Bank:
            {
                switch(HIWORD(wParam))
                {
                    case CBN_SELCHANGE:
                    {
                        LRESULT index = SendMessageW(m_self->m_adlmidi.m_bankID, CB_GETCURSEL, 0, 0);
                        if(index != CB_ERR)
                            m_self->on_fmbank_currentIndexChanged(index);
                        goto defaultWndProc;
                    }
                    break;
                    default:
                    break;
                }
                goto defaultWndProc;
            }
            case CMD_Tremolo:
            {
                BOOL checked = IsDlgButtonChecked(hWnd, CMD_Tremolo);
                if (checked)
                {
                    CheckDlgButton(hWnd, CMD_Tremolo, BST_UNCHECKED);
                    m_self->on_tremolo_clicked(false);
                } else {
                    CheckDlgButton(hWnd, CMD_Tremolo, BST_CHECKED);
                    m_self->on_tremolo_clicked(true);
                }
                goto defaultWndProc;
            }
            case CMD_Vibrato:
            {
                BOOL checked = IsDlgButtonChecked(hWnd, CMD_Vibrato);
                if (checked)
                {
                    CheckDlgButton(hWnd, CMD_Vibrato, BST_UNCHECKED);
                    m_self->on_vibrato_clicked(false);
                } else {
                    CheckDlgButton(hWnd, CMD_Vibrato, BST_CHECKED);
                    m_self->on_vibrato_clicked(true);
                }
                goto defaultWndProc;
            }
            case CMD_ScalableMod:
            {
                BOOL checked = IsDlgButtonChecked(hWnd, CMD_ScalableMod);
                if (checked)
                {
                    CheckDlgButton(hWnd, CMD_ScalableMod, BST_UNCHECKED);
                    m_self->on_modulation_clicked(false);
                } else {
                    CheckDlgButton(hWnd, CMD_ScalableMod, BST_CHECKED);
                    m_self->on_modulation_clicked(true);
                }
                goto defaultWndProc;
            }
            case CMD_AdLibDrums:
            {
                BOOL checked = IsDlgButtonChecked(hWnd, CMD_AdLibDrums);
                if (checked)
                {
                    CheckDlgButton(hWnd, CMD_AdLibDrums, BST_UNCHECKED);
                    m_self->on_adlibMode_clicked(false);
                } else {
                    CheckDlgButton(hWnd, CMD_AdLibDrums, BST_CHECKED);
                    m_self->on_adlibMode_clicked(true);
                }
                goto defaultWndProc;
            }
            default:
                break;
            }
        }
    }

defaultWndProc:
    return CallWindowProc(OrigBtnProc, hWnd, msg, wParam, lParam);
}

MainWindow::MainWindow(HINSTANCE hInstance, int nCmdShow)
{
    assert(!MainWindow::m_self && "Multiple instances of MainWindow are Forbidden! It's singleton!");
    MainWindow::m_self = this;
    m_MainWndClass = L"PGEMusPlayer";
    m_hInstance = hInstance;

    // Class for our main window.
    memset(&m_wc, 0, sizeof(m_wc));
    m_wc.cbSize        = sizeof(m_wc);
    m_wc.style         = 0;
    m_wc.lpfnWndProc   = &MainWindow::MainWndProc;
    m_wc.cbClsExtra    = 0;
    m_wc.cbWndExtra    = 0;
    m_wc.hInstance     = hInstance;
    m_wc.hIcon         = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 0, 0,
                                         LR_DEFAULTSIZE | LR_DEFAULTCOLOR | LR_SHARED);
    m_wc.hCursor       = (HCURSOR) LoadImage(NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_SHARED);
    m_wc.hbrBackground = (HBRUSH) (COLOR_BTNFACE+1);
    m_wc.lpszMenuName  = NULL;
    m_wc.lpszClassName = m_MainWndClass;
    m_wc.hIconSm       = (HICON) LoadImage(hInstance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON,
                                            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON),
                                            LR_DEFAULTCOLOR | LR_SHARED);

    // Register our window classes, or error.
    if(!RegisterClassExW(&m_wc))
    {
        MessageBoxA(NULL, "Error registering window class.", "Error", MB_ICONERROR | MB_OK);
        return;
    }


    m_height = 170;
    // Create instance of main window.
    m_hWnd = CreateWindowExW(WS_EX_ACCEPTFILES,
                             m_MainWndClass,
                             m_MainWndClass,
                             WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             320, m_height, NULL, NULL, hInstance, NULL);

    HFONT hFont = CreateFontW(-11, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                               0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                               DEFAULT_PITCH | FF_SWISS, L"Tahoma");
    HFONT hFontBold = CreateFontW(-11, 0, 0, 0, FW_BOLD, FALSE, FALSE,
                               0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
                               CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                               DEFAULT_PITCH | FF_SWISS, L"Tahoma");
    int left = 5;
    int top = 10;

    HWND hText = CreateWindowExW(0, L"STATIC", L"Press \"Open\" or drag&&drop music file into this window",
                                WS_CHILD | WS_VISIBLE | SS_LEFT,
                                left, top, 400, 15,
                                m_hWnd, NULL, hInstance, NULL);
    top += 25;

    m_labelTitle = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                left, top, 400, 15,
                                m_hWnd, NULL, hInstance, NULL);
    top += 15;
    m_labelArtist = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                left, top, 400, 15,
                                m_hWnd, NULL, hInstance, NULL);
    top += 15;
    m_labelAlboom = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                left, top, 400, 15,
                                m_hWnd, NULL, hInstance, NULL);
    top += 15;
    m_labelCopyright = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                left, top, 400, 15,
                                m_hWnd, NULL, hInstance, NULL);
    top += 15;


    m_buttonOpen = CreateWindowExW(0, L"BUTTON", L"Open", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                                left, top, 60, 21, m_hWnd,
                                (HMENU)CMD_Open, // Here is the ID of your button ( You may use your own ID )
                                hInstance, NULL);
    left += 60;

    m_buttonPlay = CreateWindowExW(0, L"BUTTON", L"Play/Pause", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                                left, top, 60, 21, m_hWnd,
                                (HMENU)CMD_Play, // Here is the ID of your button ( You may use your own ID )
                                hInstance, NULL);
    left += 60;

    m_buttonStop = CreateWindowExW(0, L"BUTTON", L"Stop", WS_VISIBLE|WS_CHILD|BS_DEFPUSHBUTTON,
                                left, top, 60, 21, m_hWnd,
                                (HMENU)CMD_Stop, // Here is the ID of your button ( You may use your own ID )
                                hInstance, NULL);

    left += 60;
    m_volume    = CreateWindowExW(0, TRACKBAR_CLASS, L"Volume", WS_VISIBLE|WS_CHILD,
                                  left, top-2, 80, 25, m_hWnd,
                                  (HMENU)CMD_Volume, // Here is the ID of your button ( You may use your own ID )
                                  hInstance, NULL);
    SendMessageW(m_volume, TBM_SETRANGE,
                (WPARAM)TRUE,               //redraw flag
                (LPARAM)MAKELONG(0, 128));  //min. & max. positions
    SendMessageW(m_volume, TBM_SETPOS,
                (WPARAM)TRUE,               //redraw flag
                (LPARAM)128);
    SendMessageW(m_volume, WM_SETFONT, (WPARAM)hFont, 0);
    left += 80;

    m_formatInfo = CreateWindowExW(0, L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                   left, top, 200, 15,
                                   m_hWnd, NULL, hInstance, NULL);
    SendMessageW(m_formatInfo, WM_SETFONT, (WPARAM)hFont, 0);

    left = 5;
    top += 21;

    m_recordWave = CreateWindowExW(0, L"BUTTON", L"Record wave", WS_VISIBLE|WS_CHILD|SS_LEFT|BS_CHECKBOX,
                                                             left, top, 200, 15,
                                                             m_hWnd, (HMENU)CMD_RecordWave, hInstance, NULL);
    SendMessageW(m_recordWave, WM_SETFONT, (WPARAM)hFont, 0);
    CheckDlgButton(m_groupADLMIDI, CMD_RecordWave, BST_UNCHECKED);

    top += 18;

    WNDPROC OldBtnProc;

    m_groupGME = CreateWindowExW(0, L"BUTTON", L"Game Music Emulators setup", WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
                                left, top, 330, 50, m_hWnd, (HMENU)GRP_GME, hInstance, NULL);
    SendMessageW(m_groupGME, WM_SETFONT, (WPARAM)hFont, 0);
    OldBtnProc = reinterpret_cast<WNDPROC>(static_cast<LONG_PTR>(
                 SetWindowLongPtr(m_groupGME, GWLP_WNDPROC,
                 reinterpret_cast<LONG_PTR>(MainWindow::SubCtrlProc))) );
    SetWindowLongPtr(m_groupGME, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(OldBtnProc));

    m_gme.m_labelTrack = CreateWindowExW(0, L"STATIC", L"Track number:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                10, 20, 70, 15,
                                 m_groupGME, NULL, hInstance, NULL);
    SendMessageW(m_gme.m_labelTrack, WM_SETFONT, (WPARAM)hFont, 0);

    m_gme.m_trackNum = CreateWindowExW(0, L"EDIT", L"ED_RED",
                                    WS_CHILD|WS_VISIBLE|ES_LEFT|WS_BORDER,
                                    80, 20, 240, 20,
                                    m_groupGME, (HMENU)(CMD_TrackID),
                                    hInstance, NULL);

    SendMessageW(m_gme.m_trackNum, WM_SETFONT, (WPARAM)hFont, 0);
    // with a spin control to its right side
    m_gme.m_trackNumUpDown = CreateWindowExW(0, UPDOWN_CLASS, L"SP_RED",
                                            WS_CHILD | WS_VISIBLE
                                            | UDS_ARROWKEYS | UDS_ALIGNRIGHT
                                            | UDS_SETBUDDYINT | WS_BORDER,
                                            80, 20, 240, 20,
                                            m_groupGME, (HMENU)(CMD_TrackIDspin), hInstance, NULL);
    SendMessageW(m_gme.m_trackNumUpDown, WM_SETFONT, (WPARAM)hFont, 0);

    // Set the buddy control.
    SendMessage(m_gme.m_trackNumUpDown, UDM_SETBUDDY, (LONG)m_gme.m_trackNum, 0L);
    // Set the range.
    SendMessage(m_gme.m_trackNumUpDown, UDM_SETRANGE, 0L, MAKELONG(32000, 0) );
    // Set the initial value
    SendMessage(m_gme.m_trackNumUpDown, UDM_SETPOS, 0L, MAKELONG((int)(0), 0));

    //SendMessage(m_gme.m_trackNumUpDown, UDS_WRAP, 0L, FALSE);




    m_groupMIDI = CreateWindowExW(0, L"BUTTON", L"MIDI Setup", WS_VISIBLE|WS_CHILD|BS_GROUPBOX|WS_GROUP,
                                left, top, 330, 50,
                                m_hWnd, (HMENU)GRP_MIDI, hInstance, NULL);
    SendMessageW(m_groupMIDI, WM_SETFONT, (WPARAM)hFont, 0);

    OldBtnProc = reinterpret_cast<WNDPROC>(static_cast<LONG_PTR>(
                 SetWindowLongPtr(m_groupMIDI, GWLP_WNDPROC,
                 reinterpret_cast<LONG_PTR>(MainWindow::SubCtrlProc))) );
    SetWindowLongPtr(m_groupMIDI, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(OldBtnProc));

    top += 50;

    m_midi.m_labelDevice = CreateWindowExW(0, L"STATIC", L"MIDI Device:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                10, 20, 70, 15,
                                m_groupMIDI, NULL, hInstance, NULL);
    SendMessageW(m_midi.m_labelDevice, WM_SETFONT, (WPARAM)hFont, 0);

    const wchar_t* midiDevices[] = {
        L"ADL Midi (OPL Synth emulation)",
        L"Timidity (GUS-like wavetable MIDI Synth)",
        L"Native midi (Built-in MIDI of your OS)"
    };
    m_midi.m_midiDevice = CreateWindowExW(0, L"COMBOBOX", L"MidiDevice", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST|WS_TABSTOP,
                                              80, 17, 240, 210, m_groupMIDI, (HMENU)CMD_MidiDevice, hInstance, NULL);
    SendMessageW(m_midi.m_midiDevice, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_midi.m_midiDevice, CB_ADDSTRING, 0, (LPARAM)midiDevices[0]);
    SendMessageW(m_midi.m_midiDevice, CB_ADDSTRING, 0, (LPARAM)midiDevices[1]);
    SendMessageW(m_midi.m_midiDevice, CB_ADDSTRING, 0, (LPARAM)midiDevices[2]);
    SendMessageW(m_midi.m_midiDevice, CB_SETCURSEL, 0, 0);

    m_groupADLMIDI = CreateWindowExW(0, L"BUTTON", L"ADLMIDI Extra Setup", WS_VISIBLE|WS_CHILD|BS_GROUPBOX|WS_GROUP,
                                left, top, 330, 125,
                                m_hWnd, (HMENU)GRP_ADLMIDI, hInstance, NULL);
    SendMessageW(m_groupADLMIDI, WM_SETFONT, (WPARAM)hFont, 0);
    //top += 50;

    OldBtnProc=reinterpret_cast<WNDPROC>(static_cast<LONG_PTR>(
                 SetWindowLongPtr(m_groupADLMIDI, GWLP_WNDPROC,
                 reinterpret_cast<LONG_PTR>(MainWindow::SubCtrlProc))) );
    SetWindowLongPtr(m_groupADLMIDI, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(OldBtnProc));


    m_adlmidi.m_labelBank = CreateWindowExW(0, L"STATIC", L"ADL Midi bank ID:", WS_CHILD | WS_VISIBLE | SS_LEFT,
                                10, 20, 70, 15,
                                m_groupADLMIDI, NULL, hInstance, NULL);
    SendMessageW(m_adlmidi.m_labelBank, WM_SETFONT, (WPARAM)hFont, 0);

    m_adlmidi.m_bankID = CreateWindowExW(0, L"COMBOBOX", L"BankId", WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST|CBS_DISABLENOSCROLL|WS_VSCROLL|CBS_NOINTEGRALHEIGHT|WS_TABSTOP,
                                              80, 17, 240, 210, m_groupADLMIDI, (HMENU)CMD_Bank, hInstance, NULL);
    SendMessageW(m_adlmidi.m_bankID, WM_SETFONT, (WPARAM)hFont, 0);
    int insCount            = MIX_ADLMIDI_getTotalBanks();
    const char*const* names = MIX_ADLMIDI_getBankNames();
    for(int i=0; i<insCount; i++)
    {
        SendMessageA(m_adlmidi.m_bankID, CB_ADDSTRING, 0, (LPARAM)names[i]);
    }
    SendMessageW(m_adlmidi.m_bankID, CB_SETCURSEL, 58, 0);


    m_adlmidi.m_tremolo = CreateWindowExW(0, L"BUTTON", L"Deep tremolo", WS_VISIBLE|WS_CHILD|SS_LEFT|BS_CHECKBOX,
                                          10, 40, 200, 15,
                                          m_groupADLMIDI, (HMENU)CMD_Tremolo, hInstance, NULL);
    SendMessageW(m_adlmidi.m_tremolo, WM_SETFONT, (WPARAM)hFont, 0);
    CheckDlgButton(m_groupADLMIDI, CMD_Tremolo, BST_CHECKED);

    m_adlmidi.m_vibrato = CreateWindowExW(0, L"BUTTON", L"Deep vibrato", WS_VISIBLE|WS_CHILD|SS_LEFT|BS_CHECKBOX,
                                          10, 60, 200, 15,
                                          m_groupADLMIDI, (HMENU)CMD_Vibrato, hInstance, NULL);
    SendMessageW(m_adlmidi.m_vibrato, WM_SETFONT, (WPARAM)hFont, 0);
    CheckDlgButton(m_groupADLMIDI, CMD_Vibrato, BST_CHECKED);

    m_adlmidi.m_scalableMod = CreateWindowExW(0, L"BUTTON", L"Scalable modulation", WS_VISIBLE|WS_CHILD|SS_LEFT|BS_CHECKBOX,
                                          10, 80, 200, 15,
                                          m_groupADLMIDI, (HMENU)CMD_ScalableMod, hInstance, NULL);
    SendMessageW(m_adlmidi.m_scalableMod, WM_SETFONT, (WPARAM)hFont, 0);
    CheckDlgButton(m_groupADLMIDI, CMD_ScalableMod, BST_UNCHECKED);

    m_adlmidi.m_adlibDrums = CreateWindowExW(0, L"BUTTON", L"AdLib drums mode", WS_VISIBLE|WS_CHILD|SS_LEFT|BS_CHECKBOX,
                                          10, 100, 200, 15,
                                          m_groupADLMIDI, (HMENU)CMD_AdLibDrums, hInstance, NULL);
    SendMessageW(m_adlmidi.m_adlibDrums, WM_SETFONT, (WPARAM)hFont, 0);
    CheckDlgButton(m_groupADLMIDI, CMD_AdLibDrums, BST_UNCHECKED);


    ShowWindow(m_groupGME, SW_HIDE);
    ShowWindow(m_groupMIDI, SW_HIDE);
    ShowWindow(m_groupADLMIDI, SW_HIDE);


    SendMessageW(m_hWnd, WM_SETFONT, (WPARAM)hFont, 0);

    SendMessageW(m_buttonOpen, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_buttonPlay, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_buttonStop, WM_SETFONT, (WPARAM)hFont, 0);

    SendMessageW(hText, WM_SETFONT, (WPARAM)hFontBold, 0);

    SendMessageW(m_labelTitle, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_labelArtist, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_labelAlboom, WM_SETFONT, (WPARAM)hFont, 0);
    SendMessageW(m_labelCopyright, WM_SETFONT, (WPARAM)hFont, 0);

    SetWindowTextW(m_hWnd, L"Simple SDL2 Mixer X Music player [WinAPI-based]");

    // Show window and force a paint.
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);

#endif
    m_blink_state=false;
    m_prevTrackID = 0;
}

#ifdef MUSPLAY_USE_WINAPI
void MainWindow::exec()
{
    MSG msg;

    // Main message loop.
    while(GetMessage(&msg, NULL, 0, 0) > 0)
    {
        if (!TranslateAccelerator(msg.hwnd, NULL, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
#endif

MainWindow::~MainWindow()
{
    on_stop_clicked();
    Mix_CloseAudio();

    #ifndef MUSPLAY_USE_WINAPI
    QSettings setup;
    setup.setValue("Window-Geometry", saveGeometry());
    setup.setValue("MIDI-Device", ui->mididevice->currentIndex());
    setup.setValue("ADLMIDI-Bank-ID", ui->fmbank->currentIndex());
    setup.setValue("ADLMIDI-Tremolo", ui->tremolo->isChecked());
    setup.setValue("ADLMIDI-Vibrato", ui->vibrato->isChecked());
    setup.setValue("ADLMIDI-AdLib-Drums-Mode", ui->adlibMode->isChecked());
    setup.setValue("ADLMIDI-Scalable-Modulation", ui->modulation->isChecked());
    setup.setValue("Volume", ui->volume->value());
    delete ui;
    #else
    MainWindow::m_self = nullptr;
    #endif
}

#ifndef MUSPLAY_USE_WINAPI
void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}
#endif

void MainWindow::openMusicByArg(QString musPath)
{
    #ifndef MUSPLAY_USE_WINAPI
    if(ui->recordWav->isChecked()) return;
    #else
    BOOL waveRecorderInProcess = IsDlgButtonChecked(m_hWnd, CMD_RecordWave);
    if(waveRecorderInProcess) return;
    #endif
    currentMusic=musPath;
    #ifndef MUSPLAY_USE_WINAPI
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    #endif
    Mix_HaltMusic();
    on_play_clicked();
}

#ifndef MUSPLAY_USE_WINAPI
void MainWindow::dropEvent(QDropEvent *e)
{
    this->raise();
    this->setFocus(Qt::ActiveWindowFocusReason);

    if(ui->recordWav->isChecked())
    {
        return;
    }
    foreach (const QUrl &url, e->mimeData()->urls()) {
        const QString &fileName = url.toLocalFile();
        currentMusic=fileName;
    }
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    Mix_HaltMusic();
    on_play_clicked();
    this->raise();
    e->accept();
}
#endif

void MainWindow::on_open_clicked()
{
    #ifndef MUSPLAY_USE_WINAPI
    QString file = QFileDialog::getOpenFileName(this, tr("Open music file"),
                                                 (currentMusic.isEmpty() ? QApplication::applicationDirPath() : currentMusic), "All (*.*)");
    if(file.isEmpty()) return;
    currentMusic=file;
    ui->recordWav->setEnabled(!currentMusic.endsWith(".wav", Qt::CaseInsensitive));//Avoid self-trunkling!
    #else
    OPENFILENAMEW open;
    memset(&open, 0, sizeof(open));

    wchar_t flnm[MAX_PATH];
    flnm[0]=L'\0';
    open.lStructSize = sizeof(OPENFILENAMEW);
    open.hInstance = m_hInstance;
    open.hwndOwner = m_hWnd;
    open.lpstrFilter = L"All files (*.*)\0*.*";
    open.lpstrDefExt = L"\0";
    open.lpstrFile = flnm;
    open.nMaxFile = MAX_PATH;
    open.lpstrTitle = L"Open music file to play...";
    open.Flags = OFN_FILEMUSTEXIST;
    if(GetOpenFileNameW(&open)==TRUE)
    {
        currentMusic = Wstr2Str(open.lpstrFile);
    } else {
        return;
    }
    #endif
    Mix_HaltMusic();
    on_play_clicked();
}

void MainWindow::on_stop_clicked()
{
    PGE_MusicPlayer::MUS_stopMusic();
    #ifndef MUSPLAY_USE_WINAPI
    ui->play->setText(tr("Play"));
    if(ui->recordWav->isChecked())
    {
        ui->recordWav->setChecked(false);
        PGE_MusicPlayer::stopWavRecording();
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        ui->frame->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
    }
    #else
    SetWindowTextW(m_buttonPlay, L"Play");
    BOOL checked = IsDlgButtonChecked(m_hWnd, CMD_RecordWave);
    if (checked)
    {
        CheckDlgButton(m_hWnd, CMD_RecordWave, BST_UNCHECKED);
        PGE_MusicPlayer::stopWavRecording();
        EnableWindow(m_buttonOpen, TRUE);
        EnableWindow(m_buttonPlay, TRUE);
        EnableWindow(m_gme.m_trackNum, TRUE);
        EnableWindow(m_gme.m_trackNumUpDown, TRUE);
        EnableWindow(m_midi.m_midiDevice, TRUE);
        EnableWindow(m_adlmidi.m_bankID, TRUE);
        EnableWindow(m_adlmidi.m_tremolo, TRUE);
        EnableWindow(m_adlmidi.m_vibrato, TRUE);
        EnableWindow(m_adlmidi.m_scalableMod, TRUE);
        EnableWindow(m_adlmidi.m_adlibDrums, TRUE);
    }
    #endif
}

void MainWindow::on_play_clicked()
{
    if(Mix_PlayingMusic())
    {
        if(Mix_PausedMusic())
        {
            Mix_ResumeMusic();
            #ifndef MUSPLAY_USE_WINAPI
            ui->play->setText(tr("Pause"));
            #else
            SetWindowTextW(m_buttonPlay, L"Pause");
            #endif
            return;
        }
        else
        {
            Mix_PauseMusic();
            #ifndef MUSPLAY_USE_WINAPI
            ui->play->setText(tr("Resume"));
            #else
            SetWindowTextW(m_buttonPlay, L"Resume");
            #endif
            return;
        }
    }

    #ifndef MUSPLAY_USE_WINAPI
    ui->play->setText(tr("Play"));
    m_prevTrackID = ui->trackID->value();
    #else
    char trackIDtext[1024];
    memset(trackIDtext, 0, 1024);
    GetWindowTextA(m_gme.m_trackNum, trackIDtext, 1024);
    int trackID = /*(int)SendMessageW( m_gme.m_trackNumUpDown, UDM_GETPOS, 0, 0);//*/ atoi(trackIDtext);
    m_prevTrackID = trackID;
    SetWindowTextW(m_buttonPlay, L"Play");
    if(currentMusic.empty())
        return;
    #endif
    if(PGE_MusicPlayer::MUS_openFile(currentMusic
                                     #ifndef MUSPLAY_USE_WINAPI
                                     +"|"+ui->trackID->text()
                                     #else
                                     +"|"+trackIDtext //std::to_string(trackID)
                                     #endif
                                     ) )
    {
        #ifndef MUSPLAY_USE_WINAPI
        PGE_MusicPlayer::MUS_changeVolume(ui->volume->value());
        #endif
        PGE_MusicPlayer::MUS_playMusic();
        #ifndef MUSPLAY_USE_WINAPI
        ui->play->setText(tr("Pause"));
        #else
        SetWindowTextW(m_buttonPlay, L"Pause");
        #endif
    }
    #ifndef MUSPLAY_USE_WINAPI
    ui->musTitle->setText(PGE_MusicPlayer::MUS_getMusTitle());
    ui->musArtist->setText(PGE_MusicPlayer::MUS_getMusArtist());
    ui->musAlbum->setText(PGE_MusicPlayer::MUS_getMusAlbum());
    ui->musCopyright->setText(PGE_MusicPlayer::MUS_getMusCopy());

    ui->gme_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    ui->midi_setup->setVisible(false);
    ui->frame->setVisible(false);
    this->window()->resize(100, 100);
    ui->frame->setVisible(true);
    ui->smallInfo->setText(PGE_MusicPlayer::musicType());

    ui->gridLayout->update();

    switch(PGE_MusicPlayer::type)
    {
        case MUS_MID:
            ui->adlmidi_xtra->setVisible(ui->mididevice->currentIndex()==0);
            ui->midi_setup->setVisible(true);
            ui->frame->setVisible(true);
            break;
        case MUS_SPC:
            ui->gme_setup->setVisible(true);
            ui->frame->setVisible(true);
            break;
        default:
            break;
    }
    this->window()->updateGeometry();
    this->window()->resize(100,100);
    #else
    ShowWindow(m_groupGME, SW_HIDE);
    ShowWindow(m_groupMIDI, SW_HIDE);
    ShowWindow(m_groupADLMIDI, SW_HIDE);
    switch(PGE_MusicPlayer::type)
    {
        case MUS_MID:
            ShowWindow(m_groupMIDI,     SW_SHOW);
            ShowWindow(m_groupADLMIDI,  SW_SHOW);
            m_height = 350;
            break;
        case MUS_SPC:
            ShowWindow(m_groupGME,  SW_SHOW);
            m_height = 220;
            break;
        default:
            m_height = 170;
            break;
    }
    SetWindowPos(m_hWnd, HWND_TOP, 0, 0, 350, m_height, SWP_NOMOVE|SWP_NOZORDER);

    SetWindowTextW(m_labelTitle,        Str2Wstr(PGE_MusicPlayer::MUS_getMusTitle()).c_str());
    SetWindowTextW(m_labelArtist,       Str2Wstr(PGE_MusicPlayer::MUS_getMusArtist()).c_str());
    SetWindowTextW(m_labelAlboom,       Str2Wstr(PGE_MusicPlayer::MUS_getMusAlbum()).c_str());
    SetWindowTextW(m_labelCopyright,    Str2Wstr(PGE_MusicPlayer::MUS_getMusCopy()).c_str());
    SetWindowTextA(m_formatInfo,        PGE_MusicPlayer::musicTypeC());
    #endif
}

void MainWindow::on_mididevice_currentIndexChanged(int index)
{
    #ifndef MUSPLAY_USE_WINAPI
    ui->midi_setup->setVisible(false);
    ui->adlmidi_xtra->setVisible(false);
    this->window()->resize(100,100);
    ui->midi_setup->setVisible(true);
    #endif
    switch(index)
    {
        case 0: MIX_SetMidiDevice(MIDI_ADLMIDI);
        #ifndef MUSPLAY_USE_WINAPI
        ui->adlmidi_xtra->setVisible(true);
        #endif
        break;
        case 1: MIX_SetMidiDevice(MIDI_Timidity);
        #ifndef MUSPLAY_USE_WINAPI
        ui->adlmidi_xtra->setVisible(false);
        #endif
        break;
        case 2: MIX_SetMidiDevice(MIDI_Native);
        #ifndef MUSPLAY_USE_WINAPI
        ui->adlmidi_xtra->setVisible(false);
        #endif
        break;
        default: MIX_SetMidiDevice(MIDI_ADLMIDI);
        #ifndef MUSPLAY_USE_WINAPI
        ui->adlmidi_xtra->setVisible(true);
        #endif
        break;
    }
    #ifndef MUSPLAY_USE_WINAPI
    this->window()->resize(100,100);
    #endif
    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type==MUS_MID)
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }

}

void MainWindow::on_fmbank_currentIndexChanged(int index)
{
    MIX_ADLMIDI_setBankID(index);
    if(Mix_PlayingMusic())
    {
        if(PGE_MusicPlayer::type==MUS_MID)
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
    }
}

void MainWindow::on_tremolo_clicked(bool checked)
{
    MIX_ADLMIDI_setTremolo((int)checked);
}

void MainWindow::on_vibrato_clicked(bool checked)
{
    MIX_ADLMIDI_setVibrato((int)checked);
}

void MainWindow::on_modulation_clicked(bool checked)
{
    MIX_ADLMIDI_setScaleMod((int)checked);
}

void MainWindow::on_adlibMode_clicked(bool checked)
{
    MIX_ADLMIDI_setAdLibMode((int)checked);
}

void MainWindow::on_trackID_editingFinished()
{
    if(Mix_PlayingMusic())
    {
        #ifndef MUSPLAY_USE_WINAPI
        if( (PGE_MusicPlayer::type == MUS_SPC) && (m_prevTrackID != ui->trackID->value()) )
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
        #else
        char buff[1024];
        memset(buff, 0, 1024);
        GetWindowTextA(m_gme.m_trackNum, buff, 1024);
        int trackID = atoi(buff);
        //int trackID = (int)SendMessageW( m_gme.m_trackNumUpDown, UDM_GETPOS, 0, 0);
        if( (PGE_MusicPlayer::type == MUS_SPC) && (m_prevTrackID != trackID) )
        {
            Mix_HaltMusic();
            on_play_clicked();
        }
        #endif
    }
}

void MainWindow::on_recordWav_clicked(bool checked)
{
    if(checked)
    {
        PGE_MusicPlayer::MUS_stopMusic();
        #ifndef MUSPLAY_USE_WINAPI
        ui->play->setText(tr("Play"));
        #else
        SetWindowTextW(m_buttonPlay, L"Play");
        #endif
        #ifndef MUSPLAY_USE_WINAPI
        QFileInfo twav(currentMusic);
        #endif
        PGE_MusicPlayer::stopWavRecording();
        #ifndef MUSPLAY_USE_WINAPI
        PGE_MusicPlayer::startWavRecording(twav.absoluteDir().absolutePath()+"/"+twav.baseName()+".wav");
        #else
        PGE_MusicPlayer::startWavRecording(currentMusic + ".wav");
        #endif
        on_play_clicked();
        #ifndef MUSPLAY_USE_WINAPI
        ui->open->setEnabled(false);
        ui->play->setEnabled(false);
        ui->frame->setEnabled(false);
        m_blinker.start(500);
        #else
        EnableWindow(m_buttonOpen, FALSE);
        EnableWindow(m_buttonPlay, FALSE);
        EnableWindow(m_gme.m_trackNum, FALSE);
        EnableWindow(m_gme.m_trackNumUpDown, FALSE);
        EnableWindow(m_midi.m_midiDevice, FALSE);
        EnableWindow(m_adlmidi.m_bankID, FALSE);
        EnableWindow(m_adlmidi.m_tremolo, FALSE);
        EnableWindow(m_adlmidi.m_vibrato, FALSE);
        EnableWindow(m_adlmidi.m_scalableMod, FALSE);
        EnableWindow(m_adlmidi.m_adlibDrums, FALSE);
        #endif
    } else {
        on_stop_clicked();
        PGE_MusicPlayer::stopWavRecording();
        #ifndef MUSPLAY_USE_WINAPI
        ui->open->setEnabled(true);
        ui->play->setEnabled(true);
        ui->frame->setEnabled(true);
        m_blinker.stop();
        ui->recordWav->setStyleSheet("");
        #else
        EnableWindow(m_buttonOpen, TRUE);
        EnableWindow(m_buttonPlay, TRUE);
        EnableWindow(m_gme.m_trackNum, TRUE);
        EnableWindow(m_gme.m_trackNumUpDown, TRUE);
        EnableWindow(m_midi.m_midiDevice, TRUE);
        EnableWindow(m_adlmidi.m_bankID, TRUE);
        EnableWindow(m_adlmidi.m_tremolo, TRUE);
        EnableWindow(m_adlmidi.m_vibrato, TRUE);
        EnableWindow(m_adlmidi.m_scalableMod, TRUE);
        EnableWindow(m_adlmidi.m_adlibDrums, TRUE);
        #endif
    }
}

void MainWindow::_blink_red()
{
    m_blink_state=!m_blink_state;
    #ifndef MUSPLAY_USE_WINAPI
    if(m_blink_state)
        ui->recordWav->setStyleSheet("background-color : red; color : black;");
    else
        ui->recordWav->setStyleSheet("background-color : black; color : red;");
    #endif
}

void MainWindow::on_resetDefaultADLMIDI_clicked()
{
    #ifndef MUSPLAY_USE_WINAPI
    ui->fmbank->setCurrentIndex( 58 );
    ui->tremolo->setChecked(true);
    ui->vibrato->setChecked(true);
    ui->adlibMode->setChecked(false);
    ui->modulation->setChecked(false);

    MIX_ADLMIDI_setTremolo((int)ui->tremolo->isChecked());
    MIX_ADLMIDI_setVibrato((int)ui->vibrato->isChecked());
    MIX_ADLMIDI_setAdLibMode((int)ui->adlibMode->isChecked());
    MIX_ADLMIDI_setScaleMod((int)ui->modulation->isChecked());

    on_fmbank_currentIndexChanged( ui->fmbank->currentIndex() );
    #endif
}

void MainWindow::on_volume_valueChanged(int value)
{
    PGE_MusicPlayer::MUS_changeVolume(value);
}

#ifndef MUSPLAY_USE_WINAPI
void MainWindow::contextMenu(const QPoint &pos)
{
    QMenu x;
    QAction* open        = x.addAction("Open");
    QAction* playpause   = x.addAction("Play/Pause");
    QAction* stop        = x.addAction("Stop");
                           x.addSeparator();
    QAction* reverb       = x.addAction("Reverb");
    reverb->setCheckable(true);
    reverb->setChecked(PGE_MusicPlayer::reverbEnabled);
    QAction* assoc_files = x.addAction("Associate files");
                           x.addSeparator();
    QMenu  * about       = x.addMenu("About");
    QAction* version     = about->addAction("SDL Mixer X Music Player v." _FILE_VERSION );
                           version->setEnabled(false);
    QAction* license     = about->addAction("Licensed under GNU GPLv3 license");
                           about->addSeparator();
    QAction* source      = about->addAction("Get source code");

    QAction *ret = x.exec(this->mapToGlobal(pos));
    if(open == ret)
    {
        on_open_clicked();
    }
    else if(playpause == ret)
    {
        on_play_clicked();
    }
    else if(stop == ret)
    {
        on_stop_clicked();
    }
    else if(reverb == ret)
    {
        PGE_MusicPlayer::reverbEnabled = reverb->isChecked();
        if(PGE_MusicPlayer::reverbEnabled)
            Mix_RegisterEffect(MIX_CHANNEL_POST, reverbEffect, reverbEffectDone, NULL);
        else
            Mix_UnregisterEffect(MIX_CHANNEL_POST, reverbEffect);
    }
    else if(assoc_files == ret)
    {
        AssocFiles af(this);
        af.setWindowModality(Qt::WindowModal);
        af.exec();
    }
    else if(ret == license)
    {
        QDesktopServices::openUrl(QUrl("http://www.gnu.org/licenses/gpl"));
    }
    else if(ret == source)
    {
        QDesktopServices::openUrl(QUrl("https://github.com/Wohlhabend-Networks/PGE-Project"));
    }
}
#endif
