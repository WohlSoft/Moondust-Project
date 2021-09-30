/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#ifndef MATRIX_H
#define MATRIX_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDialog>
#include <QSharedPointer>
#include "../calibration_main.h"
#include "main/calibration.h"

namespace Ui {
class Matrix;
}

class QCheckBox;
class QPushButton;
class QGridLayout;

class Matrix : public QDialog
{
    Q_OBJECT

    QCheckBox *m_enFrame[10][10];
    Calibration *m_conf = nullptr;
    QGraphicsScene *m_matrixScene = nullptr;
    QGridLayout *m_matrixGrid = nullptr;
    QVector<QSharedPointer<QCheckBox>> m_matrixCheckBoxes;
    QVector<QSharedPointer<QPushButton>> m_matrixSelectors;
    QVector<QSharedPointer<QGridLayout>> m_matrixCells;

    void clearGrid();
    void rebuildGrid();

public:
    explicit Matrix(Calibration *conf, QWidget *mw, QWidget *parent = nullptr);
    ~Matrix();
    Calibration::FramesSet m_frameConfig;

    void updateScene(const QPixmap &sprite);

    int m_frameX = 0;
    int m_frameY = 0;

    void setFrame(int x, int y);
    void drawGrid();

signals:
    void frameSelected(int x, int y);

private:
    QGraphicsPixmapItem m_image;
    QPixmap m_scaledImage;
    Ui::Matrix *ui;
};

#endif // MATRIX_H
