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

#include "animate.h"
#include <ui_animate.h>
#include "animationedit.h"
#include "aniFrames.h"
#include "../main/globals.h"
#include "../main/mw.h"

Animate::Animate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Animate)
{
    ui->setupUi(this);

    //Here will be read AniFrames from INI

    AniScene = new AnimationScene();
    AniScene->setSceneRect(0,0, ui->AnimateView->width()-20, ui->AnimateView->height()-20);

    ui->AnimateView->setScene(AniScene);

    aniStyle = "Idle";
    aniDir = 1; //0 - left, 1 - right

    foreach(AniFrameSet frms, AniFrames.set)
        ui->animationsList->addItem(frms.name);
    QList<QListWidgetItem*> items=ui->animationsList->findItems("*", Qt::MatchWildcard);
    if(!items.isEmpty())
        items.first()->setSelected(true);

    AniScene->setAnimation(AniFrames.set[0].R);
}

Animate::~Animate()
{
      int j = ui->animationsList->count() - 1;
      for(; j>=0; j--) {
        QListWidgetItem *it = ui->animationsList->item(j);
            delete it;
      }
    delete ui;
}

void Animate::keyPressEvent(QKeyEvent *e)
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
    case Qt::Key_Up:
    case Qt::Key_Down:
        if(!ui->animationsList->selectedItems().isEmpty())
            on_animationsList_itemClicked(ui->animationsList->selectedItems().first());
    default:
        break;
    }

    QDialog::keyPressEvent(e);
}


void Animate::aniFindSet()
{
    foreach(AniFrameSet frms, AniFrames.set)
    {
        if(frms.name==aniStyle)
        {
            if(aniDir==1)
                AniScene->setAnimation(frms.R);
            else
                AniScene->setAnimation(frms.L);
            break;
        }
    }
}



/////////////////Slots//////////////////////////

void Animate::on_EditAnimationBtn_clicked()
{
    AnimationEdit dialog(AniFrames);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    dialog.exec();
    AniFrames = dialog.frameList;
    aniFindSet();
}

//Set Direction
void Animate::on_directLeft_clicked()
{
    aniDir=0;
    aniFindSet();
}

void Animate::on_directRight_clicked()
{
    aniDir=1;
    aniFindSet();
}

void Animate::on_FrameSpeed_valueChanged(int arg1)
{
    AniScene->timer.setInterval(arg1);
}


void Animate::on_animationsList_itemClicked(QListWidgetItem *item)
{
    if(!item) return;
    aniStyle=item->text();
    aniFindSet();
}

void Animate::on_animationsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *)
{
    on_animationsList_itemClicked(current);
}
