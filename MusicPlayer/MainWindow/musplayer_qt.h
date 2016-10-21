#if !defined(MUSPLAYER_QT_H) && !defined(MUSPLAY_USE_WINAPI)
#define MUSPLAYER_QT_H

#include "musplayer_base.h"

#include <QMainWindow>

#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QTimer>

namespace Ui {
class MainWindow;
}

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

private:
    //! Controlls blinking of the wav-recording label
    QTimer m_blinker;
    //! UI form class pointer
    Ui::MainWindow *ui;
};

#endif // MUSPLAYER_QT_H
