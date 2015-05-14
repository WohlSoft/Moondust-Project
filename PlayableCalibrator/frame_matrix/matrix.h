/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef MATRIX_H
#define MATRIX_H

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QDialog>
#include "../calibrationmain.h"

namespace Ui {
class Matrix;
}

class Matrix : public QDialog
{
    Q_OBJECT
    
public:
    explicit Matrix(QWidget *parent = 0);
    ~Matrix();
    QGraphicsScene *MatrixS;
    QList<QVector<frameOpts > > FrameConfig;
    int frameX;
    int frameY;
    void setFrame(int x, int y);
    void drawGrid();

private slots:
    void on_EnFrame_0_0_clicked(bool checked);
    void on_EnFrame_0_1_clicked(bool checked);
    void on_EnFrame_0_2_clicked(bool checked);
    void on_EnFrame_0_3_clicked(bool checked);
    void on_EnFrame_0_4_clicked(bool checked);
    void on_EnFrame_0_5_clicked(bool checked);
    void on_EnFrame_0_6_clicked(bool checked);
    void on_EnFrame_0_7_clicked(bool checked);
    void on_EnFrame_0_8_clicked(bool checked);
    void on_EnFrame_0_9_clicked(bool checked);

    //Y = 1
    void on_EnFrame_1_0_clicked(bool checked);
    void on_EnFrame_1_1_clicked(bool checked);
    void on_EnFrame_1_2_clicked(bool checked);
    void on_EnFrame_1_3_clicked(bool checked);
    void on_EnFrame_1_4_clicked(bool checked);
    void on_EnFrame_1_5_clicked(bool checked);
    void on_EnFrame_1_6_clicked(bool checked);
    void on_EnFrame_1_7_clicked(bool checked);
    void on_EnFrame_1_8_clicked(bool checked);
    void on_EnFrame_1_9_clicked(bool checked);

    //Y = 2
    void on_EnFrame_2_0_clicked(bool checked);
    void on_EnFrame_2_1_clicked(bool checked);
    void on_EnFrame_2_2_clicked(bool checked);
    void on_EnFrame_2_3_clicked(bool checked);
    void on_EnFrame_2_4_clicked(bool checked);
    void on_EnFrame_2_5_clicked(bool checked);
    void on_EnFrame_2_6_clicked(bool checked);
    void on_EnFrame_2_7_clicked(bool checked);
    void on_EnFrame_2_8_clicked(bool checked);
    void on_EnFrame_2_9_clicked(bool checked);

    //Y = 3
    void on_EnFrame_3_0_clicked(bool checked);
    void on_EnFrame_3_1_clicked(bool checked);
    void on_EnFrame_3_2_clicked(bool checked);
    void on_EnFrame_3_3_clicked(bool checked);
    void on_EnFrame_3_4_clicked(bool checked);
    void on_EnFrame_3_5_clicked(bool checked);
    void on_EnFrame_3_6_clicked(bool checked);
    void on_EnFrame_3_7_clicked(bool checked);
    void on_EnFrame_3_8_clicked(bool checked);
    void on_EnFrame_3_9_clicked(bool checked);

    //Y = 4
    void on_EnFrame_4_0_clicked(bool checked);
    void on_EnFrame_4_1_clicked(bool checked);
    void on_EnFrame_4_2_clicked(bool checked);
    void on_EnFrame_4_3_clicked(bool checked);
    void on_EnFrame_4_4_clicked(bool checked);
    void on_EnFrame_4_5_clicked(bool checked);
    void on_EnFrame_4_6_clicked(bool checked);
    void on_EnFrame_4_7_clicked(bool checked);
    void on_EnFrame_4_8_clicked(bool checked);
    void on_EnFrame_4_9_clicked(bool checked);

    //Y = 5
    void on_EnFrame_5_0_clicked(bool checked);
    void on_EnFrame_5_1_clicked(bool checked);
    void on_EnFrame_5_2_clicked(bool checked);
    void on_EnFrame_5_3_clicked(bool checked);
    void on_EnFrame_5_4_clicked(bool checked);
    void on_EnFrame_5_5_clicked(bool checked);
    void on_EnFrame_5_6_clicked(bool checked);
    void on_EnFrame_5_7_clicked(bool checked);
    void on_EnFrame_5_8_clicked(bool checked);
    void on_EnFrame_5_9_clicked(bool checked);

    //Y = 6
    void on_EnFrame_6_0_clicked(bool checked);
    void on_EnFrame_6_1_clicked(bool checked);
    void on_EnFrame_6_2_clicked(bool checked);
    void on_EnFrame_6_3_clicked(bool checked);
    void on_EnFrame_6_4_clicked(bool checked);
    void on_EnFrame_6_5_clicked(bool checked);
    void on_EnFrame_6_6_clicked(bool checked);
    void on_EnFrame_6_7_clicked(bool checked);
    void on_EnFrame_6_8_clicked(bool checked);
    void on_EnFrame_6_9_clicked(bool checked);

    //Y = 7
    void on_EnFrame_7_0_clicked(bool checked);
    void on_EnFrame_7_1_clicked(bool checked);
    void on_EnFrame_7_2_clicked(bool checked);
    void on_EnFrame_7_3_clicked(bool checked);
    void on_EnFrame_7_4_clicked(bool checked);
    void on_EnFrame_7_5_clicked(bool checked);
    void on_EnFrame_7_6_clicked(bool checked);
    void on_EnFrame_7_7_clicked(bool checked);
    void on_EnFrame_7_8_clicked(bool checked);
    void on_EnFrame_7_9_clicked(bool checked);

    //Y = 8
    void on_EnFrame_8_0_clicked(bool checked);
    void on_EnFrame_8_1_clicked(bool checked);
    void on_EnFrame_8_2_clicked(bool checked);
    void on_EnFrame_8_3_clicked(bool checked);
    void on_EnFrame_8_4_clicked(bool checked);
    void on_EnFrame_8_5_clicked(bool checked);
    void on_EnFrame_8_6_clicked(bool checked);
    void on_EnFrame_8_7_clicked(bool checked);
    void on_EnFrame_8_8_clicked(bool checked);
    void on_EnFrame_8_9_clicked(bool checked);

    //Y = 9
    void on_EnFrame_9_0_clicked(bool checked);
    void on_EnFrame_9_1_clicked(bool checked);
    void on_EnFrame_9_2_clicked(bool checked);
    void on_EnFrame_9_3_clicked(bool checked);
    void on_EnFrame_9_4_clicked(bool checked);
    void on_EnFrame_9_5_clicked(bool checked);
    void on_EnFrame_9_6_clicked(bool checked);
    void on_EnFrame_9_7_clicked(bool checked);
    void on_EnFrame_9_8_clicked(bool checked);
    void on_EnFrame_9_9_clicked(bool checked);


    // Y = 0
    void on_SetFrame_0_0_clicked();
    void on_SetFrame_0_1_clicked();
    void on_SetFrame_0_2_clicked();
    void on_SetFrame_0_3_clicked();
    void on_SetFrame_0_4_clicked();
    void on_SetFrame_0_5_clicked();
    void on_SetFrame_0_6_clicked();
    void on_SetFrame_0_7_clicked();
    void on_SetFrame_0_8_clicked();
    void on_SetFrame_0_9_clicked();

    // Y = 1
    void on_SetFrame_1_0_clicked();
    void on_SetFrame_1_1_clicked();
    void on_SetFrame_1_2_clicked();
    void on_SetFrame_1_3_clicked();
    void on_SetFrame_1_4_clicked();
    void on_SetFrame_1_5_clicked();
    void on_SetFrame_1_6_clicked();
    void on_SetFrame_1_7_clicked();
    void on_SetFrame_1_8_clicked();
    void on_SetFrame_1_9_clicked();

    // Y = 2
    void on_SetFrame_2_0_clicked();
    void on_SetFrame_2_1_clicked();
    void on_SetFrame_2_2_clicked();
    void on_SetFrame_2_3_clicked();
    void on_SetFrame_2_4_clicked();
    void on_SetFrame_2_5_clicked();
    void on_SetFrame_2_6_clicked();
    void on_SetFrame_2_7_clicked();
    void on_SetFrame_2_8_clicked();
    void on_SetFrame_2_9_clicked();

    // Y = 3
    void on_SetFrame_3_0_clicked();
    void on_SetFrame_3_1_clicked();
    void on_SetFrame_3_2_clicked();
    void on_SetFrame_3_3_clicked();
    void on_SetFrame_3_4_clicked();
    void on_SetFrame_3_5_clicked();
    void on_SetFrame_3_6_clicked();
    void on_SetFrame_3_7_clicked();
    void on_SetFrame_3_8_clicked();
    void on_SetFrame_3_9_clicked();

    // Y = 4
    void on_SetFrame_4_0_clicked();
    void on_SetFrame_4_1_clicked();
    void on_SetFrame_4_2_clicked();
    void on_SetFrame_4_3_clicked();
    void on_SetFrame_4_4_clicked();
    void on_SetFrame_4_5_clicked();
    void on_SetFrame_4_6_clicked();
    void on_SetFrame_4_7_clicked();
    void on_SetFrame_4_8_clicked();
    void on_SetFrame_4_9_clicked();

    // Y = 5
    void on_SetFrame_5_0_clicked();
    void on_SetFrame_5_1_clicked();
    void on_SetFrame_5_2_clicked();
    void on_SetFrame_5_3_clicked();
    void on_SetFrame_5_4_clicked();
    void on_SetFrame_5_5_clicked();
    void on_SetFrame_5_6_clicked();
    void on_SetFrame_5_7_clicked();
    void on_SetFrame_5_8_clicked();
    void on_SetFrame_5_9_clicked();

    // Y = 6
    void on_SetFrame_6_0_clicked();
    void on_SetFrame_6_1_clicked();
    void on_SetFrame_6_2_clicked();
    void on_SetFrame_6_3_clicked();
    void on_SetFrame_6_4_clicked();
    void on_SetFrame_6_5_clicked();
    void on_SetFrame_6_6_clicked();
    void on_SetFrame_6_7_clicked();
    void on_SetFrame_6_8_clicked();
    void on_SetFrame_6_9_clicked();

    // Y = 7
    void on_SetFrame_7_0_clicked();
    void on_SetFrame_7_1_clicked();
    void on_SetFrame_7_2_clicked();
    void on_SetFrame_7_3_clicked();
    void on_SetFrame_7_4_clicked();
    void on_SetFrame_7_5_clicked();
    void on_SetFrame_7_6_clicked();
    void on_SetFrame_7_7_clicked();
    void on_SetFrame_7_8_clicked();
    void on_SetFrame_7_9_clicked();

    // Y = 8
    void on_SetFrame_8_0_clicked();
    void on_SetFrame_8_1_clicked();
    void on_SetFrame_8_2_clicked();
    void on_SetFrame_8_3_clicked();
    void on_SetFrame_8_4_clicked();
    void on_SetFrame_8_5_clicked();
    void on_SetFrame_8_6_clicked();
    void on_SetFrame_8_7_clicked();
    void on_SetFrame_8_8_clicked();
    void on_SetFrame_8_9_clicked();

    // Y = 9
    void on_SetFrame_9_0_clicked();
    void on_SetFrame_9_1_clicked();
    void on_SetFrame_9_2_clicked();
    void on_SetFrame_9_3_clicked();
    void on_SetFrame_9_4_clicked();
    void on_SetFrame_9_5_clicked();
    void on_SetFrame_9_6_clicked();
    void on_SetFrame_9_7_clicked();
    void on_SetFrame_9_8_clicked();
    void on_SetFrame_9_9_clicked();


private:
    QGraphicsPixmapItem image;
    QPixmap scaledImage;
    Ui::Matrix *ui;
};

#endif // MATRIX_H
