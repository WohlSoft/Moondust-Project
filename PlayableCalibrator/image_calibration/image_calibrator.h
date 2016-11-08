#pragma once
#ifndef IMAGE_CALIBRATOR_H
#define IMAGE_CALIBRATOR_H

#include <QDialog>
#include <QPixmap>
#include <QImage>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include "../main/globals.h"

namespace Ui
{
    class ImageCalibrator;
}

class ImageCalibrator : public QDialog
{
        Q_OBJECT

    public:
        explicit ImageCalibrator(QWidget *parent = 0);
        ~ImageCalibrator();

        bool init(QString imgPath);
        QGraphicsScene *m_scene;
        QString m_targetPath;

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
        int m_frmX;
        int m_frmY;

        QPixmap m_sprite;
        QGraphicsPixmapItem *m_imgFrame;
        QGraphicsRectItem *m_phsFrame;
        QGraphicsRectItem *m_physics;

        QString m_pngPath;
        QString m_gifPath;
        QString m_gifPathM;
        QString m_iniPath;

        Ui::ImageCalibrator *ui;
        bool m_lockUI;
};

#endif // IMAGE_CALIBRATOR_H
