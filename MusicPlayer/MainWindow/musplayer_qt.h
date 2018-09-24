#if !defined(MUSPLAYER_QT_H) && !defined(MUSPLAY_USE_WINAPI)
#define MUSPLAYER_QT_H

#include "musplayer_base.h"
//#include "mw_qt/playlist_model.h"

#include <QMainWindow>

#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTimer>

class SeekBar;
class SfxTester;
class SetupMidi;

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

protected:
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

    void moveEvent(QMoveEvent *event);

public slots:
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

private slots:
    void restartMusic();

    void on_open_clicked();
    void on_stop_clicked();
    void on_play_clicked();

    void on_trackID_editingFinished();

    void on_recordWav_clicked(bool checked);

    /*!
     * \brief Changes color of "Recording WAV" label between black and red
     */
    void _blink_red();

    void updatePositionSlider();
    void musicPosition_seeked(double value);

    void on_actionOpen_triggered();
    void on_actionQuit_triggered();
    void on_actionHelpLicense_triggered();
    void on_actionHelpAbout_triggered();
    void on_actionHelpGitHub_triggered();
    void on_actionMidiSetup_triggered();
    void on_actionSfxTesting_triggered();
    void on_actionEnableReverb_triggered(bool checked);
    void on_actionFileAssoc_triggered();

private:
    bool playListMode = false;
    //PlayListModel playList;
    //! Controlls blinking of the wav-recording label
    QTimer m_blinker;
    QTimer m_positionWatcher;
    bool   m_positionWatcherLock = false;
    //! UI form class pointer
    Ui::MainWindow *ui;

    QPoint    m_oldWindowPos;

    SeekBar   *m_seekBar = nullptr;
    SfxTester *m_sfxTester = nullptr;
    SetupMidi *m_setupMidi = nullptr;
};

#endif // MUSPLAYER_QT_H
