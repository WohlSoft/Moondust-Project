/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "AnimationScene.h"

AnimationScene::AnimationScene(Calibration &conf, QObject *parent) :
    QGraphicsScene(parent)
{
    auto *mw = qobject_cast<CalibrationMain*>(this->parent());
    Q_ASSERT(mw);

    m_conf = &conf;
    m_allAnimations = conf.animations;
    m_framesTable = conf.frames;
    m_spriteImage = mw->m_xImageSprite;
    m_noAnimate = QPixmap(":/images/NoAni.png");
    draw();
    m_currentImage = m_noAnimate;
    m_imageFrame.setPixmap(m_currentImage);
    addItem(&m_imageFrame);
    m_imageFrame.setPos(100, 200);
    m_imageFrame.setZValue(1.0);
    m_ground.setRect(1, 1, 200, 20);
    m_ground.setBrush(QBrush(Qt::lightGray));
    m_ground.setPen(QPen(QBrush(Qt::gray), 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    m_ground.setZValue(-5.0);
    addItem(&m_ground);
    m_timer.setInterval(128);
    connect(&m_timer, SIGNAL(timeout()),
            this, SLOT(nextFrame()));
}

void AnimationScene::setAnimation(QVector<AniFrame > frameS)
{
    m_timer.stop();
    m_currentAnimation = frameS;
    m_curFrameIdx = 0;
    setFrame(0);
    m_timer.start();
}



void AnimationScene::draw()
{
    m_currentImage =  m_spriteImage.copy(QRect(m_pos.x(), m_pos.y(), FRAME_WIDTH, FRAME_HEIGHT));
    // mPos.x(),mPos.y(), *mSpriteImage, mCurrentFrame, 0, 100,100 );
}

QPoint AnimationScene::pos() const
{
    return m_pos;
}

void AnimationScene::setFrame(int frame)
{
    int w, h, x, y;

    if((m_currentAnimation.size() == 0) || (frame >= m_currentAnimation.size()))
    {
        m_imageFrame.setPos(this->sceneRect().width() / 2 - 50.0, this->sceneRect().height() / 2 - 50.0);
        m_currentImage = m_noAnimate;
        m_imageFrame.setPixmap(m_currentImage);
        m_ground.hide();
        return;
    }

    //following variable keeps track which
    //frame to show from sprite sheet
    m_currentFrameX = FRAME_WIDTH * m_currentAnimation[frame].x;
    m_pos.setX(m_currentFrameX);
    m_currentFrameY = FRAME_HEIGHT * m_currentAnimation[frame].y;
    m_pos.setY(m_currentFrameY);
    w = m_conf->frameWidth;
    auto &f = m_currentAnimation[frame];
    h = m_framesTable[{f.x, f.y}].isDuck ? m_conf->frameHeightDuck : m_conf->frameHeight;
    x = m_framesTable[{f.x, f.y}].offsetX;
    y = m_framesTable[{f.x, f.y}].offsetY;
    draw();
    m_imageFrame.setPos(this->sceneRect().width() / 2 - w / 2, this->sceneRect().height() / 2 + (100.0 - h));
    m_ground.show();
    m_ground.setPos(this->sceneRect().width() / 2 - m_ground.rect().width() / 2,
                  this->sceneRect().height() / 2 + 100);
    m_imageFrame.setOffset(-x, -y);
    m_imageFrame.setPixmap(m_currentImage);
}

void AnimationScene::setFrameInterval(int interval)
{
    m_timer.setInterval(interval);
}

void AnimationScene::nextFrame()
{
    m_curFrameIdx++;

    if(m_curFrameIdx >= m_currentAnimation.size())
        m_curFrameIdx = 0;

    setFrame(m_curFrameIdx);
}
