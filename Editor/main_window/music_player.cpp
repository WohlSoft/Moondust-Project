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


#include "../ui_mainwindow.h"
#include "../mainwindow.h"
#include "music_player.h"



void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    unsigned int test = index;
    if(ui->LVLPropsMusicNumber->hasFocus())
    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
    }

    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) activeLvlEditWin()->LvlData.modified = true;
    }

    WriteToLog(QtDebugMsg, "Call to Set Music if playing");
    setMusic(ui->actionPlayMusic->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                activeLvlEditWin()->LvlData.modified = true;
            }
        }
    }
}

void MainWindow::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(activeChildWindow()==1)
    {
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else return;

    MusicFileList musicList(dirPath);
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText(musicList.SelectedFile);
    }

}

void MainWindow::on_actionPlayMusic_triggered(bool checked)
{
    WriteToLog(QtDebugMsg, "Clicked play music button");
    LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
    LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentIndex();
    setMusic(checked);
}


void MainWindow::on_LVLPropsMusicCustom_textChanged(const QString &arg1)
{
    if(activeChildWindow()==1)
    {
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1;
    }

    setMusic( ui->actionPlayMusic->isChecked() );
}




void MainWindow::setMusic(bool checked)
{
    QString dirPath;
    QString musicFile;
    QString musicFilePath;
    bool silent;
    unsigned int CurMusNum;

    if(
            (configs.main_music_lvl.size()==0)||
            (configs.main_music_spc.size()==0)||
            (configs.main_music_wld.size()==0)
            )
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for music not loaded"));
        return;
    }

    if( ( LvlMusPlay::currentMusicId == ui->LVLPropsMusicNumber->currentIndex() ) &&
            (LvlMusPlay::currentCustomMusic == ui->LVLPropsMusicCustom->text()) &&
            (LvlMusPlay::musicButtonChecked == ui->actionPlayMusic->isChecked()) ) return;

    WriteToLog(QtDebugMsg, "-> New MediaPlayList");
    QMediaPlaylist * CurrentMusic = new QMediaPlaylist;


    if(activeChildWindow()!=1)
    {
        if(checked)
        {
            if(
                    (
                        (LvlMusPlay::currentMusicId>0)&&
                        (((unsigned long)LvlMusPlay::currentMusicId!=configs.music_custom_id))
                    )||
                     (
                         ((unsigned long)LvlMusPlay::currentMusicId == configs.music_custom_id)
                         &&(LvlMusPlay::currentCustomMusic!="")
                     )
              )
            {
                MusicPlayer->play();
                silent=false;
            }
            else
            {
                silent=true;
            }
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by Checked"));
                MusicPlayer->stop();
            silent=true;
        }
        return;
    }

    if(activeChildWindow()==1)
    {
        WriteToLog(QtDebugMsg, "Get Level File Path");
        dirPath = activeLvlEditWin()->LvlData.path;
    }
    else
        return;

    WriteToLog(QtDebugMsg, "Check for Sielent");

    if((ui->LVLPropsMusicNumber->currentIndex() <= 0) // Music is sielent
            ||
       //Mute, if music ID not exist
      (ui->LVLPropsMusicNumber->currentIndex()-1 > configs.main_music_lvl.size() ))
        silent=true;
    else
        silent=false;

    WriteToLog(QtDebugMsg, "ifNot Sielent, play Music");

    if(!silent)
    {
        CurMusNum = ui->LVLPropsMusicNumber->currentIndex()-1;

        if(CurMusNum==configs.music_custom_id-1)
        {
                WriteToLog(QtDebugMsg, QString("get Custom music path"));
            musicFile = ui->LVLPropsMusicCustom->text();
            musicFilePath = dirPath + "/" + musicFile;
        }
        else
        {
                WriteToLog(QtDebugMsg, QString("get standart music path"));
            musicFile = configs.main_music_lvl[CurMusNum].file;
            dirPath = configs.dirs.music;
            musicFilePath = configs.dirs.music + musicFile;
            //QMessageBox::information(this, "test", "music is \n"+musicFile+"\n"+QString::number());
        }

        if(checked)
        {
            if( (QFile::exists(musicFilePath)) && (QFileInfo(musicFilePath)).isFile() )
            {
                WriteToLog(QtDebugMsg, QString("Set music player -> addMedia"));
                CurrentMusic->addMedia(QUrl::fromLocalFile( musicFilePath ));
                CurrentMusic->setPlaybackMode(QMediaPlaylist::Loop);
                WriteToLog(QtDebugMsg, QString("Set music player -> stop Current"));
                MusicPlayer->stop();
                WriteToLog(QtDebugMsg, QString("Set music player -> set PlayList"));
                MusicPlayer->setPlaylist(CurrentMusic);
                WriteToLog(QtDebugMsg, QString("Set music player -> setVolme and play"));
                MusicPlayer->setVolume(100);
                MusicPlayer->play();
            }
            else
                MusicPlayer->stop();
        }
        else
        {
            WriteToLog(QtDebugMsg, QString("Set music player -> Stop by checker"));
            MusicPlayer->stop();
        }
    }
    else
    {
        WriteToLog(QtDebugMsg, QString("Set music player -> Stop by sielent"));
        MusicPlayer->stop();
    }

    LvlMusPlay::currentCustomMusic = ui->LVLPropsMusicCustom->text();
    LvlMusPlay::currentMusicId = ui->LVLPropsMusicNumber->currentIndex();
    LvlMusPlay::musicButtonChecked  = ui->actionPlayMusic->isChecked();
}

