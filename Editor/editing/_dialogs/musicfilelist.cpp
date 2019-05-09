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

#include <QDir>
#include <QDirIterator>
#include <QMessageBox>

#include <common_features/timecounter.h>

#include "musicfilelist.h"
#include <ui_musicfilelist.h>

#include <QtConcurrent>

MusicFileList::MusicFileList(QString Folder, QString current, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MusicFileList)
{
    parentFolder=Folder;
    lastCurrentFile=current;

    ui->setupUi(this);
    connect(this, &MusicFileList::itemAdded, this, &MusicFileList::addItem);
    connect(this, &MusicFileList::digFinished, this, &MusicFileList::finalizeDig);
    setCursor(Qt::BusyCursor);
    fileWalker = QtConcurrent::run(this, &MusicFileList::buildMusicList);
}

MusicFileList::~MusicFileList()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    fileWalker.waitForFinished();
    delete ui;
}

void MusicFileList::buildMusicList()
{
    QDir musicDir(parentFolder);
    QStringList filters;
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
            << "*.okt" << "*.stm" << "*.stx" << "*.ult" << "*.uni"
            //GAME EMU
            << "*.ay" << "*.gbs"<<"*.gym"<<"*.hes"<<"*.kss"<<"*.nsf"<<"*.nsfe"<<"*.sap"<<"*.spc"<<"*.vgm"<<"*.vgz";

    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);
    QDirIterator dirsList(parentFolder, filters,
                          QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);
    while(dirsList.hasNext())
    {
        dirsList.next();
        emit itemAdded(musicDir.relativeFilePath(dirsList.filePath()));
        if(fileWalker.isCanceled()) break;
    }
    digFinished();
}

void MusicFileList::addItem(QString item)
{
    ui->FileList->addItem(item);
    if(lastCurrentFile==item)
    {
        QList<QListWidgetItem*> list=ui->FileList->findItems(item, Qt::MatchFixedString);
        if(!list.isEmpty())
        {
            list.first()->setSelected(true);
            ui->FileList->scrollToItem(list.first());
        }
    }
}

void MusicFileList::finalizeDig()
{
    ui->FileList->sortItems(Qt::AscendingOrder);
    QList<QListWidgetItem *> list = ui->FileList->findItems(lastCurrentFile, Qt::MatchFixedString);
    if(!list.isEmpty())
    {
        list.first()->setSelected(true);
        ui->FileList->scrollToItem(list.first());
    }
    setCursor(Qt::ArrowCursor);
}

void MusicFileList::on_FileList_itemDoubleClicked(QListWidgetItem *item)
{
    SelectedFile = item->text();
    if(fileWalker.isRunning())
        fileWalker.cancel();
    accept();
}

void MusicFileList::on_buttonBox_accepted()
{
    foreach (QListWidgetItem * container, ui->FileList->selectedItems()) {
    SelectedFile = container->text();
    }
    if(SelectedFile!="")
    {
        if(fileWalker.isRunning())
            fileWalker.cancel();
        accept();
    }
}

void MusicFileList::on_buttonBox_rejected()
{
    if(fileWalker.isRunning())
        fileWalker.cancel();
    reject();
}
