#pragma once
#ifndef IMAGE_CALIBRATOR_H
#define IMAGE_CALIBRATOR_H

#include <QDialog>
#include <QPixmap>
#include <QImage>
#include <QSet>
#include <QList>
#include <QMenu>
#include <QFileSystemWatcher>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include "main/calibration.h"

namespace Ui
{
    class ImageCalibrator;
}

class CalibrationMain;
class Matrix;
class QStatusBar;

inline uint qHash(const QColor &key, uint seed)
{
    return qHash(key.rgba(), seed);
}

class ImageCalibrator : public QDialog
{
    Q_OBJECT

    QVector<QVector<CalibrationFrame > > m_imgOffsets;
    Calibration *m_conf = nullptr;
    CalibrationFrame m_setupBuffer;
    CalibrationMain *m_mw = nullptr;
    QStatusBar *m_status = nullptr;

    QSet<QColor> m_recent_colours;
    QSet<QColor> m_frame_colours;

    struct FrameHistory
    {
        int state = -1;
        struct Entry
        {
            QPixmap img;
            CalibrationFrame setup;
        };

        QVector<Entry> history;

        bool canUndo()
        {
            return state > 0;
        }

        bool canRedo()
        {
            return state < history.size() - 1;
        }

        QPixmap undo(CalibrationFrame &setupOut)
        {
            if(!canUndo())
                return QPixmap();

            auto &h = history[--state];
            setupOut = h.setup;
            return h.img;
        }

        QPixmap redo(CalibrationFrame &setupOut)
        {
            if(!canRedo())
                return QPixmap();

            auto &h = history[++state];
            setupOut = h.setup;
            return h.img;
        }

        void addHistory(const QPixmap &img, const CalibrationFrame &setup)
        {
            Entry e;

            if(!history.isEmpty() && canRedo())
                history.resize(state + 1);

            e.img = img;
            e.setup = setup;

            history.push_back(e);
            state++;
        }
    };

    QVector<QVector<FrameHistory > > m_history;

    QMenu m_toolActions;

    Matrix *m_matrix = nullptr;
public:
    explicit ImageCalibrator(Calibration *conf, QWidget *parent = nullptr);
    ~ImageCalibrator();

    bool init(QString imgPath);
    void unInit();
    QString m_targetPath;

    void setFrame(int x, int y);
    void setPreviewOffset(const QPoint &off);
    void setPreviewZoom(double zoom);
    void showSpriteMap();

    bool hitboxModified() const;
    bool hitboxNeedSave() const;

protected:
    void closeEvent(QCloseEvent *e);
    void focusInEvent(QFocusEvent *event);
    void keyPressEvent(QKeyEvent *e);
    void keyReleaseEvent(QKeyEvent *e);

private slots:
    void on_FrameX_valueChanged(int arg1);
    void on_FrameY_valueChanged(int arg1);
    void frameSelected(int x, int y);
    void referenceSelected(int x, int y);

    void on_OffsetX_valueChanged(int arg1);
    void on_OffsetY_valueChanged(int arg1);
    void updateOffset(int x, int y, int offsetX, int offsetY);
    void updateOffsets(int x, int y, int deltaX, int deltaY);
    void updateAllOffsets(int deltaX, int deltaY);

    void on_dublPixDrawMode_clicked(bool checked);

    void toolMirrorSMBXFrames();
    void toolMirrorSMBXAction(int dir);

    void toolEraseUnused();
    void toolFlipFrameH();
    void toolFlipFrameV();

    void on_Matrix_clicked(bool checked);
    void on_Reset_clicked();
    void makeBackup();
    void on_WritePNG_GIF_clicked();
    void on_WritePNG_clicked();
    void on_WriteGIF_clicked();
    void updateControls();
    void updateScene();
    void saveCalibrates();
    void loadCalibrates();
    QPixmap generateTarget();
    QPixmap getCurrentFrame();
    QPixmap getFrame(int x, int y);
    QPixmap getFrame(int x, int y, int oX, int oY, int cW, int cH);

    void on_CropW_valueChanged(int arg1);
    void on_CropH_valueChanged(int arg1);

    void on_refOpacity_sliderMoved(int position);
    void on_refClear_clicked();

    void on_openFrameInEditor_clicked();

    void toolChanged(bool);
    void pickColour();
    void refreshColours();


private:
    int m_frmX;
    int m_frmY;

    QPixmap m_sprite;
    QPixmap m_spriteOrig;
    bool    m_spriteModified = false;

    QString m_pngPath;
    QString m_gifPath;
    QString m_gifPathM;
    QString m_backupPath;
    QString m_iniPath;

    bool m_hitboxModified = false;
    bool m_hitboxNeedSave = false;

    Ui::ImageCalibrator *ui;
    bool m_lockUI;

    QFileSystemWatcher m_watchingFrame;
    QString            m_pendingFileToUpdate;
    void tempFrameUpdated(const QString &path);
    void tempFrameUpdatedProceed();
    void frameEdited();
    void updateCurrentFrame(const QPixmap &f);
    void updateFrame(int x, int y, const QPixmap &f);
    void historyUndo(bool);
    void historyRedo(bool);
    int m_tempFileX = 0;
    int m_tempFileY = 0;
};

#endif // IMAGE_CALIBRATOR_H
