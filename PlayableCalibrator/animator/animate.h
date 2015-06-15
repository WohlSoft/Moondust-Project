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

#ifndef ANIMATE_H
#define ANIMATE_H

#include <QDialog>
#include "calibrationmain.h"
#include "AnimationScene.h"
#include "aniFrames.h"

namespace Ui {
class Animate;
}

class Animate : public QDialog
{
    Q_OBJECT
    
public:
    explicit Animate(QWidget *parent = 0);
    ~Animate();
    AnimationScene *AniScene;

    QString aniStyle;
    int aniDir;
    void aniFindSet();
protected:
    void keyPressEvent(QKeyEvent *e);
private slots:
    void on_EditAnimationBtn_clicked();

    void on_directLeft_clicked();
    void on_directRight_clicked();

    void on_FrameSpeed_valueChanged(int arg1);
    void on_animationsList_itemClicked(QListWidgetItem *item);
    void on_animationsList_currentItemChanged(QListWidgetItem *current, QListWidgetItem *);

private:
    Ui::Animate *ui;
};

#endif // ANIMATE_H
