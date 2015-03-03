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

#include <audio/music_player.h>
#include <editing/_dialogs/musicfilelist.h>
#include <main_window/dock/lvl_events_box.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_sctc_props.h"
#include "ui_lvl_sctc_props.h"

LvlSectionProps::LvlSectionProps(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlSectionProps)
{
    setVisible(false);
    ui->setupUi(this);

    switchResizeMode(false);

    lockSctSettingsProps=false;

    QRect mwg = mw()->geometry();
    int GOffset=240;
    mw()->addDockWidget(Qt::LeftDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setGeometry(
                mwg.x()+mwg.width()-width()-GOffset,
                mwg.y()+120,
                width(),
                height()
                );

    connect(mw()->ui->ResizingToolbar, SIGNAL(visibilityChanged(bool)),
                   this, SLOT(switchResizeMode(bool)));

    mw()->docks_level.
          addState(this, &GlobalSettings::LevelSectionBoxVis);
}


LvlSectionProps::~LvlSectionProps()
{
    delete ui;
}

void LvlSectionProps::re_translate()
{
    ui->retranslateUi(this);
    setLevelSectionData();
}

void LvlSectionProps::focusInEvent(QFocusEvent *ev)
{
    QDockWidget::focusInEvent(ev);
    //ev->accept();
    //qApp->setActiveWindow(mw());
}

// Level Section tool box show/hide
void LvlSectionProps::on_LvlSectionProps_visibilityChanged(bool visible)
{
    mw()->ui->actionSection_Settings->setChecked(visible);
}

void MainWindow::on_actionSection_Settings_triggered(bool checked)
{
    dock_LvlSectionProps->setVisible(checked);
    if(checked) dock_LvlSectionProps->raise();
}


// ////////////////Set LevelSection data//////////////////////////////////
void LvlSectionProps::setLevelSectionData()
{
    lockSctSettingsProps=true;

    int i;
        WriteToLog(QtDebugMsg, QString("Set level Section Data"));
    ui->LVLPropsBackImage->clear();
    ui->LVLPropsMusicNumber->clear();

    mw()->dock_LvlEvents->cbox_sct_mus()->clear(); //Music list in events
    mw()->dock_LvlEvents->cbox_sct_bg()->clear();  //Background list in events

    QPixmap empty(100,70);
    empty.fill(QColor(Qt::black));

    ui->LVLPropsBackImage->addItem(QIcon(empty), tr("[No image]"), "0" );
    mw()->dock_LvlEvents->cbox_sct_bg()->addItem(QIcon(empty), tr("[No image]"), "0" );
    ui->LVLPropsMusicNumber->addItem( tr("[Silence]"), "0" );
    mw()->dock_LvlEvents->cbox_sct_mus()->addItem( tr("[Silence]"), "0" );

    ui->LVLPropsBackImage->setIconSize(QSize(100,70));
    mw()->dock_LvlEvents->cbox_sct_bg()->setIconSize(QSize(100,70));

    QAbstractItemView *abVw = ui->LVLPropsBackImage->view();
            QListView *listVw = qobject_cast<QListView*>(abVw);
            if (listVw) {
                listVw->setSpacing(2);
                listVw->setViewMode(QListView::IconMode);
                listVw->setUniformItemSizes(true);
            }

    abVw = mw()->dock_LvlEvents->cbox_sct_bg()->view();
            listVw = qobject_cast<QListView*>(abVw);
            if (listVw) {
                listVw->setSpacing(2);
                listVw->setViewMode(QListView::IconMode);
                listVw->setUniformItemSizes(true);
            }


    for(i=0; i< mw()->configs.main_bg.size();i++)
    {
        QPixmap bgThumb(100,70);
        bgThumb.fill(QColor(Qt::white));
        QPainter xx(&bgThumb);

        QPixmap tmp;
        tmp = mw()->configs.main_bg[i].image.scaledToHeight(70);

        if (mw()->activeChildWindow()==1)
        {
            LevelEdit * edit = mw()->activeLvlEditWin();
            for(int q=0; q<edit->scene->uBGs.size();q++)
            {
                if(edit->scene->uBGs[q].id==mw()->configs.main_bg[i].id)
                {
                    if(!edit->scene->uBGs[q].image.isNull())
                        tmp = edit->scene->uBGs[q].image.scaledToHeight(70);
                    break;
                }
            }
        }

        if(!tmp.isNull())
        {
            int d=0;
            for(int i=0; i<100; i+=tmp.width() )
            {
                xx.drawPixmap(i,0, tmp.width(), tmp.height(), tmp);
                d+=tmp.width();
            }
            if(d<100)
            {
                xx.drawPixmap(d,0, tmp.width()-(100-d), tmp.height(), tmp);
            }
        }
        xx.end();

        ui->LVLPropsBackImage->addItem(QIcon(bgThumb), mw()->configs.main_bg[i].name, QString::number(mw()->configs.main_bg[i].id));
        mw()->dock_LvlEvents->cbox_sct_bg()->addItem(QIcon(bgThumb), mw()->configs.main_bg[i].name, QString::number(mw()->configs.main_bg[i].id));
    }

    for(i=0; i< mw()->configs.main_music_lvl.size();i++)
    {
        ui->LVLPropsMusicNumber->addItem(mw()->configs.main_music_lvl[i].name, QString::number(mw()->configs.main_music_lvl[i].id) );
        mw()->dock_LvlEvents->cbox_sct_mus()->addItem(mw()->configs.main_music_lvl[i].name, QString::number(mw()->configs.main_music_lvl[i].id) );
    }

    //Set current data
    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) {lockSctSettingsProps=false; return;}
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
        mw()->ui->actionLevWarp->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].IsWarp);
        ui->LVLPropsOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        mw()->ui->actionLevOffScr->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn);
        ui->LVLPropsNoTBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        mw()->ui->actionLevNoBack->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].noback);
        ui->LVLPropsUnderWater->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        mw()->ui->actionLevUnderW->setChecked(edit->LvlData.sections[edit->LvlData.CurSection].underwater);
        ui->LVLPropsMusicCustom->setText(edit->LvlData.sections[edit->LvlData.CurSection].music_file);
        ui->LVLPropsMusicCustomEn->setChecked((edit->LvlData.sections[edit->LvlData.CurSection].music_id == mw()->configs.music_custom_id));
    }

    lockSctSettingsProps=false;
}





// Level Section Settings
void LvlSectionProps::on_LVLPropsLevelWarp_clicked(bool checked)
{
    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECISWARP, QVariant(checked));
        mw()->ui->actionLevWarp->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].IsWarp = checked;
        edit->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevWarp_triggered(bool checked)
{
    if(activeChildWindow()==1)
    {
        LevelEdit * edit = activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECISWARP, QVariant(checked));
        dock_LvlSectionProps->ui->LVLPropsLevelWarp->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].IsWarp = checked;
        edit->LvlData.modified = true;
    }
}


void LvlSectionProps::on_LVLPropsOffScr_clicked(bool checked)
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        mw()->ui->actionLevOffScr->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn = checked;
        edit->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevOffScr_triggered(bool checked)
{
    if(activeChildWindow()==1)
    {
        LevelEdit * edit = activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECOFFSCREENEXIT, QVariant(checked));
        dock_LvlSectionProps->ui->LVLPropsOffScr->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].OffScreenEn = checked;
        edit->LvlData.modified = true;
    }
}



void LvlSectionProps::on_LVLPropsNoTBack_clicked(bool checked)
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECNOBACK, QVariant(checked));
        mw()->ui->actionLevNoBack->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].noback = checked;
        edit->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevNoBack_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        LevelEdit * edit = activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECNOBACK, QVariant(checked));
        dock_LvlSectionProps->ui->LVLPropsNoTBack->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].noback = checked;
        edit->LvlData.modified = true;
    }
}


void LvlSectionProps::on_LVLPropsUnderWater_clicked(bool checked)
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECUNDERWATER, QVariant(checked));
        mw()->ui->actionLevUnderW->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].underwater = checked;
        edit->LvlData.modified = true;
    }
}

void MainWindow::on_actionLevUnderW_triggered(bool checked)
{
    if (activeChildWindow()==1)
    {
        LevelEdit * edit = activeLvlEditWin();
        if(!edit) return;
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECUNDERWATER, QVariant(checked));
        dock_LvlSectionProps->ui->LVLPropsUnderWater->setChecked(checked);
        edit->LvlData.sections[edit->LvlData.CurSection].underwater = checked;
        edit->LvlData.modified = true;
    }
}







///////////////////////////////////////Resize section/////////////////////////////////////
void LvlSectionProps::on_ResizeSection_clicked()
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        qApp->setActiveWindow(mw());
        edit->setFocus();
        if(edit->scene->pResizer==NULL)
        {
            edit->scene->setSectionResizer(true);
        }
    }
}

void LvlSectionProps::on_applyResize_clicked()
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        mw()->on_actionResizeApply_triggered();
    }
}

void LvlSectionProps::on_cancelResize_clicked()
{
    if (mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        mw()->on_actionResizeCancel_triggered();
    }
}


// ////////////////////////////////////////////////////////////////////////////////
void LvlSectionProps::on_LVLPropsBackImage_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    if(mw()->configs.main_bg.size()==0)
    {
        WriteToLog(QtCriticalMsg, QString("Error! *.INI Configs for backgrounds not loaded"));
        return;
    }

    //if(ui->LVLPropsBackImage->hasFocus())
    //{
    ui->LVLPropsBackImage->setEnabled(false);
    WriteToLog(QtDebugMsg, "Change BG to "+QString::number(index));
    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        QList<QVariant> backData;
        backData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].background);
        backData.push_back(ui->LVLPropsBackImage->currentData().toInt());
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECBACKGROUNDIMG, QVariant(backData));
        edit->scene->ChangeSectionBG(ui->LVLPropsBackImage->currentData().toInt());
        edit->LvlData.modified = true;
    }
    ui->LVLPropsBackImage->setEnabled(true);
}

void LvlSectionProps::switchResizeMode(bool mode)
{
    ui->applyResize->setVisible(mode);
    ui->cancelResize->setVisible(mode);
    ui->ResizeSection->setVisible(!mode);
}

void LvlSectionProps::loadMusic()
{
    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        LvlMusPlay::setMusic(LvlMusPlay::LevelMusic,
                    edit->LvlData.sections[edit->LvlData.CurSection].music_id,
                    edit->LvlData.sections[edit->LvlData.CurSection].music_file);
    }
    mw()->setMusic(mw()->ui->actionPlayMusic->isChecked());
}


void LvlSectionProps::on_LVLPropsMusicNumber_currentIndexChanged(int index)
{
    if(lockSctSettingsProps) return;

    unsigned int test = index;
    ui->LVLPropsMusicCustomEn->setChecked(  test == mw()->configs.music_custom_id );

    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;

        QList<QVariant> musicData;
        musicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_id);
        musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECMUSIC, QVariant(musicData));
        edit->LvlData.sections[edit->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
        if(ui->LVLPropsMusicNumber->hasFocus()) edit->LvlData.modified = true;
    }
    loadMusic();
}

void LvlSectionProps::on_LVLPropsMusicCustomEn_toggled(bool checked)
{
    if(lockSctSettingsProps) return;

    if(ui->LVLPropsMusicCustomEn->hasFocus())
    {
        if(checked)
        {
            ui->LVLPropsMusicNumber->setCurrentIndex( mw()->configs.music_custom_id );
            if(mw()->activeChildWindow()==1)
            {
                LevelEdit * edit = mw()->activeLvlEditWin();
                if(!edit) return;

                QList<QVariant> musicData;
                musicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_id);
                musicData.push_back(ui->LVLPropsMusicNumber->currentIndex());
                edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECMUSIC, QVariant(musicData));
                edit->LvlData.sections[edit->LvlData.CurSection].music_id = ui->LVLPropsMusicNumber->currentIndex();
                edit->LvlData.modified = true;
            }
        }
    }
}

void LvlSectionProps::on_LVLPropsMusicCustomBrowse_clicked()
{
    QString dirPath;
    if(mw()->activeChildWindow()!=1) return;

    LevelEdit * edit = mw()->activeLvlEditWin();
    if(!edit) return;

    dirPath = edit->LvlData.path;

    if(edit->isUntitled)
    {
        QMessageBox::information(this, tr("Please, save file"), tr("Please, save file first, if you want to select custom music file."), QMessageBox::Ok);
        return;
    }

    MusicFileList musicList( dirPath, ui->LVLPropsMusicCustom->text() );
    if( musicList.exec() == QDialog::Accepted )
    {
        ui->LVLPropsMusicCustom->setText( musicList.SelectedFile );
        ui->LVLPropsMusicCustom->setModified(true);
        on_LVLPropsMusicCustom_editingFinished();
    }
}


void LvlSectionProps::on_LVLPropsMusicCustom_editingFinished()//_textChanged(const QString &arg1)
{
    if(lockSctSettingsProps) return;
    if(!ui->LVLPropsMusicCustom->isModified()) return;
    ui->LVLPropsMusicCustom->setModified(false);

    if(mw()->activeChildWindow()==1)
    {
        LevelEdit * edit = mw()->activeLvlEditWin();
        if(!edit) return;
        QString arg1 = ui->LVLPropsMusicCustom->text();

        QList<QVariant> cusMusicData;
        cusMusicData.push_back(edit->LvlData.sections[edit->LvlData.CurSection].music_file);
        cusMusicData.push_back(arg1.simplified().remove('\"'));
        edit->scene->addChangeSectionSettingsHistory(edit->LvlData.CurSection, HistorySettings::SETTING_SECCUSTOMMUSIC, QVariant(cusMusicData));
        edit->LvlData.sections[edit->LvlData.CurSection].music_file = arg1.simplified().remove('\"');
    }

    loadMusic();
}
