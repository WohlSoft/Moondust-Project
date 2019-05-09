/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef WLD_SAVEIMAGE_H
#define WLD_SAVEIMAGE_H

#include <QDialog>

namespace Ui {
class WldSaveImage;
}

class WldSaveImage : public QDialog
{
    Q_OBJECT

public:
    explicit WldSaveImage(QRect sourceRect, QSize targetSize, bool proportion, QWidget *parent = 0);
    QRect getRect;
    QSize imageSize;
    bool m_keepAspectRatio = false;
    bool hidePaths = false;
    bool hideMusBoxes = false;
    bool hideGrid = false;

    ~WldSaveImage();

private slots:

    void on_imgHeight_valueChanged(int arg1);
    void on_imgWidth_valueChanged(int arg1);
    void on_SaveProportion_clicked(bool checked);
    void on_HidePaths_clicked(bool checked);
    void on_HideMusBoxes_clicked(bool checked);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_size1x_clicked();
    void on_size05x_clicked();
    void on_size2x_clicked();

private:
    bool WldSaveImage_lock;
    Ui::WldSaveImage *ui;
};

#endif // WLD_SAVEIMAGE_H
