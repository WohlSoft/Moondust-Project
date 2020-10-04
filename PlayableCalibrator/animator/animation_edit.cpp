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

#include "animation_edit.h"
#include <ui_animation_edit.h>
#include "frame_matrix/matrix.h"

AnimationEdit::AnimationEdit(Calibration *conf, QObject *mw, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnimationEdit)
{
    m_mw = qobject_cast<CalibrationMain*>(mw);
    Q_ASSERT(m_mw);

    m_conf = conf;
    m_frameList = conf->animations;
    m_currentFrame = 0;
    m_direction = 1;
    ui->setupUi(this);
    ui->FramesSets->clear();

    for(auto &l : m_frameList)
        ui->FramesSets->addItem(l.name);

    m_ticker.connect(&m_ticker, SIGNAL(timeout()), this, SLOT(nextFrame()));
    m_ticker.setInterval(ui->frameSpeed->value());

    ui->aniToAdd->setValidator(new QRegExpValidator(QRegExp("[0-9A-Za-z]*"), ui->aniToAdd));
}

AnimationEdit::~AnimationEdit()
{
    delete ui;
}

void AnimationEdit::on_AddLeft_clicked()
{
    int x = -1;
    int y = -1;
    Matrix dialog(m_conf, m_mw, this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    if(dialog.exec() == QDialog::Accepted)
    {
        x = dialog.m_frameX;
        y = dialog.m_frameY;
        addFrameL(x, y);
        applyFrameSet();
    }
    else
        return;
}

void AnimationEdit::on_SetLeft_clicked()
{
    int x = -1;
    int y = -1;
    QList<QListWidgetItem *> selected = ui->FramesL->selectedItems();
    Matrix dialog(m_conf, m_mw, this);

    for(QListWidgetItem *item : selected)
    {
        dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
        dialog.setFrame(item->data(Qt::UserRole).toPoint().x(), item->data(Qt::UserRole).toPoint().y());

        if(dialog.exec() == QDialog::Accepted)
        {
            x = dialog.m_frameX;
            y = dialog.m_frameY;
            item->setData(Qt::UserRole, QPoint(x, y));
            item->setText(QString("X" + QString::number(x) + "-Y" + QString::number(y)));
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
    int x = -1;
    int y = -1;
    Matrix dialog(m_conf, m_mw, this);
    dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);

    if(dialog.exec() == QDialog::Accepted)
    {
        x = dialog.m_frameX;
        y = dialog.m_frameY;
        addFrameR(x, y);
        applyFrameSet();
    }
    else
        return;
}


void AnimationEdit::on_SetRight_clicked()
{
    int x = -1;
    int y = -1;
    QList<QListWidgetItem *> selected = ui->FramesR->selectedItems();
    Matrix dialog(m_conf, m_mw, this);

    for(QListWidgetItem *item : selected)
    {
        dialog.setWindowFlags(Qt::Window | Qt::WindowCloseButtonHint);
        dialog.setFrame(item->data(Qt::UserRole).toPoint().x(), item->data(Qt::UserRole).toPoint().y());

        if(dialog.exec() == QDialog::Accepted)
        {
            x = dialog.m_frameX;
            y = dialog.m_frameY;
            item->setData(Qt::UserRole, QPoint(x, y));
            item->setText(QString("X" + QString::number(x) + "-Y" + QString::number(y)));
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
    QListWidgetItem *test1 = new QListWidgetItem;
    test1->setData(Qt::UserRole, QPoint(x, y));
    test1->setText(QString("X" + QString::number(x) + "-Y" + QString::number(y)));
    ui->FramesL->addItem(test1);
}

void AnimationEdit::addFrameR(int x, int y)
{
    QListWidgetItem *test2 = new QListWidgetItem;
    test2->setData(Qt::UserRole, QPoint(x, y));
    test2->setText(QString("X" + QString::number(x) + "-Y" + QString::number(y)));
    ui->FramesR->addItem(test2);
}

void  AnimationEdit::applyFrameSet()
{
    QList<QListWidgetItem *> selected = ui->FramesSets->selectedItems();
    AniFrame frameAdd;
    QList<QListWidgetItem *> items;

    for(auto *item : selected)
    {
        if(!m_frameList.contains(item->text()))
            continue;

        auto &f = m_frameList[item->text()];

        f.L.clear();
        items = ui->FramesL->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

        for(auto *item : items)
        {
            frameAdd.x = item->data(Qt::UserRole).toPoint().x();
            frameAdd.y = item->data(Qt::UserRole).toPoint().y();
            f.L.push_back(frameAdd);
        }

        f.R.clear();
        items = ui->FramesR->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard);

        for(auto *item : items)
        {
            frameAdd.x = item->data(Qt::UserRole).toPoint().x();
            frameAdd.y = item->data(Qt::UserRole).toPoint().y();
            f.R.push_back(frameAdd);
        }
    }
}


void AnimationEdit::on_FramesSets_currentItemChanged(QListWidgetItem *item, QListWidgetItem *)
{
    if(!item)
        return;

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

    if(m_frameList.contains(item->text()))
    {
        auto &f = m_frameList[item->text()];
        for(auto &frame : f.L)
            addFrameL(frame.x, frame.y);
        for(auto &frame : f.R)
            addFrameR(frame.x, frame.y);
    }
}

void AnimationEdit::showFrame(int x, int y)
{
    qDebug() << x << y;
    int fW = m_mw->m_xImageSprite.width() / 10;
    int fH = m_mw->m_xImageSprite.height() / 10;
    m_currentFrameImg = m_mw->m_xImageSprite.copy(x * fW, y * fH, fW, fH);
    ui->img->setPixmap(m_currentFrameImg);
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
    on_FramesL_currentItemChanged(item, nullptr);
}

void AnimationEdit::on_FramesR_itemClicked(QListWidgetItem *item)
{
    on_FramesR_currentItemChanged(item, nullptr);
}

void AnimationEdit::nextFrame()
{
    if(!m_frames.isEmpty())
    {
        showFrame(m_frames[m_currentFrame].x, m_frames[m_currentFrame].y);
        m_currentFrame++;

        if(m_currentFrame >= m_frames.size())
            m_currentFrame = 0;
    }
}

void AnimationEdit::play()
{
    if(!ui->FramesSets->currentItem())
        return;

    if(m_frameList.contains(ui->FramesSets->currentItem()->text()))
    {
        auto &f = m_frameList[ui->FramesSets->currentItem()->text()];
        if(m_direction < 0)
            m_frames = f.L;
        else
            m_frames = f.R;
    }

    m_currentFrame = 0;
    nextFrame();
    m_ticker.start();
}

void AnimationEdit::pause()
{
    m_ticker.stop();
}

void AnimationEdit::on_playLeft_clicked()
{
    m_direction = -1;
    play();
}

void AnimationEdit::on_playRight_clicked()
{
    m_direction = 1;
    play();
}

void AnimationEdit::on_frameSpeed_valueChanged(int arg1)
{
    m_ticker.setInterval(arg1);
}

void AnimationEdit::on_addAni_clicked()
{
    QString t = ui->aniToAdd->text();

    if(t.isEmpty())
    {
        QMessageBox::information(this,
                                 tr("Empty animation name"),
                                 tr("Animation name is empty!"),
                                 QMessageBox::Ok);
        return;
    }

    for(auto &l : m_frameList)
    {
        if(t == l.name)
        {
            QMessageBox::information(this,
                                     tr("Animation exist"),
                                     tr("Animation with '%1' name already exist!").arg(t),
                                     QMessageBox::Ok);
            return;
        }
    }

    AniFrameSet set;
    set.name = t;
    m_frameList.insert(t, set);

    ui->FramesSets->addItem(t);
}

void AnimationEdit::on_delAni_clicked()
{
    QList<QListWidgetItem *> selected = ui->FramesSets->selectedItems();
    if(!selected.isEmpty())
    {
        ui->FramesL->setEnabled(false);
        ui->FramesR->setEnabled(false);
        ui->AddLeft->setEnabled(false);
        ui->AddRight->setEnabled(false);
        ui->SetLeft->setEnabled(false);
        ui->SetRight->setEnabled(false);
        ui->DelLeft->setEnabled(false);
        ui->DelRight->setEnabled(false);
        ui->FramesL->clear();
        ui->FramesR->clear();

        auto *w = selected.first();
        m_frameList.remove(w->text());
        ui->FramesSets->removeItemWidget(w);
        delete w;
    }
}
