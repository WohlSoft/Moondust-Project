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

#include <QtConcurrent>
#include <QDesktopWidget>

#include <editing/_dialogs/levelfilelist.h>
#include <editing/_scenes/world/wld_history_manager.h>
#include <PGE_File_Formats/file_formats.h>
#include <common_features/main_window_ptr.h>

#include <tools/async/asyncstarcounter.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_settings_box.h"
#include "ui_wld_settings_box.h"

#include <tuple>

WorldSettingsBox::WorldSettingsBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WorldSettingsBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    world_settings_lock_fields = false;
    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );
    m_lastVisibilityState = isVisible();
    mw()->docks_world.
    addState(this, &m_lastVisibilityState);
    updateLevelIntroLabel();
    connect(ui->WLD_NoWorldMap, &QCheckBox::toggled, [this](bool)
    {
        updateLevelIntroLabel();
    });
}

WorldSettingsBox::~WorldSettingsBox()
{
    delete ui;
}

void WorldSettingsBox::re_translate()
{
    ui->retranslateUi(this);
    setCurrentWorldSettings();
    updateLevelIntroLabel();
}




void WorldSettingsBox::on_WorldSettingsBox_visibilityChanged(bool visible)
{
    mw()->ui->actionWorld_settings->setChecked(visible);
}


void MainWindow::on_actionWorld_settings_triggered(bool checked)
{
    dock_WldSettingsBox->setVisible(checked);

    if(checked) dock_WldSettingsBox->raise();
}



void WorldSettingsBox::setCurrentWorldSettings()
{
    world_settings_lock_fields = true;
    int WinType = mw()->activeChildWindow();

    if(WinType == 3)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        LogDebug("-> Set Worldmap settings");
        LogDebug("-> setTitle");
        ui->WLD_Title->setText(edit->WldData.EpisodeTitle);
        LogDebug("-> setText");
        ui->WLD_AutostartLvl->setText(edit->WldData.IntroLevel_file);
        ui->WLD_Stars->setValue(static_cast<int>(edit->WldData.stars));
        ui->WLD_NoWorldMap->setChecked(edit->WldData.HubStyledWorld);
        mw()->ui->actionWLDDisableMap->setChecked(edit->WldData.HubStyledWorld);
        ui->WLD_RestartLevel->setChecked(edit->WldData.restartlevel);
        mw()->ui->actionWLDFailRestart->setChecked(edit->WldData.restartlevel);
        ui->WLD_Credirs->setText(edit->WldData.authors);
        LogDebug("-> Character List");

        //clear character list
        while(!WLD_CharacterCheckBoxes.isEmpty())
        {
            QMap<QCheckBox *, int>::iterator it = WLD_CharacterCheckBoxes.begin();
            delete it.key();
            WLD_CharacterCheckBoxes.erase(it);
        }

        LogDebug("-> Clear Menu");
        //ui->WLD_DisableCharacters

        //Create absence data
        if(edit->WldData.nocharacter.size() < mw()->configs.main_characters.size())
        {
            for(int i = 0; i <= mw()->configs.main_characters.size() - edit->WldData.nocharacter.size(); i++)
                edit->WldData.nocharacter.push_back(false);
        }

        for(int i = 0; i < mw()->configs.main_characters.size(); ++i)
        {
            QCheckBox *cur = new QCheckBox(mw()->configs.main_characters[i].name);

            if(i < edit->WldData.nocharacter.size())
                cur->setChecked(edit->WldData.nocharacter[i]);

            WLD_CharacterCheckBoxes[cur] = static_cast<int>(mw()->configs.main_characters[i].id);
            connect(cur, SIGNAL(clicked(bool)), this, SLOT(characterActivated(bool)));
            ui->WLD_DisableCharacters->layout()->addWidget(cur);
        }

        LogDebug("-> Done");
    }

    world_settings_lock_fields = false;
}


void WorldSettingsBox::characterActivated(bool checked)
{
    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        QCheckBox *ch = qobject_cast<QCheckBox *>(sender());

        if(!ch)
            return;

        long ind = mw()->configs.getCharacterI(static_cast<unsigned long>(WLD_CharacterCheckBoxes[ch]));

        if(ind == -1)
            return;

        QList<QVariant> chData;
        chData << WLD_CharacterCheckBoxes[ch] << checked;
        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_CHARACTER, chData);
        edit->WldData.nocharacter[static_cast<int>(ind)] = checked;
    }
}


void MainWindow::on_actionWLDProperties_triggered()
{
    ui->actionWorld_settings->trigger();
}

void WorldSettingsBox::on_WLD_Title_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_Title->isModified()) return;

    ui->WLD_Title->setModified(false);

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        QList<QVariant> var;
        var << edit->WldData.EpisodeTitle << ui->WLD_Title->text();
        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_WORLDTITLE, var);
        edit->WldData.EpisodeTitle = ui->WLD_Title->text();
        edit->WldData.meta.modified = true;
        edit->setWindowTitle(ui->WLD_Title->text() == "" ? edit->userFriendlyCurrentFile() : ui->WLD_Title->text());
    }
}

void WorldSettingsBox::updateLevelIntroLabel()
{
    ui->introLevelLabel->setText(ui->WLD_NoWorldMap->isChecked() ? tr("Main hub level:") : tr("Intro level:"));
}

void WorldSettingsBox::on_WLD_NoWorldMap_clicked(bool checked)
{
    if(world_settings_lock_fields) return;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_HUB, checked);
        mw()->ui->actionWLDDisableMap->setChecked(checked);
        edit->WldData.HubStyledWorld = checked;
        edit->WldData.meta.modified = true;
    }
}

void MainWindow::on_actionWLDDisableMap_triggered(bool checked)
{
    if(dock_WldSettingsBox->world_settings_lock_fields) return;

    if(activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        if(!edit) return;

        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_HUB, checked);
        dock_WldSettingsBox->ui->WLD_NoWorldMap->setChecked(checked);
        edit->WldData.HubStyledWorld = checked;
        edit->WldData.meta.modified = true;
    }
}


void WorldSettingsBox::on_WLD_RestartLevel_clicked(bool checked)
{
    if(world_settings_lock_fields) return;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_RESTARTAFTERFAIL, checked);
        mw()->ui->actionWLDFailRestart->setChecked(checked);
        edit->WldData.restartlevel = checked;
        edit->WldData.meta.modified = true;
    }
}
void MainWindow::on_actionWLDFailRestart_triggered(bool checked)
{
    if(dock_WldSettingsBox->world_settings_lock_fields) return;

    if(activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = activeWldEditWin();

        if(!edit) return;

        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_RESTARTAFTERFAIL, checked);
        dock_WldSettingsBox->ui->WLD_RestartLevel->setChecked(checked);
        edit->WldData.restartlevel = checked;
        edit->WldData.meta.modified = true;
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


void WorldSettingsBox::on_WLD_AutostartLvl_editingFinished()
{
    if(world_settings_lock_fields) return;

    if(!ui->WLD_AutostartLvl->isModified()) return;

    ui->WLD_AutostartLvl->setModified(false);

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        QList<QVariant> var;
        var << edit->WldData.IntroLevel_file << ui->WLD_AutostartLvl->text();
        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_INTROLEVEL, var);
        edit->WldData.IntroLevel_file = ui->WLD_AutostartLvl->text();
        edit->WldData.meta.modified = true;
    }
}


void WorldSettingsBox::on_WLD_AutostartLvlBrowse_clicked()
{
    if(world_settings_lock_fields) return;

    QString dirPath;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
        dirPath = mw()->activeWldEditWin()->WldData.meta.path;
    else
        return;

    LevelFileList levelList(dirPath, ui->WLD_AutostartLvl->text());

    if(levelList.exec() == QDialog::Accepted)
    {
        if(mw()->activeChildWindow() == MainWindow::WND_World)
        {
            ui->WLD_AutostartLvl->setText(levelList.SelectedFile);
            ui->WLD_AutostartLvl->setModified(true);
            on_WLD_AutostartLvl_editingFinished();
        }
    }
}

void WorldSettingsBox::on_WLD_Stars_valueChanged(int arg1)
{
    if(world_settings_lock_fields) return;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        QList<QVariant> var;
        var << edit->WldData.stars << arg1;
        edit->scene->m_history->addChangeWorldSettingsHistory(HistorySettings::SETTING_TOTALSTARS, var);
        edit->WldData.stars = static_cast<unsigned int>(arg1);
        edit->WldData.meta.modified = true;
    }
}


void WorldSettingsBox::on_WLD_Credirs_textChanged()
{
    if(world_settings_lock_fields) return;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit) return;

        edit->WldData.authors = ui->WLD_Credirs->toPlainText();
        edit->WldData.meta.modified = true;
    }
}



/********************************Star counter begin**************************************************/
unsigned long WorldSettingsBox::StarCounter_checkLevelFile(QString FilePath, QSet<QString> &exists)
{
    QRegExp lvlext = QRegExp(".*\\.(lvl|lvlx)$");
    lvlext.setCaseSensitivity(Qt::CaseInsensitive);
    LevelData getLevelHead;
    unsigned long starCount = 0;
    getLevelHead.stars = 0;

    if(lvlext.exactMatch(FilePath))
    {
        if(!FileFormats::OpenLevelFile(FilePath, getLevelHead))
            return 0;

        unsigned long foundStars = 0;

        //Mark all stars
        for(int q = 0; q < getLevelHead.npc.size(); q++)
        {
            getLevelHead.npc[q].is_star = mw()->configs.main_npc[static_cast<int>(getLevelHead.npc[q].id)].setup.is_star;

            if((getLevelHead.npc[q].is_star) && (!getLevelHead.npc[q].friendly))
                foundStars++;

            if(StarCounter_canceled)
                return starCount;
        }

        starCount += foundStars;

        for(int i = 0; i < getLevelHead.doors.size(); i++)
        {
            if(!getLevelHead.doors[i].lname.isEmpty())
            {
                QString FilePath_W = getLevelHead.meta.path + "/" + getLevelHead.doors[i].lname;

                if(!FilePath_W.endsWith(".lvl", Qt::CaseInsensitive) &&
                   !FilePath_W.endsWith(".lvlx", Qt::CaseInsensitive))
                    FilePath_W.append(".lvl");

                if(!QFile::exists(FilePath_W))
                    continue;

                if(exists.contains(FilePath_W))
                    continue;

                exists.insert(FilePath_W);
                starCount += StarCounter_checkLevelFile(FilePath_W, exists);

                if(StarCounter_canceled)
                    return starCount;
            }
        }
    }

    return starCount;
}

unsigned long WorldSettingsBox::doStarCount(QString dir, QList<WorldLevelTile> levels, QString introLevel)
{
    //Count stars of all used levels on this world map
    QString dirPath = dir;
    unsigned long starzzz = 0;
    bool introCounted = false;
    StarCounter_canceled = false;
    QSet<QString> LevelAlreadyChecked;

    for(int i = 0; i < levels.size() || !introCounted; i++)
    {
        //Attempt to read stars quantity of level:
        QString FilePath;

        if(introCounted)
        {
            FilePath = dirPath + "/" + levels[i].lvlfile;

            if(levels[i].lvlfile.isEmpty())
                continue;
        }
        else
        {
            FilePath = dirPath + "/" + introLevel;
            i--;
            introCounted = true;

            if(FilePath.isEmpty())
                continue;
        }

        if(!FilePath.endsWith(".lvl", Qt::CaseInsensitive) &&
           !FilePath.endsWith(".lvlx", Qt::CaseInsensitive))
            FilePath.append(".lvl");

        if(!QFile::exists(FilePath))
            continue;

        if(LevelAlreadyChecked.contains(FilePath))
            continue;

        LevelAlreadyChecked.insert(FilePath);
        emit countedStar(i < 0 ? 1 : i);
        starzzz += StarCounter_checkLevelFile(FilePath, LevelAlreadyChecked);

        if(StarCounter_canceled)
            break;
    }

    return starzzz;
}

void WorldSettingsBox::on_WLD_DoCountStars_clicked()
{
    if(world_settings_lock_fields)
        return;

    QString dirPath;

    if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *edit = mw()->activeWldEditWin();

        if(!edit)
            return;

#ifdef PGE_USE_NEW_STAR_COUNTER
        /**********************New star counter*************************/
        /* New star counter in works and currently buggy :P
         * but it must work more organized and safely
         ***************************************************************/
        AsyncStarCounter starCounter(dirPath,
                                     edit->WldData.levels,
                                     edit->WldData.IntroLevel_file,
                                     MainWinConnect::configs,
                                     mw());
        starCounter.startAndShowProgress();

        if(starCounter.isCancelled())
            return;

        ui->WLD_Stars->setValue(starCounter.countedStars());
        /**********************New star counter**end********************/
#else
        /***********************Old star counter************************/
        /*
         * Old Star Counter works in the single thread.
         * Doing level data fetching recoursively to build level list.
         ***************************************************************/
        QString __backUP;
        __backUP = ui->WLD_DoCountStars->text();
        ui->WLD_DoCountStars->setEnabled(false);
        ui->WLD_DoCountStars->setText(tr("Counting..."));
        dirPath = edit->WldData.meta.path;
        /*********************Stop animations to increase performance***********************/
        edit->scene->stopAnimation();
        /*********************Stop animations to increase performance***********************/
        QProgressDialog progress(tr("Calculating total star count in accessible levels"),
                                 tr("Abort"),
                                 0,
                                 edit->WldData.levels.size(),
                                 mw());
        progress.setWindowTitle(tr("Counting stars..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(util::alignToScreenCenter(progress.size()));
        progress.setMinimumDuration(0);
        progress.show();
        progress.connect(this,
                         &WorldSettingsBox::countedStar,
                         &progress,
                         &QProgressDialog::setValue,
                         Qt::BlockingQueuedConnection);
        QFuture<unsigned long> isOk = QtConcurrent::run(this,
                                      &WorldSettingsBox::doStarCount,
                                      QString(dirPath),
                                      edit->WldData.levels,
                                      edit->WldData.IntroLevel_file);

        /*************************Wait until star counter will do work***************************/
        while(!isOk.isFinished())
        {
            qApp->processEvents();

            if(progress.wasCanceled())
                StarCounter_canceled = true;

            QThread::msleep(1);
        }

        /****************************************************************************************/

        /***********************Resume stoped animation and restore 'count' button state**************************/
        if(edit->scene->m_opts.animationEnabled)
            edit->scene->startAnimation();

        ui->WLD_DoCountStars->setEnabled(true);
        ui->WLD_DoCountStars->setText(__backUP);

        /***********************Resume stoped animation and restore 'count' button state**************************/
        if(progress.wasCanceled())
            return;

        ui->WLD_Stars->setValue(static_cast<int>(isOk.result()));
        progress.close();
        /***********************Old star counter**end*******************/
#endif
    }
}
/********************************Star counter End**************************************************/
