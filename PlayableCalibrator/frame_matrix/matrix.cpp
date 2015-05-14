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

#include "matrix.h"
#include <ui_matrix.h>
#include "../main/globals.h"

#include "../main/mw.h"

#include <QPainter>


Matrix::Matrix(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Matrix)
{
    ui->setupUi(this);
    MatrixS = new QGraphicsScene;
    ui->SpriteMatrix->setScene(MatrixS);

    this->updateGeometry();
    //qApp->processEvents();
    this->show();

    scaledImage = QPixmap(MW::sprite()).scaled(ui->SpriteMatrix->viewport()->width(),
                                               ui->SpriteMatrix->viewport()->height(),
                                               Qt::KeepAspectRatio);

    drawGrid();
    image.setPixmap(scaledImage);
    image.setPos(0.0, 0.0);
    MatrixS->addItem(&image);

    FrameConfig = framesX;

    //Set data to matrix:
    // Set Y = 0
    ui->EnFrame_0_0->setChecked(framesX[0][0].used);
    ui->EnFrame_0_1->setChecked(framesX[1][0].used);
    ui->EnFrame_0_2->setChecked(framesX[2][0].used);
    ui->EnFrame_0_3->setChecked(framesX[3][0].used);
    ui->EnFrame_0_4->setChecked(framesX[4][0].used);
    ui->EnFrame_0_5->setChecked(framesX[5][0].used);
    ui->EnFrame_0_6->setChecked(framesX[6][0].used);
    ui->EnFrame_0_7->setChecked(framesX[7][0].used);
    ui->EnFrame_0_8->setChecked(framesX[8][0].used);
    ui->EnFrame_0_9->setChecked(framesX[9][0].used);

    // Set Y = 1
    ui->EnFrame_1_0->setChecked(framesX[0][1].used);
    ui->EnFrame_1_1->setChecked(framesX[1][1].used);
    ui->EnFrame_1_2->setChecked(framesX[2][1].used);
    ui->EnFrame_1_3->setChecked(framesX[3][1].used);
    ui->EnFrame_1_4->setChecked(framesX[4][1].used);
    ui->EnFrame_1_5->setChecked(framesX[5][1].used);
    ui->EnFrame_1_6->setChecked(framesX[6][1].used);
    ui->EnFrame_1_7->setChecked(framesX[7][1].used);
    ui->EnFrame_1_8->setChecked(framesX[8][1].used);
    ui->EnFrame_1_9->setChecked(framesX[9][1].used);

    // Set Y = 2
    ui->EnFrame_2_0->setChecked(framesX[0][2].used);
    ui->EnFrame_2_1->setChecked(framesX[1][2].used);
    ui->EnFrame_2_2->setChecked(framesX[2][2].used);
    ui->EnFrame_2_3->setChecked(framesX[3][2].used);
    ui->EnFrame_2_4->setChecked(framesX[4][2].used);
    ui->EnFrame_2_5->setChecked(framesX[5][2].used);
    ui->EnFrame_2_6->setChecked(framesX[6][2].used);
    ui->EnFrame_2_7->setChecked(framesX[7][2].used);
    ui->EnFrame_2_8->setChecked(framesX[8][2].used);
    ui->EnFrame_2_9->setChecked(framesX[9][2].used);

    // Set Y = 3
    ui->EnFrame_3_0->setChecked(framesX[0][3].used);
    ui->EnFrame_3_1->setChecked(framesX[1][3].used);
    ui->EnFrame_3_2->setChecked(framesX[2][3].used);
    ui->EnFrame_3_3->setChecked(framesX[3][3].used);
    ui->EnFrame_3_4->setChecked(framesX[4][3].used);
    ui->EnFrame_3_5->setChecked(framesX[5][3].used);
    ui->EnFrame_3_6->setChecked(framesX[6][3].used);
    ui->EnFrame_3_7->setChecked(framesX[7][3].used);
    ui->EnFrame_3_8->setChecked(framesX[8][3].used);
    ui->EnFrame_3_9->setChecked(framesX[9][3].used);


    // Set Y = 4
    ui->EnFrame_4_0->setChecked(framesX[0][4].used);
    ui->EnFrame_4_1->setChecked(framesX[1][4].used);
    ui->EnFrame_4_2->setChecked(framesX[2][4].used);
    ui->EnFrame_4_3->setChecked(framesX[3][4].used);
    ui->EnFrame_4_4->setChecked(framesX[4][4].used);
    ui->EnFrame_4_5->setChecked(framesX[5][4].used);
    ui->EnFrame_4_6->setChecked(framesX[6][4].used);
    ui->EnFrame_4_7->setChecked(framesX[7][4].used);
    ui->EnFrame_4_8->setChecked(framesX[8][4].used);
    ui->EnFrame_4_9->setChecked(framesX[9][4].used);

    // Set Y = 5
    ui->EnFrame_5_0->setChecked(framesX[0][5].used);
    ui->EnFrame_5_1->setChecked(framesX[1][5].used);
    ui->EnFrame_5_2->setChecked(framesX[2][5].used);
    ui->EnFrame_5_3->setChecked(framesX[3][5].used);
    ui->EnFrame_5_4->setChecked(framesX[4][5].used);
    ui->EnFrame_5_5->setChecked(framesX[5][5].used);
    ui->EnFrame_5_6->setChecked(framesX[6][5].used);
    ui->EnFrame_5_7->setChecked(framesX[7][5].used);
    ui->EnFrame_5_8->setChecked(framesX[8][5].used);
    ui->EnFrame_5_9->setChecked(framesX[9][5].used);

    // Set Y = 6
    ui->EnFrame_6_0->setChecked(framesX[0][6].used);
    ui->EnFrame_6_1->setChecked(framesX[1][6].used);
    ui->EnFrame_6_2->setChecked(framesX[2][6].used);
    ui->EnFrame_6_3->setChecked(framesX[3][6].used);
    ui->EnFrame_6_4->setChecked(framesX[4][6].used);
    ui->EnFrame_6_5->setChecked(framesX[5][6].used);
    ui->EnFrame_6_6->setChecked(framesX[6][6].used);
    ui->EnFrame_6_7->setChecked(framesX[7][6].used);
    ui->EnFrame_6_8->setChecked(framesX[8][6].used);
    ui->EnFrame_6_9->setChecked(framesX[9][6].used);

    // Set Y = 7
    ui->EnFrame_7_0->setChecked(framesX[0][7].used);
    ui->EnFrame_7_1->setChecked(framesX[1][7].used);
    ui->EnFrame_7_2->setChecked(framesX[2][7].used);
    ui->EnFrame_7_3->setChecked(framesX[3][7].used);
    ui->EnFrame_7_4->setChecked(framesX[4][7].used);
    ui->EnFrame_7_5->setChecked(framesX[5][7].used);
    ui->EnFrame_7_6->setChecked(framesX[6][7].used);
    ui->EnFrame_7_7->setChecked(framesX[7][7].used);
    ui->EnFrame_7_8->setChecked(framesX[8][7].used);
    ui->EnFrame_7_9->setChecked(framesX[9][7].used);

    // Set Y = 8
    ui->EnFrame_8_0->setChecked(framesX[0][8].used);
    ui->EnFrame_8_1->setChecked(framesX[1][8].used);
    ui->EnFrame_8_2->setChecked(framesX[2][8].used);
    ui->EnFrame_8_3->setChecked(framesX[3][8].used);
    ui->EnFrame_8_4->setChecked(framesX[4][8].used);
    ui->EnFrame_8_5->setChecked(framesX[5][8].used);
    ui->EnFrame_8_6->setChecked(framesX[6][8].used);
    ui->EnFrame_8_7->setChecked(framesX[7][8].used);
    ui->EnFrame_8_8->setChecked(framesX[8][8].used);
    ui->EnFrame_8_9->setChecked(framesX[9][8].used);

    // Set Y = 9
    ui->EnFrame_9_0->setChecked(framesX[0][9].used);
    ui->EnFrame_9_1->setChecked(framesX[1][9].used);
    ui->EnFrame_9_2->setChecked(framesX[2][9].used);
    ui->EnFrame_9_3->setChecked(framesX[3][9].used);
    ui->EnFrame_9_4->setChecked(framesX[4][9].used);
    ui->EnFrame_9_5->setChecked(framesX[5][9].used);
    ui->EnFrame_9_6->setChecked(framesX[6][9].used);
    ui->EnFrame_9_7->setChecked(framesX[7][9].used);
    ui->EnFrame_9_8->setChecked(framesX[8][9].used);
    ui->EnFrame_9_9->setChecked(framesX[9][9].used);
}

Matrix::~Matrix()
{
    delete ui;
}

void Matrix::setFrame(int x, int y)
{
    frameX = x;
    frameY = y;
    QPainter painter(&scaledImage);
        QColor color;
        color.setRed(255);
        color.setBlue(0);
        color.setGreen(0);
        color.setAlpha(128);
        painter.setBrush(QBrush(color));
        painter.drawEllipse(QPoint(
                          (scaledImage.width()/10)*x+(scaledImage.width()/10)/2,
                          (scaledImage.height()/10)*y+(scaledImage.height()/10)/2
                                ), 30, 30);
        painter.end();

    image.setPixmap(scaledImage);

    accept();
}

void Matrix::drawGrid()
{
    int ws=scaledImage.width()/10;
    int hs=scaledImage.height()/10;
    int w=scaledImage.width();
    int h=scaledImage.height();
    QPainter painter(&scaledImage);
    painter.setPen(QPen(Qt::black, 1));
    for(int i=1;i<=9;i++)
    {
        painter.drawLine(ws*i, 0, ws*i, w);
    }
    for(int i=1;i<=9;i++)
    {
        painter.drawLine(0, hs*i, h, hs*i);
    }
    painter.end();
    image.setPixmap(scaledImage);
}


// Y = 0
void Matrix::on_EnFrame_0_0_clicked(bool checked)
{
    if(ui->EnFrame_0_0->hasFocus())
        FrameConfig[0][0].used=checked;
}

void Matrix::on_EnFrame_0_1_clicked(bool checked)
{
    if(ui->EnFrame_0_1->hasFocus())
        FrameConfig[1][0].used=checked;
}

void Matrix::on_EnFrame_0_2_clicked(bool checked)
{
    if(ui->EnFrame_0_2->hasFocus())
        FrameConfig[2][0].used=checked;
}

void Matrix::on_EnFrame_0_3_clicked(bool checked)
{
    if(ui->EnFrame_0_3->hasFocus())
        FrameConfig[3][0].used=checked;
}

void Matrix::on_EnFrame_0_4_clicked(bool checked)
{
    if(ui->EnFrame_0_4->hasFocus())
        FrameConfig[4][0].used=checked;
}

void Matrix::on_EnFrame_0_5_clicked(bool checked)
{
    if(ui->EnFrame_0_5->hasFocus())
        FrameConfig[5][0].used=checked;
}

void Matrix::on_EnFrame_0_6_clicked(bool checked)
{
    if(ui->EnFrame_0_6->hasFocus())
        FrameConfig[6][0].used=checked;
}

void Matrix::on_EnFrame_0_7_clicked(bool checked)
{
    if(ui->EnFrame_0_7->hasFocus())
        FrameConfig[7][0].used=checked;
}

void Matrix::on_EnFrame_0_8_clicked(bool checked)
{
    if(ui->EnFrame_0_8->hasFocus())
        FrameConfig[8][0].used=checked;
}

void Matrix::on_EnFrame_0_9_clicked(bool checked)
{
    if(ui->EnFrame_0_9->hasFocus())
        FrameConfig[9][0].used=checked;
}



// Y = 1

void Matrix::on_EnFrame_1_0_clicked(bool checked)
{
    if(ui->EnFrame_1_0->hasFocus())
        FrameConfig[0][1].used=checked;
}

void Matrix::on_EnFrame_1_1_clicked(bool checked)
{
    if(ui->EnFrame_1_1->hasFocus())
        FrameConfig[1][1].used=checked;
}

void Matrix::on_EnFrame_1_2_clicked(bool checked)
{
    if(ui->EnFrame_1_2->hasFocus())
        FrameConfig[2][1].used=checked;
}

void Matrix::on_EnFrame_1_3_clicked(bool checked)
{
    if(ui->EnFrame_1_3->hasFocus())
        FrameConfig[3][1].used=checked;
}

void Matrix::on_EnFrame_1_4_clicked(bool checked)
{
    if(ui->EnFrame_1_4->hasFocus())
        FrameConfig[4][1].used=checked;
}

void Matrix::on_EnFrame_1_5_clicked(bool checked)
{
    if(ui->EnFrame_1_5->hasFocus())
        FrameConfig[5][1].used=checked;
}

void Matrix::on_EnFrame_1_6_clicked(bool checked)
{
    if(ui->EnFrame_1_6->hasFocus())
        FrameConfig[6][1].used=checked;
}

void Matrix::on_EnFrame_1_7_clicked(bool checked)
{
    if(ui->EnFrame_1_7->hasFocus())
        FrameConfig[7][1].used=checked;
}

void Matrix::on_EnFrame_1_8_clicked(bool checked)
{
    if(ui->EnFrame_1_8->hasFocus())
        FrameConfig[8][1].used=checked;
}

void Matrix::on_EnFrame_1_9_clicked(bool checked)
{
    if(ui->EnFrame_1_9->hasFocus())
        FrameConfig[9][1].used=checked;
}

// Y = 2

void Matrix::on_EnFrame_2_0_clicked(bool checked)
{
    if(ui->EnFrame_2_0->hasFocus())
        FrameConfig[0][2].used=checked;
}

void Matrix::on_EnFrame_2_1_clicked(bool checked)
{
    if(ui->EnFrame_2_1->hasFocus())
        FrameConfig[1][2].used=checked;
}

void Matrix::on_EnFrame_2_2_clicked(bool checked)
{
    if(ui->EnFrame_2_2->hasFocus())
        FrameConfig[2][2].used=checked;
}

void Matrix::on_EnFrame_2_3_clicked(bool checked)
{
    if(ui->EnFrame_2_3->hasFocus())
        FrameConfig[3][2].used=checked;
}

void Matrix::on_EnFrame_2_4_clicked(bool checked)
{
    if(ui->EnFrame_2_4->hasFocus())
        FrameConfig[4][2].used=checked;
}

void Matrix::on_EnFrame_2_5_clicked(bool checked)
{
    if(ui->EnFrame_2_5->hasFocus())
        FrameConfig[5][2].used=checked;
}

void Matrix::on_EnFrame_2_6_clicked(bool checked)
{
    if(ui->EnFrame_2_6->hasFocus())
        FrameConfig[6][2].used=checked;
}

void Matrix::on_EnFrame_2_7_clicked(bool checked)
{
    if(ui->EnFrame_2_7->hasFocus())
        FrameConfig[7][2].used=checked;
}

void Matrix::on_EnFrame_2_8_clicked(bool checked)
{
    if(ui->EnFrame_2_8->hasFocus())
        FrameConfig[8][2].used=checked;
}

void Matrix::on_EnFrame_2_9_clicked(bool checked)
{
    if(ui->EnFrame_2_9->hasFocus())
        FrameConfig[9][2].used=checked;
}

// Y = 3

void Matrix::on_EnFrame_3_0_clicked(bool checked)
{
    if(ui->EnFrame_3_0->hasFocus())
        FrameConfig[0][3].used=checked;
}

void Matrix::on_EnFrame_3_1_clicked(bool checked)
{
    if(ui->EnFrame_3_1->hasFocus())
        FrameConfig[1][3].used=checked;
}

void Matrix::on_EnFrame_3_2_clicked(bool checked)
{
    if(ui->EnFrame_3_2->hasFocus())
        FrameConfig[2][3].used=checked;
}

void Matrix::on_EnFrame_3_3_clicked(bool checked)
{
    if(ui->EnFrame_3_3->hasFocus())
        FrameConfig[3][3].used=checked;
}

void Matrix::on_EnFrame_3_4_clicked(bool checked)
{
    if(ui->EnFrame_3_4->hasFocus())
        FrameConfig[4][3].used=checked;
}

void Matrix::on_EnFrame_3_5_clicked(bool checked)
{
    if(ui->EnFrame_3_5->hasFocus())
        FrameConfig[5][3].used=checked;
}

void Matrix::on_EnFrame_3_6_clicked(bool checked)
{
    if(ui->EnFrame_3_6->hasFocus())
        FrameConfig[6][3].used=checked;
}

void Matrix::on_EnFrame_3_7_clicked(bool checked)
{
    if(ui->EnFrame_3_7->hasFocus())
        FrameConfig[7][3].used=checked;
}

void Matrix::on_EnFrame_3_8_clicked(bool checked)
{
    if(ui->EnFrame_3_8->hasFocus())
        FrameConfig[8][3].used=checked;
}

void Matrix::on_EnFrame_3_9_clicked(bool checked)
{
    if(ui->EnFrame_3_9->hasFocus())
        FrameConfig[9][3].used=checked;
}


// Y = 4

void Matrix::on_EnFrame_4_0_clicked(bool checked)
{
    if(ui->EnFrame_4_0->hasFocus())
        FrameConfig[0][4].used=checked;
}

void Matrix::on_EnFrame_4_1_clicked(bool checked)
{
    if(ui->EnFrame_4_1->hasFocus())
        FrameConfig[1][4].used=checked;
}

void Matrix::on_EnFrame_4_2_clicked(bool checked)
{
    if(ui->EnFrame_4_2->hasFocus())
        FrameConfig[2][4].used=checked;
}

void Matrix::on_EnFrame_4_3_clicked(bool checked)
{
    if(ui->EnFrame_4_3->hasFocus())
        FrameConfig[3][4].used=checked;
}

void Matrix::on_EnFrame_4_4_clicked(bool checked)
{
    if(ui->EnFrame_4_4->hasFocus())
        FrameConfig[4][4].used=checked;
}

void Matrix::on_EnFrame_4_5_clicked(bool checked)
{
    if(ui->EnFrame_4_5->hasFocus())
        FrameConfig[5][4].used=checked;
}

void Matrix::on_EnFrame_4_6_clicked(bool checked)
{
    if(ui->EnFrame_4_6->hasFocus())
        FrameConfig[6][4].used=checked;
}

void Matrix::on_EnFrame_4_7_clicked(bool checked)
{
    if(ui->EnFrame_4_7->hasFocus())
        FrameConfig[7][4].used=checked;
}

void Matrix::on_EnFrame_4_8_clicked(bool checked)
{
    if(ui->EnFrame_4_8->hasFocus())
        FrameConfig[8][4].used=checked;
}

void Matrix::on_EnFrame_4_9_clicked(bool checked)
{
    if(ui->EnFrame_4_9->hasFocus())
        FrameConfig[9][4].used=checked;
}

// Y = 5

void Matrix::on_EnFrame_5_0_clicked(bool checked)
{
    if(ui->EnFrame_5_0->hasFocus())
        FrameConfig[0][5].used=checked;
}

void Matrix::on_EnFrame_5_1_clicked(bool checked)
{
    if(ui->EnFrame_5_1->hasFocus())
        FrameConfig[1][5].used=checked;
}

void Matrix::on_EnFrame_5_2_clicked(bool checked)
{
    if(ui->EnFrame_5_2->hasFocus())
        FrameConfig[2][5].used=checked;
}

void Matrix::on_EnFrame_5_3_clicked(bool checked)
{
    if(ui->EnFrame_5_3->hasFocus())
        FrameConfig[3][5].used=checked;
}

void Matrix::on_EnFrame_5_4_clicked(bool checked)
{
    if(ui->EnFrame_5_4->hasFocus())
        FrameConfig[4][5].used=checked;
}

void Matrix::on_EnFrame_5_5_clicked(bool checked)
{
    if(ui->EnFrame_5_5->hasFocus())
        FrameConfig[5][5].used=checked;
}

void Matrix::on_EnFrame_5_6_clicked(bool checked)
{
    if(ui->EnFrame_5_6->hasFocus())
        FrameConfig[6][5].used=checked;
}

void Matrix::on_EnFrame_5_7_clicked(bool checked)
{
    if(ui->EnFrame_5_7->hasFocus())
        FrameConfig[7][5].used=checked;
}

void Matrix::on_EnFrame_5_8_clicked(bool checked)
{
    if(ui->EnFrame_5_8->hasFocus())
        FrameConfig[8][5].used=checked;
}

void Matrix::on_EnFrame_5_9_clicked(bool checked)
{
    if(ui->EnFrame_5_9->hasFocus())
        FrameConfig[9][5].used=checked;
}

// Y = 6

void Matrix::on_EnFrame_6_0_clicked(bool checked)
{
    if(ui->EnFrame_6_0->hasFocus())
        FrameConfig[0][6].used=checked;
}

void Matrix::on_EnFrame_6_1_clicked(bool checked)
{
    if(ui->EnFrame_6_1->hasFocus())
        FrameConfig[1][6].used=checked;
}

void Matrix::on_EnFrame_6_2_clicked(bool checked)
{
    if(ui->EnFrame_6_2->hasFocus())
        FrameConfig[2][6].used=checked;
}

void Matrix::on_EnFrame_6_3_clicked(bool checked)
{
    if(ui->EnFrame_6_3->hasFocus())
        FrameConfig[3][6].used=checked;
}

void Matrix::on_EnFrame_6_4_clicked(bool checked)
{
    if(ui->EnFrame_6_4->hasFocus())
        FrameConfig[4][6].used=checked;
}

void Matrix::on_EnFrame_6_5_clicked(bool checked)
{
    if(ui->EnFrame_6_5->hasFocus())
        FrameConfig[5][6].used=checked;
}

void Matrix::on_EnFrame_6_6_clicked(bool checked)
{
    if(ui->EnFrame_6_6->hasFocus())
        FrameConfig[6][6].used=checked;
}

void Matrix::on_EnFrame_6_7_clicked(bool checked)
{
    if(ui->EnFrame_6_7->hasFocus())
        FrameConfig[7][6].used=checked;
}

void Matrix::on_EnFrame_6_8_clicked(bool checked)
{
    if(ui->EnFrame_6_8->hasFocus())
        FrameConfig[8][6].used=checked;
}

void Matrix::on_EnFrame_6_9_clicked(bool checked)
{
    if(ui->EnFrame_6_9->hasFocus())
        FrameConfig[9][6].used=checked;
}

// Y = 7

void Matrix::on_EnFrame_7_0_clicked(bool checked)
{
    if(ui->EnFrame_7_0->hasFocus())
        FrameConfig[0][7].used=checked;
}

void Matrix::on_EnFrame_7_1_clicked(bool checked)
{
    if(ui->EnFrame_7_1->hasFocus())
        FrameConfig[1][7].used=checked;
}

void Matrix::on_EnFrame_7_2_clicked(bool checked)
{
    if(ui->EnFrame_7_2->hasFocus())
        FrameConfig[2][7].used=checked;
}

void Matrix::on_EnFrame_7_3_clicked(bool checked)
{
    if(ui->EnFrame_7_3->hasFocus())
        FrameConfig[3][7].used=checked;
}

void Matrix::on_EnFrame_7_4_clicked(bool checked)
{
    if(ui->EnFrame_7_4->hasFocus())
        FrameConfig[4][7].used=checked;
}

void Matrix::on_EnFrame_7_5_clicked(bool checked)
{
    if(ui->EnFrame_7_5->hasFocus())
        FrameConfig[5][7].used=checked;
}

void Matrix::on_EnFrame_7_6_clicked(bool checked)
{
    if(ui->EnFrame_7_6->hasFocus())
        FrameConfig[6][7].used=checked;
}

void Matrix::on_EnFrame_7_7_clicked(bool checked)
{
    if(ui->EnFrame_7_7->hasFocus())
        FrameConfig[7][7].used=checked;
}

void Matrix::on_EnFrame_7_8_clicked(bool checked)
{
    if(ui->EnFrame_7_8->hasFocus())
        FrameConfig[8][7].used=checked;
}

void Matrix::on_EnFrame_7_9_clicked(bool checked)
{
    if(ui->EnFrame_7_9->hasFocus())
        FrameConfig[9][7].used=checked;
}



// Y = 8

void Matrix::on_EnFrame_8_0_clicked(bool checked)
{
    if(ui->EnFrame_8_0->hasFocus())
        FrameConfig[0][8].used=checked;
}

void Matrix::on_EnFrame_8_1_clicked(bool checked)
{
    if(ui->EnFrame_8_1->hasFocus())
        FrameConfig[1][8].used=checked;
}

void Matrix::on_EnFrame_8_2_clicked(bool checked)
{
    if(ui->EnFrame_8_2->hasFocus())
        FrameConfig[2][8].used=checked;
}

void Matrix::on_EnFrame_8_3_clicked(bool checked)
{
    if(ui->EnFrame_8_3->hasFocus())
        FrameConfig[3][8].used=checked;
}

void Matrix::on_EnFrame_8_4_clicked(bool checked)
{
    if(ui->EnFrame_8_4->hasFocus())
        FrameConfig[4][8].used=checked;
}

void Matrix::on_EnFrame_8_5_clicked(bool checked)
{
    if(ui->EnFrame_8_5->hasFocus())
        FrameConfig[5][8].used=checked;
}

void Matrix::on_EnFrame_8_6_clicked(bool checked)
{
    if(ui->EnFrame_8_6->hasFocus())
        FrameConfig[6][8].used=checked;
}

void Matrix::on_EnFrame_8_7_clicked(bool checked)
{
    if(ui->EnFrame_8_7->hasFocus())
        FrameConfig[7][8].used=checked;
}

void Matrix::on_EnFrame_8_8_clicked(bool checked)
{
    if(ui->EnFrame_8_8->hasFocus())
        FrameConfig[8][8].used=checked;
}

void Matrix::on_EnFrame_8_9_clicked(bool checked)
{
    if(ui->EnFrame_8_9->hasFocus())
        FrameConfig[9][8].used=checked;
}


// Y = 9

void Matrix::on_EnFrame_9_0_clicked(bool checked)
{
    if(ui->EnFrame_9_0->hasFocus())
        FrameConfig[0][9].used=checked;
}

void Matrix::on_EnFrame_9_1_clicked(bool checked)
{
    if(ui->EnFrame_9_1->hasFocus())
        FrameConfig[1][9].used=checked;
}

void Matrix::on_EnFrame_9_2_clicked(bool checked)
{
    if(ui->EnFrame_9_2->hasFocus())
        FrameConfig[2][9].used=checked;
}

void Matrix::on_EnFrame_9_3_clicked(bool checked)
{
    if(ui->EnFrame_9_3->hasFocus())
        FrameConfig[3][9].used=checked;
}

void Matrix::on_EnFrame_9_4_clicked(bool checked)
{
    if(ui->EnFrame_9_4->hasFocus())
        FrameConfig[4][9].used=checked;
}

void Matrix::on_EnFrame_9_5_clicked(bool checked)
{
    if(ui->EnFrame_9_5->hasFocus())
        FrameConfig[5][9].used=checked;
}

void Matrix::on_EnFrame_9_6_clicked(bool checked)
{
    if(ui->EnFrame_9_6->hasFocus())
        FrameConfig[6][9].used=checked;
}

void Matrix::on_EnFrame_9_7_clicked(bool checked)
{
    if(ui->EnFrame_9_7->hasFocus())
        FrameConfig[7][9].used=checked;
}

void Matrix::on_EnFrame_9_8_clicked(bool checked)
{
    if(ui->EnFrame_9_8->hasFocus())
        FrameConfig[8][9].used=checked;
}

void Matrix::on_EnFrame_9_9_clicked(bool checked)
{
    if(ui->EnFrame_9_9->hasFocus())
        FrameConfig[9][9].used=checked;
}




// Y = 0
void Matrix::on_SetFrame_0_0_clicked()
{
    setFrame(0,0);
}
void Matrix::on_SetFrame_0_1_clicked()
{
    setFrame(1,0);
}
void Matrix::on_SetFrame_0_2_clicked()
{
    setFrame(2,0);
}
void Matrix::on_SetFrame_0_3_clicked()
{
    setFrame(3,0);
}
void Matrix::on_SetFrame_0_4_clicked()
{
    setFrame(4,0);
}
void Matrix::on_SetFrame_0_5_clicked()
{
    setFrame(5,0);
}
void Matrix::on_SetFrame_0_6_clicked()
{
    setFrame(6,0);
}
void Matrix::on_SetFrame_0_7_clicked()
{
    setFrame(7,0);
}
void Matrix::on_SetFrame_0_8_clicked()
{
    setFrame(8,0);
}
void Matrix::on_SetFrame_0_9_clicked()
{
    setFrame(9,0);
}


// Y = 1
void Matrix::on_SetFrame_1_0_clicked()
{
    setFrame(0,1);
}
void Matrix::on_SetFrame_1_1_clicked()
{
    setFrame(1,1);
}
void Matrix::on_SetFrame_1_2_clicked()
{
    setFrame(2,1);
}
void Matrix::on_SetFrame_1_3_clicked()
{
    setFrame(3,1);
}
void Matrix::on_SetFrame_1_4_clicked()
{
    setFrame(4,1);
}
void Matrix::on_SetFrame_1_5_clicked()
{
    setFrame(5,1);
}
void Matrix::on_SetFrame_1_6_clicked()
{
    setFrame(6,1);
}
void Matrix::on_SetFrame_1_7_clicked()
{
    setFrame(7,1);
}
void Matrix::on_SetFrame_1_8_clicked()
{
    setFrame(8,1);
}
void Matrix::on_SetFrame_1_9_clicked()
{
    setFrame(9,1);
}


// Y = 2
void Matrix::on_SetFrame_2_0_clicked()
{
    setFrame(0,2);
}
void Matrix::on_SetFrame_2_1_clicked()
{
    setFrame(1,2);
}
void Matrix::on_SetFrame_2_2_clicked()
{
    setFrame(2,2);
}
void Matrix::on_SetFrame_2_3_clicked()
{
    setFrame(3,2);
}
void Matrix::on_SetFrame_2_4_clicked()
{
    setFrame(4,2);
}
void Matrix::on_SetFrame_2_5_clicked()
{
    setFrame(5,2);
}
void Matrix::on_SetFrame_2_6_clicked()
{
    setFrame(6,2);
}
void Matrix::on_SetFrame_2_7_clicked()
{
    setFrame(7,2);
}
void Matrix::on_SetFrame_2_8_clicked()
{
    setFrame(8,2);
}
void Matrix::on_SetFrame_2_9_clicked()
{
    setFrame(9,2);
}

// Y = 3
void Matrix::on_SetFrame_3_0_clicked()
{
    setFrame(0,3);
}
void Matrix::on_SetFrame_3_1_clicked()
{
    setFrame(1,3);
}
void Matrix::on_SetFrame_3_2_clicked()
{
    setFrame(2,3);
}
void Matrix::on_SetFrame_3_3_clicked()
{
    setFrame(3,3);
}
void Matrix::on_SetFrame_3_4_clicked()
{
    setFrame(4,3);
}
void Matrix::on_SetFrame_3_5_clicked()
{
    setFrame(5,3);
}
void Matrix::on_SetFrame_3_6_clicked()
{
    setFrame(6,3);
}
void Matrix::on_SetFrame_3_7_clicked()
{
    setFrame(7,3);
}
void Matrix::on_SetFrame_3_8_clicked()
{
    setFrame(8,3);
}
void Matrix::on_SetFrame_3_9_clicked()
{
    setFrame(9,3);
}

// Y = 4
void Matrix::on_SetFrame_4_0_clicked()
{
    setFrame(0,4);
}
void Matrix::on_SetFrame_4_1_clicked()
{
    setFrame(1,4);
}
void Matrix::on_SetFrame_4_2_clicked()
{
    setFrame(2,4);
}
void Matrix::on_SetFrame_4_3_clicked()
{
    setFrame(3,4);
}
void Matrix::on_SetFrame_4_4_clicked()
{
    setFrame(4,4);
}
void Matrix::on_SetFrame_4_5_clicked()
{
    setFrame(5,4);
}
void Matrix::on_SetFrame_4_6_clicked()
{
    setFrame(6,4);
}
void Matrix::on_SetFrame_4_7_clicked()
{
    setFrame(7,4);
}
void Matrix::on_SetFrame_4_8_clicked()
{
    setFrame(8,4);
}
void Matrix::on_SetFrame_4_9_clicked()
{
    setFrame(9,4);
}

// Y = 5
void Matrix::on_SetFrame_5_0_clicked()
{
    setFrame(0,5);
}
void Matrix::on_SetFrame_5_1_clicked()
{
    setFrame(1,5);
}
void Matrix::on_SetFrame_5_2_clicked()
{
    setFrame(2,5);
}
void Matrix::on_SetFrame_5_3_clicked()
{
    setFrame(3,5);
}
void Matrix::on_SetFrame_5_4_clicked()
{
    setFrame(4,5);
}
void Matrix::on_SetFrame_5_5_clicked()
{
    setFrame(5,5);
}
void Matrix::on_SetFrame_5_6_clicked()
{
    setFrame(6,5);
}
void Matrix::on_SetFrame_5_7_clicked()
{
    setFrame(7,5);
}
void Matrix::on_SetFrame_5_8_clicked()
{
    setFrame(8,5);
}
void Matrix::on_SetFrame_5_9_clicked()
{
    setFrame(9,5);
}

// Y = 6
void Matrix::on_SetFrame_6_0_clicked()
{
    setFrame(0,6);
}
void Matrix::on_SetFrame_6_1_clicked()
{
    setFrame(1,6);
}
void Matrix::on_SetFrame_6_2_clicked()
{
    setFrame(2,6);
}
void Matrix::on_SetFrame_6_3_clicked()
{
    setFrame(3,6);
}
void Matrix::on_SetFrame_6_4_clicked()
{
    setFrame(4,6);
}
void Matrix::on_SetFrame_6_5_clicked()
{
    setFrame(5,6);
}
void Matrix::on_SetFrame_6_6_clicked()
{
    setFrame(6,6);
}
void Matrix::on_SetFrame_6_7_clicked()
{
    setFrame(7,6);
}
void Matrix::on_SetFrame_6_8_clicked()
{
    setFrame(8,6);
}
void Matrix::on_SetFrame_6_9_clicked()
{
    setFrame(9,6);
}

// Y = 7
void Matrix::on_SetFrame_7_0_clicked()
{
    setFrame(0,7);
}
void Matrix::on_SetFrame_7_1_clicked()
{
    setFrame(1,7);
}
void Matrix::on_SetFrame_7_2_clicked()
{
    setFrame(2,7);
}
void Matrix::on_SetFrame_7_3_clicked()
{
    setFrame(3,7);
}
void Matrix::on_SetFrame_7_4_clicked()
{
    setFrame(4,7);
}
void Matrix::on_SetFrame_7_5_clicked()
{
    setFrame(5,7);
}
void Matrix::on_SetFrame_7_6_clicked()
{
    setFrame(6,7);
}
void Matrix::on_SetFrame_7_7_clicked()
{
    setFrame(7,7);
}
void Matrix::on_SetFrame_7_8_clicked()
{
    setFrame(8,7);
}
void Matrix::on_SetFrame_7_9_clicked()
{
    setFrame(9,7);
}

// Y = 8
void Matrix::on_SetFrame_8_0_clicked()
{
    setFrame(0,8);
}
void Matrix::on_SetFrame_8_1_clicked()
{
    setFrame(1,8);
}
void Matrix::on_SetFrame_8_2_clicked()
{
    setFrame(2,8);
}
void Matrix::on_SetFrame_8_3_clicked()
{
    setFrame(3,8);
}
void Matrix::on_SetFrame_8_4_clicked()
{
    setFrame(4,8);
}
void Matrix::on_SetFrame_8_5_clicked()
{
    setFrame(5,8);
}
void Matrix::on_SetFrame_8_6_clicked()
{
    setFrame(6,8);
}
void Matrix::on_SetFrame_8_7_clicked()
{
    setFrame(7,8);
}
void Matrix::on_SetFrame_8_8_clicked()
{
    setFrame(8,8);
}
void Matrix::on_SetFrame_8_9_clicked()
{
    setFrame(9,8);
}

// Y = 9
void Matrix::on_SetFrame_9_0_clicked()
{
    setFrame(0,9);
}
void Matrix::on_SetFrame_9_1_clicked()
{
    setFrame(1,9);
}
void Matrix::on_SetFrame_9_2_clicked()
{
    setFrame(2,9);
}
void Matrix::on_SetFrame_9_3_clicked()
{
    setFrame(3,9);
}
void Matrix::on_SetFrame_9_4_clicked()
{
    setFrame(4,9);
}
void Matrix::on_SetFrame_9_5_clicked()
{
    setFrame(5,9);
}
void Matrix::on_SetFrame_9_6_clicked()
{
    setFrame(6,9);
}
void Matrix::on_SetFrame_9_7_clicked()
{
    setFrame(7,9);
}
void Matrix::on_SetFrame_9_8_clicked()
{
    setFrame(8,9);
}
void Matrix::on_SetFrame_9_9_clicked()
{
    setFrame(9,9);
}


