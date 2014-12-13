#ifndef IMAGE_CALIBRATOR_H
#define IMAGE_CALIBRATOR_H

#include <QDialog>
#include <QPixmap>
#include <QImage>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include "../main/globals.h"

namespace Ui {
class ImageCalibrator;
}

class ImageCalibrator : public QDialog
{
    Q_OBJECT

public:
    explicit ImageCalibrator(QWidget *parent = 0);
    ~ImageCalibrator();

    bool init(QString imgPath);
    QGraphicsScene *Scene;
    QString targetPath;
    QString pngPath;
    QString gifPath;
    QString gifPathM;
    QString iniPath;

private slots:
    void on_FrameX_valueChanged(int arg1);
    void on_FrameY_valueChanged(int arg1);
    void on_OffsetX_valueChanged(int arg1);
    void on_OffsetY_valueChanged(int arg1);

    void on_Matrix_clicked();
    void on_Reset_clicked();
    void on_WritePNG_GIF_clicked();
    void on_WritePNG_clicked();
    void on_WriteGIF_clicked();
    void updateControls();
    void updateScene();
    void saveCalibrates();
    void loadCalibrates();
    QPixmap generateTarget();
    QPixmap getFrame(int x, int y, int oX, int oY, int cW, int cH);

    void on_CropW_valueChanged(int arg1);

    void on_CropH_valueChanged(int arg1);

private:
    Ui::ImageCalibrator *ui;

    int frmX;
    int frmY;

    QPixmap sprite;
    QGraphicsPixmapItem *imgFrame;
    QGraphicsRectItem *phsFrame;
    QGraphicsRectItem *physics;

};

#endif // IMAGE_CALIBRATOR_H
