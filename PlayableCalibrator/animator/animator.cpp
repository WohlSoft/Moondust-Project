/*
 * Playble Character Calibrator, a free tool for playable character sprite setup tune
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "animator.h"
#include <ui_animator.h>
#include "animation_edit.h"
#include "main/calibration.h"


Animator::Animator(Calibration &conf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Animator)
{
    ui->setupUi(this);

    m_conf = &conf;
    m_mw = qobject_cast<CalibrationMain*>(parent);
    Q_ASSERT(m_mw);

    m_noAnimate = QPixmap(":/images/NoAni.png");

    m_aniStyle = "Idle";
    m_aniDir = 1; //0 - left, 1 - right

    ui->preview->setDrawMetaData(false);

    m_timer.setInterval(128);
    QObject::connect(&m_timer, SIGNAL(timeout()), this, SLOT(nextFrame()));

    fullReload();
}

Animator::~Animator()
{
    int j = ui->animationsList->count() - 1;
    for(; j >= 0; j--)
    {
        QListWidgetItem *it = ui->animationsList->item(j);
        delete it;
    }
    delete ui;
}

void Animator::syncCalibration()
{
    ui->preview->setGlobalSetup(*m_conf);
    aniFindSet();
}

void Animator::fullReload()
{
    ui->preview->setGlobalSetup(*m_conf);
    rebuildAnimationsList();
    aniFindSet();
}

void Animator::languageSwitched()
{
    ui->retranslateUi(this);
}

void Animator::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Left:
        on_directLeft_clicked();
        ui->directLeft->setChecked(true);
        break;
    case Qt::Key_Right:
        on_directRight_clicked();
        ui->directRight->setChecked(true);
        break;
    default:
        break;
    }

    QDialog::keyPressEvent(e);
}


void Animator::aniFindSet()
{
    if(!m_conf->animations.contains(m_aniStyle))
        return;

    auto &frms = m_conf->animations[m_aniStyle];

    m_timer.stop();
    m_currentAnimation = (m_aniDir == 1) ? frms.R : frms.L;
    ui->preview->setAllowScroll(true);
    setFrame(0);
    m_timer.start();
}

void Animator::rebuildAnimationsList()
{
    ui->animationsList->blockSignals(true);

    ui->animationsList->clear();

    for(AniFrameSet frms : m_conf->animations)
        ui->animationsList->addItem(frms.name);

    for(int i = 0; i < ui->animationsList->count(); ++i)
    {
        auto *it = ui->animationsList->item(i);

        if(it->text() == m_aniStyle)
        {
            it->setSelected(true);
            ui->animationsList->scrollToItem(it);
            break;
        }
    }

    ui->animationsList->blockSignals(false);
}


/////////////////Slots//////////////////////////

void Animator::on_EditAnimationBtn_clicked()
{
    auto old = m_conf->animations;
    AnimationEdit dialog(m_conf, this->parent(), this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    m_conf->animations = dialog.m_frameList;
    rebuildAnimationsList();
    aniFindSet();

    if(old != m_conf->animations)
        emit settingsModified();
}

//Set Direction
void Animator::on_directLeft_clicked()
{
    m_aniDir = 0;
    aniFindSet();
}

void Animator::on_directRight_clicked()
{
    m_aniDir = 1;
    aniFindSet();
}

void Animator::on_FrameSpeed_valueChanged(int arg1)
{
    m_timer.setInterval(arg1);
}

void Animator::on_animationsList_itemSelectionChanged()
{
    auto s = ui->animationsList->selectedItems();
    if(s.isEmpty())
        return;
    auto *item = s.first();
    if(!item)
        return;
    m_aniStyle = item->text();
    aniFindSet();
}

void Animator::nextFrame()
{
    m_curFrameIdx++;

    if(m_curFrameIdx >= m_currentAnimation.size())
        m_curFrameIdx = 0;

    setFrame(m_curFrameIdx);
}

void Animator::setFrame(int frame)
{
    auto &img = m_mw->m_xImageSprite;
    int cellW = m_mw->m_xImageSprite.width() / m_conf->matrixWidth;
    int cellH = m_mw->m_xImageSprite.height() / m_conf->matrixWidth;

    if((m_currentAnimation.size() == 0) || (frame >= m_currentAnimation.size()))
    {
        CalibrationFrame frame;
        if(m_prevOffset.isNull())
            m_prevOffset = ui->preview->getOffset();
        ui->preview->setBlockRepaint(true);
        ui->preview->setAllowScroll(false);
        ui->preview->setFrameSetup(frame);
        ui->preview->setHitBoxFocus(false);
        ui->preview->resetScroll();
        ui->preview->setImage(m_noAnimate);
        ui->preview->setWall(FrameTuneScene::WALL_NONE);
        ui->preview->setBlockRepaint(false);
        return;
    }

    auto &cf = m_currentAnimation[frame];
    auto &ff = m_conf->frames[{cf.x, cf.y}];

    ui->preview->setBlockRepaint(true);
    ui->preview->setImage(img.copy(QRect(cf.x * cellW, cf.y * cellH, cellW, cellH)));
    ui->preview->setFrameSetup(ff);
    ui->preview->setHitBoxFocus(true);
    ui->preview->setWall(FrameTuneScene::WALL_FLOOR);
    if(!m_prevOffset.isNull())
    {
        ui->preview->setOffset(m_prevOffset);
        m_prevOffset.setX(0);
        m_prevOffset.setY(0);
    }
    ui->preview->setBlockRepaint(false);
}
