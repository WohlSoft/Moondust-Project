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

#include "animationedit.h"
#include <ui_animationedit.h>
#include "../frame_matrix/matrix.h"
#include "../main/globals.h"
#include "../main/mw.h"

AnimationEdit::AnimationEdit(FrameSets &frmConfs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationEdit)
{
    SrcFrames = framesX;
    frameList = frmConfs;

    direction = 1;

    ui->setupUi(this);

    ui->FramesSets->clear();
    for(int i=0; i<frameList.set.size(); i++)
    {
        ui->FramesSets->addItem(frameList.set[i].name);
    }

    ticker.connect(&ticker, SIGNAL(timeout()), this, SLOT(nextFrame()) );
    ticker.setInterval(ui->frameSpeed->value());
}

AnimationEdit::~AnimationEdit()
{
    delete ui;
}

void AnimationEdit::on_AddLeft_clicked()
{
    int x=-1;
    int y=-1;

    Matrix dialog;

    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(dialog.exec()==QDialog::Accepted)
    {
        x = dialog.frameX;
        y = dialog.frameY;

        addFrameL(x, y);

        applyFrameSet();
    }
    else
        return;

}

void AnimationEdit::on_SetLeft_clicked()
{
    int x=-1;
    int y=-1;

    QList<QListWidgetItem *> selected = ui->FramesL->selectedItems();

    Matrix dialog;

    foreach(QListWidgetItem * item, selected)
    {
        dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
        dialog.setFrame(item->data(Qt::UserRole).toPoint().x(), item->data(Qt::UserRole).toPoint().y());
        if(dialog.exec()==QDialog::Accepted)
        {
            x = dialog.frameX;
            y = dialog.frameY;
            item->setData(Qt::UserRole, QPoint(x,y));
            item->setText( QString("X"+QString::number(x)+"-Y"+QString::number(y)) );
            applyFrameSet();
            showFrame(x, y);
            return;
        }
        else
            return;
    }

}

void AnimationEdit::on_DelLeft_clicked()
{
    QList<QListWidgetItem *> selected = ui->FramesL->selectedItems();
    qDeleteAll(selected);
    applyFrameSet();
}

void AnimationEdit::on_AddRight_clicked()
{
    int x=-1;
    int y=-1;

    Matrix dialog;

    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
    if(dialog.exec()==QDialog::Accepted)
    {
        x = dialog.frameX;
        y = dialog.frameY;
        addFrameR(x, y);
        applyFrameSet();
    }
    else
        return;
}


void AnimationEdit::on_SetRight_clicked()
{
    int x=-1;
    int y=-1;

    QList<QListWidgetItem *> selected = ui->FramesR->selectedItems();

    Matrix dialog;
    foreach(QListWidgetItem * item, selected)
    {
        dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
        dialog.setFrame(item->data(Qt::UserRole).toPoint().x(), item->data(Qt::UserRole).toPoint().y());
        if(dialog.exec()==QDialog::Accepted)
        {
            x = dialog.frameX;
            y = dialog.frameY;
            item->setData(Qt::UserRole, QPoint(x,y));
            item->setText( QString("X"+QString::number(x)+"-Y"+QString::number(y)) );
            applyFrameSet();
            showFrame(x, y);
            return;
        }
        else
            return;
    }
}

void AnimationEdit::on_DelRight_clicked()
{
    QList<QListWidgetItem *> selected = ui->FramesR->selectedItems();
    qDeleteAll(selected);
    applyFrameSet();
}


void AnimationEdit::addFrameL(int x, int y)
{
    QListWidgetItem * test1 = new QListWidgetItem;
    test1->setData(Qt::UserRole, QPoint(x, y));
    test1->setText( QString("X"+QString::number(x)+"-Y"+QString::number(y)) );
    ui->FramesL->addItem(test1);
}

void AnimationEdit::addFrameR(int x, int y)
{
    QListWidgetItem * test2 = new QListWidgetItem;
    test2->setData(Qt::UserRole, QPoint(x, y));
    test2->setText( QString("X"+QString::number(x)+"-Y"+QString::number(y)) );
    ui->FramesR->addItem(test2);
}

void  AnimationEdit::applyFrameSet()
{
    QList<QListWidgetItem *> selected = ui->FramesSets->selectedItems();
    int i=0;
    AniFrame frameAdd;
    QList<QListWidgetItem *> items;

    foreach(QListWidgetItem * item, selected)
    {
        for(i=0;i<frameList.set.size(); i++)
        {
            if(frameList.set[i].name==item->text())
            {
                frameList.set[i].L.clear();
                items = ui->FramesL->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
                foreach(QListWidgetItem * item, items)
                {
                    frameAdd.x = item->data(Qt::UserRole).toPoint().x();
                    frameAdd.y = item->data(Qt::UserRole).toPoint().y();
                    frameList.set[i].L.push_back(frameAdd);
                }

                frameList.set[i].R.clear();
                items = ui->FramesR->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);
                foreach(QListWidgetItem * item, items)
                {
                    frameAdd.x = item->data(Qt::UserRole).toPoint().x();
                    frameAdd.y = item->data(Qt::UserRole).toPoint().y();
                    frameList.set[i].R.push_back(frameAdd);
                }

                break;
            }
        }
    }

}


void AnimationEdit::on_FramesSets_currentItemChanged(QListWidgetItem *item, QListWidgetItem *)
{
    if(!item) return;

    ui->FramesL->setEnabled(true);
    ui->FramesR->setEnabled(true);

    ui->AddLeft->setEnabled(true);
    ui->AddRight->setEnabled(true);
    ui->SetLeft->setEnabled(true);
    ui->SetRight->setEnabled(true);
    ui->DelLeft->setEnabled(true);
    ui->DelRight->setEnabled(true);


    ui->FramesL->clear();
    ui->FramesR->clear();
    foreach(AniFrameSet frames, frameList.set)
    {
        if(frames.name==item->text())
        {
        foreach(AniFrame frame, frames.L)
            addFrameL(frame.x, frame.y);
        foreach(AniFrame frame, frames.R)
            addFrameR(frame.x, frame.y);
        break;
        }
    }

}

void AnimationEdit::showFrame(int x, int y)
{
    qDebug() << x << y;
    int fW = MW::sprite().width()/10;
    int fH = MW::sprite().height()/10;
    current_frame = MW::sprite().copy(x*fW, y*fH, fW, fH);
    ui->img->setPixmap(current_frame);
}

void AnimationEdit::on_FramesL_itemSelectionChanged()
{
    if(ui->FramesL->selectedItems().isEmpty()) return;

    showFrame(
              ui->FramesL->selectedItems().first()->data(Qt::UserRole).toPoint().x(),
              ui->FramesL->selectedItems().first()->data(Qt::UserRole).toPoint().y()
              );
}

void AnimationEdit::on_FramesR_itemSelectionChanged()
{
    if(ui->FramesR->selectedItems().isEmpty()) return;

    showFrame(
              ui->FramesR->selectedItems().first()->data(Qt::UserRole).toPoint().x(),
              ui->FramesR->selectedItems().first()->data(Qt::UserRole).toPoint().y()
              );
}

void AnimationEdit::on_FramesL_currentItemChanged(QListWidgetItem *item, QListWidgetItem *)
{
    if(!item) return;
    pause();
    showFrame(
              item->data(Qt::UserRole).toPoint().x(),
              item->data(Qt::UserRole).toPoint().y()
              );
}

void AnimationEdit::on_FramesR_currentItemChanged(QListWidgetItem *item, QListWidgetItem *)
{
    if(!item) return;
    pause();
    showFrame(
              item->data(Qt::UserRole).toPoint().x(),
              item->data(Qt::UserRole).toPoint().y()
                );
}

void AnimationEdit::on_FramesL_itemClicked(QListWidgetItem *item)
{
    on_FramesL_currentItemChanged(item, NULL);
}

void AnimationEdit::on_FramesR_itemClicked(QListWidgetItem *item)
{
    on_FramesR_currentItemChanged(item, NULL);
}

void AnimationEdit::nextFrame()
{
    if(!frames.isEmpty())
    {
        showFrame(frames[currentFrame].x, frames[currentFrame].y);
        currentFrame++;
        if(currentFrame>=frames.size())
            currentFrame=0;
    }
}

void AnimationEdit::play()
{
    if(!ui->FramesSets->currentItem()) return;

    foreach(AniFrameSet framesets, frameList.set)
    {
        if(framesets.name==ui->FramesSets->currentItem()->text())
        {
            if(direction<0)
                frames=framesets.L;
            else
                frames=framesets.R;
        break;
        }
    }
    currentFrame = 0;
    nextFrame();
    ticker.start();
}

void AnimationEdit::pause()
{
    ticker.stop();
}

void AnimationEdit::on_playLeft_clicked()
{
    direction=-1;
    play();
}

void AnimationEdit::on_playRight_clicked()
{
    direction=1;
    play();
}

void AnimationEdit::on_frameSpeed_valueChanged(int arg1)
{
    ticker.setInterval(arg1);
}
