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

#ifndef MUSICFILELIST_H
#define MUSICFILELIST_H

#include <QDialog>
#include <QListWidgetItem>
#include <QFuture>

namespace Ui {
class MusicFileList;
}

class MusicFileList : public QDialog
{
    Q_OBJECT

public:
    QString SelectedFile;
    explicit MusicFileList(QString Folder, QString current="", QWidget *parent = 0);
    ~MusicFileList();
    void buildMusicList();
    QString parentFolder;
    QString lastCurrentFile;

signals:
    void itemAdded(QString item);

private slots:
    void addItem(QString item);
    void on_FileList_itemDoubleClicked(QListWidgetItem *item);
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    QFuture<void> fileWalker;
    Ui::MusicFileList *ui;
};

#endif // MUSICFILELIST_H
