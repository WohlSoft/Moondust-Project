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

#include <QDir>
#include <QDirIterator>
#include <QMessageBox>

#include <common_features/timecounter.h>

#include "musicfilelist.h"
#include <ui_musicfilelist.h>


MusicFileList::MusicFileList(QString Folder, QString current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicFileList)
{
    QStringList filters;
    QDir musicDir(Folder);
    filters //MPEG 1 Layer III (LibMAD)
            << "*.mp3"
            //OGG Vorbis and FLAC (LibOGG, LibVorbis, LibFLAC)
            << "*.ogg" << "*.flac"
            //Uncompressed audio data
            << "*.wav" << "*.voc" << "*.aiff"
            //MIDI
            << "*.mid"
            //MikMod (Modules)
            << "*.mod" << "*.it" << "*.s3m" << "*.669" << "*.med" << "*.xm" << "*.amf"
            << "*.apun" << "*.dsm" << "*.far" << "*.gdm" << "*.imf" << "*.mtm"
            << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni";

    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);

    QStringList fileList;
    //fileList << Folder;
    QDirIterator dirsList(Folder, filters,
                          QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

    TimeCounter limiter;
    limiter.start();
    while(dirsList.hasNext())
      {
            dirsList.next();
            fileList << musicDir.relativeFilePath(dirsList.filePath());
            if(limiter.current()>1500)
            {
              QMessageBox::StandardButton reply;
              reply = QMessageBox::question(this, tr("Too many subfolders"), tr("If you contunue this operation, application can be frozen.\nDo you want to continue?"),
                                            QMessageBox::Yes|QMessageBox::Abort);
              if (reply == QMessageBox::Abort)
                  break;
            }
      }
    limiter.stop("", -1);

    ui->setupUi(this);
    //ui->FileList->insertItems(musicDir.entryList().size(), musicDir.entryList(filters));
    ui->FileList->insertItems(fileList.size(), fileList);

    // Select current item
    for(int i=0; i<ui->FileList->count(); i++)
    {
        if(ui->FileList->item(i)->text()==current)
        {
            ui->FileList->item(i)->setSelected(true);
            ui->FileList->scrollToItem(ui->FileList->item(i));
         break;
        }
    }

}

MusicFileList::~MusicFileList()
{
    delete ui;
}

void MusicFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    accept();
}

void MusicFileList::on_buttonBox_accepted()
{
    foreach (QListWidgetItem * container, ui->FileList->selectedItems()) {
    SelectedFile = container->text();
    }
    if(SelectedFile!="")
        accept();
}

void MusicFileList::on_buttonBox_rejected()
{
    reject();
}
