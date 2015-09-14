/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef SAVEIMAGE_H
#define SAVEIMAGE_H

#include <QDialog>

namespace Ui {
class ExportToImage;
}

class ExportToImage : public QDialog
{
    Q_OBJECT
    
public:
    explicit ExportToImage(QVector<long > &imgSize, QWidget *parent = 0);
    QVector<long > imageSize;

    ~ExportToImage();
    bool HideWatersAndDoors();
    bool TiledBackground();
    
private slots:
    void on_imgHeight_valueChanged(int arg1);
    void on_imgWidth_valueChanged(int arg1);
    void on_SaveProportion_toggled(bool checked);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void on_size1x_clicked();
    void on_size05x_clicked();
    void on_size2x_clicked();

private:
    Ui::ExportToImage *ui;
};

#endif // SAVEIMAGE_H
