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

    //Here will be read AniFrames from INI

    m_aniScene = new AnimationScene(conf, parent);
    m_aniScene->setSceneRect(0, 0,
                             ui->AnimatorView->width() - 20,
                             ui->AnimatorView->height() - 20);

    ui->AnimatorView->setScene(m_aniScene);

    m_aniStyle = "Idle";
    m_aniDir = 1; //0 - left, 1 - right

    rebuildAnimationsList();

    aniFindSet();
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

    if(m_aniDir == 1)
        m_aniScene->setAnimation(frms.R);
    else
        m_aniScene->setAnimation(frms.L);
}

void Animator::rebuildAnimationsList()
{
    ui->animationsList->clear();
    for(AniFrameSet frms : m_conf->animations)
        ui->animationsList->addItem(frms.name);

    QList<QListWidgetItem *> items = ui->animationsList->findItems("*", Qt::MatchWildcard);
    for(auto &it : items)
    {
        if(it->text() == m_aniStyle)
        {
            it->setSelected(true);
            ui->animationsList->scrollToItem(it);
            break;
        }
    }
}


/////////////////Slots//////////////////////////

void Animator::on_EditAnimationBtn_clicked()
{
    AnimationEdit dialog(m_conf, this->parent(), this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    m_conf->animations = dialog.m_frameList;
    rebuildAnimationsList();
    aniFindSet();
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
    m_aniScene->setFrameInterval(arg1);
}

void Animator::on_animationsList_currentItemChanged(QListWidgetItem *item, QListWidgetItem *)
{
    if(!item)
        return;
    m_aniStyle = item->text();
    aniFindSet();
}
