#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QUrl>
#include <QDragEnterEvent>
#include <QMimeData>

namespace Ui {
class MainWindow;
}

/*!
 * \brief Class of main application window
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //! Full path to currently opened music
    QString currentMusic;

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
     * \brief Open music file
     * \param musPath full or relative path to music file to open
     */
    void openMusicByArg(QString musPath);

private slots:
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

    /*!
     * \brief Track-ID editing finished event
     */
    void on_trackID_editingFinished();

private:
    //! UI form class pointer
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
