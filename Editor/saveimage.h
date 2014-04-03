/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
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
    explicit ExportToImage(QWidget *parent = 0);
    ~ExportToImage();
    
private:
    Ui::ExportToImage *ui;
};

#endif // SAVEIMAGE_H
