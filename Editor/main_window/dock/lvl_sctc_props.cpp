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
#include "../music_player.h"

static bool lockSctSettingsProps=false;

// Level Section Settings
void MainWindow::on_LVLPropsLevelWarp_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECISWARP, QVariant(checked));
        ui->actionLevWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevWarp_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECISWARP, QVariant(checked));
        ui->LVLPropsLevelWarp->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].IsWarp = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsOffScr_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        ui->actionLevOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevOffScr_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        ui->LVLPropsOffScr->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].OffScreenEn = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}



void MainWindow::on_LVLPropsNoTBack_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECNOBACK, QVariant(checked));
        ui->actionLevNoBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevNoBack_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECNOBACK, QVariant(checked));
        ui->LVLPropsNoTBack->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].noback = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}


void MainWindow::on_LVLPropsUnderWater_clicked(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECUNDERWATER, QVariant(checked));
        ui->actionLevUnderW->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevUnderW_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECUNDERWATER, QVariant(checked));
        ui->LVLPropsUnderWater->setChecked(checked);
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].underwater = checked;
        activeLvlEditWin()->LvlData.modified = true;
    }
}







///////////////////////////////////////Resize section/////////////////////////////////////
void MainWindow::on_ResizeSection_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->setFocus();
        if(activeLvlEditWin()->scene->pResizer==NULL)
        {
            activeLvlEditWin()->scene->setSectionResizer(true);
            ui->ResizeSection->setVisible(false);
            ui->applyResize->setVisible(true);
            ui->cancelResize->setVisible(true);
        }
    }
}

void MainWindow::on_applyResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, true);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
    }
}

void MainWindow::on_cancelResize_clicked()
{
    if (activeChildWindow()==1)
    {
        activeLvlEditWin()->scene->setSectionResizer(false, false);
        ui->ResizeSection->setVisible(true);
        ui->applyResize->setVisible(false);
        ui->cancelResize->setVisible(false);
    }
}


// ////////////////////////////////////////////////////////////////////////////////


void MainWindow::on_LVLPropsBackImage_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    if(configs.main_bg.size()==0)
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for backgrounds not loaded"));
        return;
    }

    //if(ui->LVLPropsBackImage->hasFocus())
    //{
    ui->LVLPropsBackImage->setEnabled(false);
    WriteToLog(QtDebugMsg, "Change BG to "+QString::number(index));
    if (activeChildWindow()==1)
    {
        QList<QVariant> backData;
        backData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].background);
        backData.push_back(ui->LVLPropsBackImage->currentData().toInt());
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECBACKGROUNDIMG, QVariant(backData));
        activeLvlEditWin()->scene->ChangeSectionBG(ui->LVLPropsBackImage->currentData().toInt());
        activeLvlEditWin()->LvlData.modified = true;
    }
    ui->LVLPropsBackImage->setEnabled(true);
//    }
//    else
//    {
//        if (activeChildWindow()==1)
//        {
//           ui->LVLPropsBackImage->setCurrentIndex(
//                       activeLvlEditWin()->LvlData.sections[
//                       activeLvlEditWin()->LvlData.CurSection].background);
//        }
//    }
}



// ////////////////Set LevelSection data//////////////////////////////////

void MainWindow::setLevelSectionData()
{
    lockSctSettingsProps=true;

    int i;
        WriteToLog(QtDebugMsg, QString("Set level Section Data"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    ui->LVLEvent_SctMus_List->clear(); //Music list in events
    ui->LVLEvent_SctBg_List->clear();  //Background list in events

    ui->LVLPropsBackImage->addItem( tr("[No image]"), "0" );
    ui->LVLEvent_SctBg_List->addItem( tr("[No image]"), "0" );
    ui->LVLPropsMusicNumber->addItem( tr("[Silence]"), "0" );
    ui->LVLEvent_SctMus_List->addItem( tr("[Silence]"), "0" );

    for(i=0; i< configs.main_bg.size();i++)
    {
        ui->LVLPropsBackImage->addItem(configs.main_bg[i].name, QString::number(configs.main_bg[i].id));
        ui->LVLEvent_SctBg_List->addItem(configs.main_bg[i].name, QString::number(configs.main_bg[i].id));
    }

    for(i=0; i< configs.main_music_lvl.size();i++)
    {
        ui->LVLPropsMusicNumber->addItem(configs.main_music_lvl[i].name, QString::number(configs.main_music_lvl[i].id) );
        ui->LVLEvent_SctMus_List->addItem(configs.main_music_lvl[i].name, QString::number(configs.main_music_lvl[i].id) );
    }

    //Set current data
    if (activeChildWindow()==1)
    {
        leveledit * edit = activeLvlEditWin();
        //edit->LvlData.sections[edit->LvlData.CurSection].background
        //edit->LvlData.sections[edit->LvlData.CurSection].music_id
        ui->LVLPropsBackImage->setCurrentIndex(0);
        for(int i=0;i<ui->LVLPropsBackImage->count();i++)
        {
            if((unsigned long)ui->LVLPropsBackImage->itemData(i).toInt() ==
                    edit->LvlData.sections[edit->LvlData.CurSection].background)
            {
                ui->LVLPropsBackImage->setCurrentIndex(i); break;
            }
        }

        ui->LVLPropsMusicNumber->setCurrentIndex(0);
        for(int i=0;i<ui->LVLPropsMusicNumber->count();i++)
        {
            if((unsigned long)ui->LVLPropsMusicNumber->itemData(i).toInt() ==
                    edit->LvlData.sections[edit->LvlData.CurSection].music_id)
            {
                ui->LVLPropsMusicNumber->setCurrentIndex(i); break;
            }
        }

        ui->LVLPropsLevelWarp->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].IsWarp);
        ui->actionLevWarp->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].IsWarp);
        ui->LVLPropsOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->actionLevOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->LVLPropsNoTBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        ui->actionLevNoBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        ui->LVLPropsUnderWater->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        ui->actionLevUnderW->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        ui->LVLPropsMusicCustom->setText(edit->LvlData.sections[edit->LvlData.CurSection].music_file);
        ui->LVLPropsMusicCustomEn->setChecked((activeLvlEditWin()->LvlData.sections[edit->LvlData.CurSection].music_id == configs.music_custom_id));
    }

    lockSctSettingsProps=false;
}


void MainWindow::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    unsigned int test = index;
//    if(ui->LVLPropsMusicNumber->hasFocus())
//    {
        ui->LVLPropsMusicCustomEn->setChecked(  test == configs.music_custom_id );
//    }

    if(activeChildWindow()==1)
    {
        QList<QVariant> musicData;
        musicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id);
        musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECMUSIC, QVariant(musicData));
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) activeLvlEditWin()->LvlData.modified = true;
    }

    WriteToLog(QtDebugMsg, "Call to Set Music if playing");
    setMusic(ui->actionPlayMusic->isChecked());
}

void MainWindow::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(lockSctSettingsProps) return;

    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( configs.music_custom_id );
            if(activeChildWindow()==1)
            {
                QList<QVariant> musicData;
                musicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_id);
                musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
                activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECMUSIC, QVariant(musicData));
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

    if(activeLvlEditWin()->isUntitled)
    {
        QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
        return;
    }

    MusicFileList musicList( dirPath, ui->LVLPropsMusicCustom->text() );
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText( musicList.SelectedFile );
    }
}

void MainWindow::on_LVLPropsMusicCustom_textChanged(const QString &arg1)
{
    if(lockSctSettingsProps) return;

    if(activeChildWindow()==1)
    {
        QList<QVariant> cusMusicData;
        cusMusicData.push_back(activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file);
        cusMusicData.push_back(arg1.simplified().remove('\"'));
        activeLvlEditWin()->scene->addChangeSectionSettingsHistory(activeLvlEditWin()->LvlData.CurSection, LvlScene::SETTING_SECCUSTOMMUSIC, QVariant(cusMusicData));
        activeLvlEditWin()->LvlData.sections[activeLvlEditWin()->LvlData.CurSection].music_file = arg1.simplified().remove('\"');
    }

    setMusic( ui->actionPlayMusic->isChecked() );
}





void MainWindow::SetCurrentLevelSection(int SctId, int open)
{
    lockSctSettingsProps=true;

    int SectionId = SctId;
    int WinType = activeChildWindow();

    WriteToLog(QtDebugMsg, "Set Current Section");
    if ((open==1)&&(WinType==1)) // Only Set Checked section number without section select
    {
        WriteToLog(QtDebugMsg, "get Current Section");
        SectionId = activeLvlEditWin()->LvlData.CurSection;
    }

    WriteToLog(QtDebugMsg, "Set checkbox to");
    ui->actionSection_1->setChecked( (SectionId==0) );
    ui->actionSection_2->setChecked( (SectionId==1) );
    ui->actionSection_3->setChecked( (SectionId==2) );
    ui->actionSection_4->setChecked( (SectionId==3) );
    ui->actionSection_5->setChecked( (SectionId==4) );
    ui->actionSection_6->setChecked( (SectionId==5) );
    ui->actionSection_7->setChecked( (SectionId==6) );
    ui->actionSection_8->setChecked( (SectionId==7) );
    ui->actionSection_9->setChecked( (SectionId==8) );
    ui->actionSection_10->setChecked( (SectionId==9) );
    ui->actionSection_11->setChecked( (SectionId==10) );
    ui->actionSection_12->setChecked( (SectionId==11) );
    ui->actionSection_13->setChecked( (SectionId==12) );
    ui->actionSection_14->setChecked( (SectionId==13) );
    ui->actionSection_15->setChecked( (SectionId==14) );
    ui->actionSection_16->setChecked( (SectionId==15) );
    ui->actionSection_17->setChecked( (SectionId==16) );
    ui->actionSection_18->setChecked( (SectionId==17) );
    ui->actionSection_19->setChecked( (SectionId==18) );
    ui->actionSection_20->setChecked( (SectionId==19) );
    ui->actionSection_21->setChecked( (SectionId==20) );

    if ((WinType==1) && (open==0))
    {
       WriteToLog(QtDebugMsg, "Call to setCurrentSection()");
       activeLvlEditWin()->setCurrentSection(SectionId);
    }

    if(WinType==1)
    {
        WriteToLog(QtDebugMsg, "Set Section Data in menu");
        //Set Section Data in menu
        ui->actionLevNoBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->actionLevOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->actionLevUnderW->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->actionLevWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text label");
        //set data in Section Settings Widget
        ui->LVLProp_CurSect->setText(QString::number(SectionId+1));

        WriteToLog(QtDebugMsg, "Set ToolBar data");
        ui->LVLPropsNoTBack->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].noback);
        ui->LVLPropsOffScr->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].OffScreenEn);
        ui->LVLPropsUnderWater->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].underwater);
        ui->LVLPropsLevelWarp->setChecked(activeLvlEditWin()->LvlData.sections[SectionId].IsWarp);

        WriteToLog(QtDebugMsg, "Set text to custom music file");
        ui->LVLPropsMusicCustom->setText(activeLvlEditWin()->LvlData.sections[SectionId].music_file);

        WriteToLog(QtDebugMsg, "Set standart Music index");
        ui->LVLPropsMusicNumber->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].music_id );

        WriteToLog(QtDebugMsg, "Set Custom music checkbox");
        ui->LVLPropsMusicCustomEn->setChecked( (activeLvlEditWin()->LvlData.sections[SectionId].music_id == configs.music_custom_id) );

        WriteToLog(QtDebugMsg, "Set background index");
        if(activeLvlEditWin()->LvlData.sections[SectionId].background < (unsigned int)ui->LVLPropsBackImage->count() )
            ui->LVLPropsBackImage->setCurrentIndex( activeLvlEditWin()->LvlData.sections[SectionId].background );
        else
            ui->LVLPropsBackImage->setCurrentIndex( ui->LVLPropsBackImage->count()-1 );

        setMusic( ui->actionPlayMusic->isChecked() );
    }

    lockSctSettingsProps=false;
}
