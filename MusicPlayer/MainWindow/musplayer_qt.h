#if !defined(MUSPLAYER_QT_H) && !defined(MUSPLAY_USE_WINAPI)
#define MUSPLAYER_QT_H

#include "musplayer_base.h"
//#include "mw_qt/playlist_model.h"

#include <QMainWindow>

#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTimer>

namespace Ui {
class MainWindow;
}

struct Mix_Chunk;

class MusPlayer_Qt : public QMainWindow, public MusPlayerBase
{
    Q_OBJECT
public:
    explicit MusPlayer_Qt(QWidget *parent = 0);
    virtual ~MusPlayer_Qt();

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

    /*!
     * \brief Context menu
     * \param pos Mouse cursor position
     */
    void contextMenu(const QPoint &pos);

    void openMusicByArg(QString musPath);

    //void setPlayListMode(bool plMode);

    //void playList_pushCurrent(bool x = false);
    //void playList_popCurrent(bool x = false);
    //void playListNext();

    void switchMidiDevice(int index);

private slots:
    void on_open_clicked();
    void on_stop_clicked();
    void on_play_clicked();
    void on_mididevice_currentIndexChanged(int index);
    void on_trackID_editingFinished();
    void on_recordWav_clicked(bool checked);
    void on_resetDefaultADLMIDI_clicked();
    /*!
     * \brief Changes color of "Recording WAV" label between black and red
     */
    void _blink_red();

    void on_sfx_open_clicked();
    void on_sfx_play_clicked();
    void on_sfx_fadeIn_clicked();
    void on_sfx_stop_clicked();
    void on_sfx_fadeout_clicked();

    void updatePositionSlider();
    void on_musicPosition_valueChanged(int value);

    void on_opn_bank_browse_clicked();
    void on_opn_bank_editingFinished();

    void on_adl_bank_browse_clicked();
    void on_adl_bank_editingFinished();

private:
    bool playListMode = false;
    //PlayListModel playList;
    //! Controlls blinking of the wav-recording label
    QTimer m_blinker;
    QTimer m_positionWatcher;
    bool   m_positionWatcherLock = false;
    //! UI form class pointer
    Ui::MainWindow *ui;

    Mix_Chunk *m_testSfx = nullptr;
    QString    m_testSfxDir;
};

#endif // MUSPLAYER_QT_H
