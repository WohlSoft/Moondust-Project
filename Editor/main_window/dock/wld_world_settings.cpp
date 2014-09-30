/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../../ui_mainwindow.h"
#include "../../mainwindow.h"

#include "../../common_features/levelfilelist.h"
#include "../../file_formats/file_formats.h"

bool world_settings_lock_fields=false;
QMap<QCheckBox *, int> WLD_CharacterCheckBoxes;


void MainWindow::setCurrentWorldSettings()
{
    world_settings_lock_fields=true;
    int WinType = activeChildWindow();
    //QCheckBox *

    WriteToLog(QtDebugMsg, "-> Set Worldmap settings");
    if(WinType==3)
    {
        WorldEdit * edit = activeWldEditWin();

        WriteToLog(QtDebugMsg, "-> setTitle");
        ui->WLD_Title->setText( edit->WldData.EpisodeTitle );

        WriteToLog(QtDebugMsg, "-> setText");
        ui->WLD_AutostartLvl->setText( edit->WldData.IntroLevel_file );

        ui->WLD_Stars->setValue( edit->WldData.stars );

        ui->WLD_NoWorldMap->setChecked( edit->WldData.HubStyledWorld );
        ui->actionWLDDisableMap->setChecked( edit->WldData.HubStyledWorld );
        ui->WLD_RestartLevel->setChecked( edit->WldData.restartlevel );
        ui->actionWLDFailRestart->setChecked( edit->WldData.restartlevel );

        ui->WLD_Credirs->setText( edit->WldData.authors );

        WriteToLog(QtDebugMsg, "-> Character List");
        //clear character list
        while(!WLD_CharacterCheckBoxes.isEmpty())
        {
            QMap<QCheckBox *, int>::iterator it = WLD_CharacterCheckBoxes.begin();
            delete it.key();
            WLD_CharacterCheckBoxes.erase(it);
        }

        WriteToLog(QtDebugMsg, "-> Clear Menu");
        //ui->WLD_DisableCharacters

        //Create absence data
        if(edit->WldData.nocharacter.size() < configs.characters.size())
        {
            for(int i=0; i<=configs.characters.size()-edit->WldData.nocharacter.size(); i++ )
                edit->WldData.nocharacter.push_back(false);
        }

        for(int i = 0; i < configs.characters.size(); ++i){
            QCheckBox* cur = new QCheckBox(configs.characters[i].name);
            if(i < edit->WldData.nocharacter.size()){
                cur->setChecked(edit->WldData.nocharacter[i]);
            }
            WLD_CharacterCheckBoxes[cur] = configs.characters[i].id;
            connect(cur, SIGNAL(clicked(bool)), this, SLOT(characterActivated(bool)));
            ui->WLD_DisableCharacters->layout()->addWidget(cur);
        }

        WriteToLog(QtDebugMsg, "-> Done");
    }
    world_settings_lock_fields=false;
}


void MainWindow::characterActivated(bool checked)
{
    if(activeChildWindow()==3){
        QCheckBox* ch = qobject_cast<QCheckBox*>(sender());
        if(!ch)
            return;
        int ind = configs.getCharacterI(WLD_CharacterCheckBoxes[ch]);
        if(ind==-1)
            return;
        QList<QVariant> chData;
        chData << WLD_CharacterCheckBoxes[ch] << checked;
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_CHARACTER, chData);
        activeWldEditWin()->WldData.nocharacter[ind] = checked;
    }
}



void MainWindow::on_actionWLDProperties_triggered()
{
    ui->actionWorld_settings->trigger();
}

void MainWindow::on_WLD_Title_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_Title->isModified()) return;
    ui->WLD_Title->setModified(false);

    if (activeChildWindow()==3)
    {
        QList<QVariant> var;
        var << activeWldEditWin()->WldData.EpisodeTitle << ui->WLD_Title->text();
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_WORLDTITLE, var);
        activeWldEditWin()->WldData.EpisodeTitle = ui->WLD_Title->text();
        activeWldEditWin()->WldData.modified = true;
        activeWldEditWin()->setWindowTitle(ui->WLD_Title->text() =="" ? activeWldEditWin()->userFriendlyCurrentFile() : ui->WLD_Title->text() );
    }
}

//void MainWindow::on_WLD_Title_textChanged(const QString &arg1)
//{


//}

void MainWindow::on_WLD_NoWorldMap_clicked(bool checked)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_HUB, checked);

        ui->actionWLDDisableMap->setChecked(checked);
        activeWldEditWin()->WldData.HubStyledWorld = checked;
        activeWldEditWin()->WldData.modified = true;
    }
}

void MainWindow::on_actionWLDDisableMap_triggered(bool checked)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_HUB, checked);

        ui->WLD_NoWorldMap->setChecked(checked);
        activeWldEditWin()->WldData.HubStyledWorld = checked;
        activeWldEditWin()->WldData.modified = true;
    }
}


void MainWindow::on_WLD_RestartLevel_clicked(bool checked)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_RESTARTAFTERFAIL, checked);

        ui->actionWLDFailRestart->setChecked(checked);
        activeWldEditWin()->WldData.restartlevel = checked;
        activeWldEditWin()->WldData.modified = true;
    }
}
void MainWindow::on_actionWLDFailRestart_triggered(bool checked)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_RESTARTAFTERFAIL, checked);

        ui->WLD_RestartLevel->setChecked(checked);
        activeWldEditWin()->WldData.restartlevel = checked;
        activeWldEditWin()->WldData.modified = true;
    }
}

/*
void MainWindow::on_WLD_AutostartLvl_textEdited(const QString &arg1)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        activeWldEditWin()->WldData.autolevel = arg1;
        activeWldEditWin()->WldData.modified = true;
    }
}
*/


void MainWindow::on_WLD_AutostartLvl_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_AutostartLvl->isModified()) return;
    ui->WLD_AutostartLvl->setModified(false);

    if (activeChildWindow()==3)
    {
        QList<QVariant> var;
        var << activeWldEditWin()->WldData.IntroLevel_file << ui->WLD_AutostartLvl->text();
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_INTROLEVEL, var);
        activeWldEditWin()->WldData.IntroLevel_file = ui->WLD_AutostartLvl->text();
        activeWldEditWin()->WldData.modified = true;
    }
}


void MainWindow::on_WLD_AutostartLvlBrowse_clicked()
{
    if(world_settings_lock_fields) return;

    QString dirPath;
    if(activeChildWindow()==3)
    {
        dirPath = activeWldEditWin()->WldData.path;
    }
    else
        return;

    LevelFileList levelList(dirPath, ui->WLD_AutostartLvl->text());
    if( levelList.exec() == QDialog::Accepted )
    {
        if(activeChildWindow()==3){
            ui->WLD_AutostartLvl->setText(levelList.SelectedFile);
            ui->WLD_AutostartLvl->setModified(true);
            on_WLD_AutostartLvl_editingFinished();
        }
    }

}

void MainWindow::on_WLD_Stars_valueChanged(int arg1)
{
    if(world_settings_lock_fields) return;
    if (activeChildWindow()==3)
    {
        QList<QVariant> var;
        var << activeWldEditWin()->WldData.stars << arg1;
        activeWldEditWin()->scene->addChangeWorldSettingsHistory(WldScene::SETTING_TOTALSTARS, var);
        activeWldEditWin()->WldData.stars = arg1;
        activeWldEditWin()->WldData.modified = true;
    }

}

void MainWindow::on_WLD_DoCountStars_clicked()
{
    if(world_settings_lock_fields) return;

    //Count stars of all used levels on this world map

    QString dirPath;
    long starzzz=0;

    if (activeChildWindow()==3)
    {
        WorldEdit * edit = activeWldEditWin();
        dirPath = edit->WldData.path;

        QRegExp lvlext = QRegExp(".*\\.(lvl|lvlx)$");
        //lvlext.setPatternSyntax(QRegExp::RegExp);
        lvlext.setCaseSensitivity(Qt::CaseInsensitive);

        QProgressDialog progress(tr("Counting stars of placed levels"), tr("Abort"), 0, edit->WldData.levels.size(), this);
             progress.setWindowTitle(tr("Counting stars..."));
             progress.setWindowModality(Qt::WindowModal);
             progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
             progress.setFixedSize(progress.size());
             progress.setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, progress.size(), qApp->desktop()->availableGeometry()));
             progress.setMinimumDuration(0);

        for(int i=0;i<edit->WldData.levels.size();i++)
        {
            //Attempt to read stars quantity of level:

            if(edit->WldData.levels[i].lvlfile.isEmpty()) continue;
            QString FilePath = dirPath+"/"+edit->WldData.levels[i].lvlfile;
            if(!QFile(FilePath).exists()) continue;

            QFile file(FilePath);
            if (!file.open(QIODevice::ReadOnly)) continue;
            QFileInfo in_1(FilePath);

            LevelData getLevelHead;
            getLevelHead.stars = 0;
            if( lvlext.exactMatch(FilePath) )
            {
                if(in_1.suffix().toLower() == "lvl")
                    getLevelHead = FileFormats::ReadLevelFile(file);         //Read SMBX LVL File
                else
                    getLevelHead = FileFormats::ReadExtendedLevelFile(file); //Read PGE LVLX File
                //getLevelHead = FileFormats::ReadLevelFile(file); //function in file_formats.cpp
                if( !getLevelHead.ReadFileValid ) continue;
            }
            file.close();
            starzzz+=getLevelHead.stars;

            if(progress.wasCanceled()) break;
            progress.setValue(i);
            qApp->processEvents();
        }

        if(progress.wasCanceled()) return;
        ui->WLD_Stars->setValue(starzzz);
        progress.close();
    }
}

void MainWindow::on_WLD_Credirs_textChanged()
{
    if(world_settings_lock_fields) return;

    if (activeChildWindow()==3)
    {
        activeWldEditWin()->WldData.authors = ui->WLD_Credirs->toPlainText();
        activeWldEditWin()->WldData.modified = true;
    }
}
