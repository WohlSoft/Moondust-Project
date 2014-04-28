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

#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H

#include <QDialog>
#include <QListWidgetItem>

namespace Ui {
class MusicFileList;
}

class MusicFileList : public QDialog
{
    Q_OBJECT

public:
    QString SelectedFile;
    explicit MusicFileList(QString Folder, QWidget *parent = 0);
    ~MusicFileList();

private slots:
    void on_FileList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::MusicFileList *ui;
};

#endif // MUSICFILELIST_H
