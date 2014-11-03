/*
 * SMBX64 Playble Character Sprite Calibrator, a free tool for playable srite design
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../main/globals.h"
#include "../main/graphics.h"

#include "SpriteScene.h"
#include <QFileInfo>

SpriteScene::SpriteScene(QObject *parent) : QGraphicsScene(parent)
{
    QGraphicsItem * dragon;
    QGraphicsItem * Frame;
    QGraphicsItem * Square;
    QList<QString > tmp;
    QFileInfo ourFile(currentFile);
    QString imgFileM;
    QPixmap sprite;
    ImageFrame = new QGraphicsPixmapItem;
    SizerBox = new QGraphicsRectItem;

    m_CurrentFrameX = 0;
    m_CurrentFrameY = 0;

    //x=0; y=0, dir=1;
    m_FramePos = QPoint(0,0);

    tmp = ourFile.fileName().split(".", QString::SkipEmptyParts);
    if(tmp.size()==2)
        imgFileM = tmp[0] + "m." + tmp[1];
    else
        imgFileM = "";

    QImage maskImg;
    if(QFile::exists(ourFile.absoluteDir().path() + "/" + imgFileM))
        maskImg = Graphics::loadQImage( ourFile.absoluteDir().path() + "/" + imgFileM );
    else
        maskImg = QImage();

    sprite = QPixmap::fromImage(
                    Graphics::setAlphaMask(
                        Graphics::loadQImage( currentFile )
                        , maskImg )
                    );


    mSpriteImage = sprite;

    draw();

    Frame = addRect(0,0,100,100, QPen(Qt::gray, 1), Qt::transparent);
    Frame->setZValue(-10);

    ImageFrame->setPixmap(QPixmap(currentImage));
    dragon = addPixmap(QPixmap(currentImage));
    ImageFrame->setParentItem(dragon);
    dragon->setZValue(0);

    Square = addRect(0,0,0,0, QPen(Qt::NoPen), Qt::transparent);
    SizerBox->setRect(0,0,100,100);
    SizerBox->setPen(QPen(Qt::green));
    SizerBox->setParentItem(Square);
    SizerBox->setZValue(3);

    //qDebug() << "Scene created";
}

void SpriteScene::draw()
{
    currentImage =  mSpriteImage.copy(QRect(m_FramePos.x(), m_FramePos.y(), 100, 100));
            // mPos.x(),mPos.y(), *mSpriteImage, mCurrentFrame, 0, 100,100 );
}

QPoint SpriteScene::pos() const
{
    return m_FramePos;
}

void SpriteScene::setFrame(int x, int y)
{
    CurFrame_X=x;
    CurFrame_Y=y;
    m_CurrentFrameX = 100 * x;
    m_CurrentFrameY = 100 * y;

    m_FramePos.setX( m_CurrentFrameX );
    m_FramePos.setY( m_CurrentFrameY );

    draw();

    ImageFrame->setPixmap(QPixmap(currentImage));
}


void SpriteScene::setSquare(int x, int y, int h, int w)
{
    //following variable keeps track which
    //frame to show from sprite sheet
    SizerBox->setRect(x,y,w-1,h-1);
}


