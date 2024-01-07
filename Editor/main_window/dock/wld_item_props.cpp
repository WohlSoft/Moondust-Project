/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <pge_qt_compat.h>
#include <editing/_scenes/world/wld_item_placing.h>
#include <editing/_scenes/world/wld_history_manager.h>
#include <editing/_dialogs/itemselectdialog.h>
#include <editing/_scenes/world/items/item_level.h>
#include <PGE_File_Formats/file_formats.h>
#include "file_list_browser/levelfilelist.h"

#include <defines.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "wld_item_props.h"
#include "ui_wld_item_props.h"

WLD_ItemProps::WLD_ItemProps(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::WLD_ItemProps)
{
    setVisible(false);
    //setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    m_lockSettings = false;

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    QObject::connect(mw(), &MainWindow::languageSwitched, this, &WLD_ItemProps::re_translate);
    QObject::connect(this, &QDockWidget::visibilityChanged, mw()->ui->action_Placing_ShowProperties, &QAction::setChecked);
    mw()->ui->action_Placing_ShowProperties->setChecked(isVisible());
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );

    m_exitButtons[EXITCODE_LEFT] = ui->WLD_PROPS_ExitLeft;
    m_exitButtons[EXITCODE_RIGHT] = ui->WLD_PROPS_ExitRight;
    m_exitButtons[EXITCODE_TOP] = ui->WLD_PROPS_ExitTop;
    m_exitButtons[EXITCODE_BOTTOM] = ui->WLD_PROPS_ExitBottom;

    // Any
    m_exitButtonIcons[EXITCODE_LEFT][0] = QIcon(":/arrows/red_left.png");
    m_exitButtonIcons[EXITCODE_RIGHT][0] = QIcon(":/arrows/red_right.png");
    m_exitButtonIcons[EXITCODE_TOP][0] = QIcon(":/arrows/red_up.png");
    m_exitButtonIcons[EXITCODE_BOTTOM][0] = QIcon(":/arrows/red_down.png");
    // None
    m_exitButtonIcons[EXITCODE_LEFT][1] = QIcon(":/arrows/black_left.png");
    m_exitButtonIcons[EXITCODE_RIGHT][1] = QIcon(":/arrows/black_right.png");
    m_exitButtonIcons[EXITCODE_TOP][1] = QIcon(":/arrows/black_up.png");
    m_exitButtonIcons[EXITCODE_BOTTOM][1] = QIcon(":/arrows/black_down.png");
    // Code
    m_exitButtonIcons[EXITCODE_LEFT][2] = QIcon(":/arrows/green_left.png");
    m_exitButtonIcons[EXITCODE_RIGHT][2] = QIcon(":/arrows/green_right.png");
    m_exitButtonIcons[EXITCODE_TOP][2] = QIcon(":/arrows/green_up.png");
    m_exitButtonIcons[EXITCODE_BOTTOM][2] = QIcon(":/arrows/green_down.png");

    for(size_t i = 0; i < 4; ++i)
    {
        QObject::connect(&m_exitCodes[i], &QMenu::triggered,
                         this, &WLD_ItemProps::levelExitCodeSelected);
        m_exitButtons[i]->setMenu(&m_exitCodes[i]);
    }
}

WLD_ItemProps::~WLD_ItemProps()
{
    delete ui;
}

void WLD_ItemProps::re_translate()
{
    ui->retranslateUi(this);
    resetExitTypesList();
}


static bool s_setExitCodeMenuCode(QMenu *menu, int code);
static void s_updateToolButton(QToolButton *tb, QMenu *menu, QIcon icons[3]);

void WLD_ItemProps::openPropertiesFor(int Type, WorldLevelTile level, bool newItem, bool dontShow)
{
    bool ret;
    m_lockSettings = true;

    switch(Type)
    {
    case 0:
    {
        if(newItem)
            m_currentLevelArrayId = -1;
        else
            m_currentLevelArrayId = level.meta.array_id;

        ui->WLD_PROPS_lvlID->setText(tr("Level ID: %1, Array ID: %2").arg(level.id).arg(level.meta.array_id));
        ui->WLD_PROPS_lvlPos->setText(tr("Position: [%1, %2]").arg(level.x).arg(level.y));
        ui->WLD_PROPS_LVLTitle->setText(level.title);
        ui->WLD_PROPS_LVLFile->setText(level.lvlfile);
        ui->WLD_PROPS_EnterTo->setValue(level.entertowarp);

        ui->WLD_PROPS_AlwaysVis->setChecked(level.alwaysVisible);
        ui->WLD_PROPS_PathBG->setChecked(level.pathbg);
        ui->WLD_PROPS_BigPathBG->setChecked(level.bigpathbg);
        ui->WLD_PROPS_GameStart->setChecked(level.gamestart);

        ui->WLD_PROPS_GotoX->setText((level.gotox == -1) ? "" : QString::number(level.gotox));
        ui->WLD_PROPS_GotoY->setText((level.gotoy == -1) ? "" : QString::number(level.gotoy));

        ui->WLD_PROPS_GetPoint->setChecked(false);
        ui->WLD_PROPS_GetPoint->setCheckable(false);

        ret = s_setExitCodeMenuCode(&m_exitCodes[EXITCODE_LEFT], level.left_exit);
        if(!ret && newItem)
        {
            WldPlacingItems::levelSet.left_exit = -1;
            level.left_exit = -1;
        }
        s_updateToolButton(ui->WLD_PROPS_ExitLeft, &m_exitCodes[EXITCODE_LEFT], m_exitButtonIcons[EXITCODE_LEFT]);

        ret = s_setExitCodeMenuCode(&m_exitCodes[EXITCODE_RIGHT], level.right_exit);
        if(!ret && newItem)
        {
            WldPlacingItems::levelSet.right_exit = -1;
            level.right_exit = -1;
        }
        s_updateToolButton(ui->WLD_PROPS_ExitRight, &m_exitCodes[EXITCODE_RIGHT], m_exitButtonIcons[EXITCODE_RIGHT]);

        ret = s_setExitCodeMenuCode(&m_exitCodes[EXITCODE_TOP], level.top_exit);
        if(!ret && newItem)
        {
            WldPlacingItems::levelSet.top_exit = -1;
            level.top_exit = -1;
        }
        s_updateToolButton(ui->WLD_PROPS_ExitTop, &m_exitCodes[EXITCODE_TOP], m_exitButtonIcons[EXITCODE_TOP]);

        ret = s_setExitCodeMenuCode(&m_exitCodes[EXITCODE_BOTTOM], level.bottom_exit);
        if(!ret && newItem)
        {
            WldPlacingItems::levelSet.bottom_exit = -1;
            level.bottom_exit = -1;
        }
        s_updateToolButton(ui->WLD_PROPS_ExitBottom, &m_exitCodes[EXITCODE_BOTTOM], m_exitButtonIcons[EXITCODE_BOTTOM]);

        m_lockSettings = false;
        if(!dontShow)
        {
            show();
            raise();
        }
        break;
    }
    case -1: //Nothing to edit
    default:
        hide();
    }
    m_lockSettings = false;
}

void WLD_ItemProps::hideToolbox()
{
    hide();
}


struct LevelExitCodeEntry
{
    QString label;
    bool disable = false;
    bool visible = true;
    int code = -1;
};

static bool s_setExitCodeMenuCode(QMenu *menu, int code)
{
    bool found = false;
    QAction *any = nullptr;
    for(auto *a : menu->actions())
    {
        int d = a->data().toInt();
        bool c = (d == code);
        a->setChecked(c);
        if(c)
            found = true;
    }

    if(!found && any)
        any ->setChecked(true);

    return found;
}

static void s_updateToolButton(QToolButton *tb, QMenu *menu, QIcon icons[])
{
    for(auto *a : menu->actions())
    {
        if(a->isChecked())
        {
            QString t = a->text();
            int code = a->data().toInt();
            tb->setText(t.size() > 10 ? t.mid(0, 7) + "..." : t);
            tb->setToolTip(a->text());
            QFont f = tb->font();
            f.setBold(code > 0);
            tb->setFont(f);
            if(code < 0)
                tb->setIcon(icons[0]);
            else if(code == 0)
                tb->setIcon(icons[1]);
            else
                tb->setIcon(icons[2]);
        }
    }
}

static void s_updateExitCodeMenu(QMenu *menu, const QVector<LevelExitCodeEntry> &list)
{
    int checked = -1;

    for(auto *a : menu->actions())
    {
        if(a->isChecked())
        {
            checked = a->data().toInt();
            break;
        }
    }

    menu->clear();

    for(auto &l : list)
    {
        if(l.code == 1)
            menu->addSeparator();
        QAction *a = menu->addAction(QString("%1 - %2").arg(l.code == -1 ? "*" : QString::number(l.code)).arg(l.label));
        a->setVisible(!l.disable);
        a->setCheckable(true);
        a->setData(l.code);
        a->setChecked(l.code == checked);
    }
}

void WLD_ItemProps::resetExitTypesList()
{
    int codeCounter = -1;
    QVector<LevelExitCodeEntry> levelExitType;
    DataConfig &c = mw()->configs;

    QStringList built_in_names =
    {
        tr("Any", "Exit type name"),
        tr("None", "Exit type name"),
        tr("Card Roulette Exit", "Exit type name"),
        tr("Ball Exit", "Exit type name"),
        tr("Walked Offscreen", "Exit type name"),
        tr("Secret Exit", "Exit type name"),
        tr("Crystal Sphare Exit", "Exit type name"),
        tr("Warp Exit", "Exit type name"),
        tr("Star Exit", "Exit type name"),
        tr("Tape Exit", "Exit type name")
    };

    for(auto it : built_in_names)
    {
        LevelExitCodeEntry e;
        e.disable = false;
        e.code = codeCounter++;
        e.label = it;
        levelExitType.push_back(e);
    }

    if(!c.meta_exit_codes.isEmpty() && levelExitType.size() > (c.meta_exit_codes.size() + 2))
        levelExitType.resize(c.meta_exit_codes.size() + 2);

    for(auto &m : c.meta_exit_codes)
    {
        if(m.code <= 0 || m.built_in)
            continue; // Skip invalid and built-in overriding
        int idx = m.code + 1;

        if(idx >= levelExitType.size())
            levelExitType.push_back(LevelExitCodeEntry());

        auto &it = levelExitType[idx];
        it.code = m.code;
        it.disable = m.disabled;

        if(m.name_lang.contains(GlobalSettings::locale))
            it.label = m.name_lang[GlobalSettings::locale];
        else if(!m.name.isEmpty())
            it.label = m.name;
        else
            it.label = tr("<Exit type %1>", "Fallback name for the exit code when it's blank").arg(m.code);
    }

    m_lockSettings = true;
    for(size_t i = 0; i < 4; ++i)
    {
        s_updateExitCodeMenu(&m_exitCodes[i], levelExitType);
        s_updateToolButton(m_exitButtons[i], &m_exitCodes[i], m_exitButtonIcons[i]);
    }
    m_lockSettings = false;
}


//ITemProps
void WLD_ItemProps::on_WLD_PROPS_PathBG_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.pathbg = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->setShowSmallPathBG(checked);
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_PATHBACKGROUND, QVariant(checked));
    }
}

void WLD_ItemProps::on_WLD_PROPS_BigPathBG_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.bigpathbg = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->setShowBigPathBG(checked);
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_BIGPATHBACKGROUND, QVariant(checked));
    }

}

void WLD_ItemProps::on_WLD_PROPS_AlwaysVis_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.alwaysVisible = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->alwaysVisible(checked);
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_ALWAYSVISIBLE, QVariant(checked));
    }

}

void WLD_ItemProps::on_WLD_PROPS_GameStart_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.gamestart = checked;
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.gamestart = checked;
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_GAMESTARTPOINT, QVariant(checked));
    }

}

void WLD_ItemProps::on_WLD_PROPS_LVLFile_editingFinished()
{
    if(m_lockSettings) return;

    if(!ui->WLD_PROPS_LVLFile->isModified()) return;
    ui->WLD_PROPS_LVLFile->setModified(false);

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.lvlfile = ui->WLD_PROPS_LVLFile->text();
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.lvlfile = ui->WLD_PROPS_LVLFile->text();
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_LEVELFILE, QVariant(ui->WLD_PROPS_LVLFile->text()));
    }

}

void WLD_ItemProps::on_WLD_PROPS_LVLTitle_editingFinished()
{
    if(m_lockSettings) return;

    if(!ui->WLD_PROPS_LVLTitle->isModified()) return;
    ui->WLD_PROPS_LVLTitle->setModified(false);

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.title = ui->WLD_PROPS_LVLTitle->text();
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.title = ui->WLD_PROPS_LVLTitle->text();
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_LEVELTITLE, QVariant(ui->WLD_PROPS_LVLTitle->text()));
    }
}

void WLD_ItemProps::on_WLD_PROPS_EnterTo_valueChanged(int arg1)
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.entertowarp = arg1;
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.entertowarp = arg1;
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_DOORID, QVariant(arg1));
    }

}

void WLD_ItemProps::on_WLD_PROPS_LVLBrowse_clicked()
{
    if(m_lockSettings) return;

    QString dirPath;
    if(mw()->activeChildWindow() != MainWindow::WND_World)
        return;

    WorldEdit *edit = mw()->activeWldEditWin();
    if(!edit) return;

    dirPath = edit->WldData.meta.path;

    if(edit->isUntitled())
    {
        QMessageBox::information(this,
                                 tr("Please save the file"),
                                 tr("Please save the file before selecting levels."),
                                 QMessageBox::Ok);
        return;
    }

    LevelFileList levelList(dirPath, ui->WLD_PROPS_LVLFile->text());
    if(levelList.exec() == QDialog::Accepted)
    {
        ui->WLD_PROPS_LVLFile->setText(levelList.currentFile());
        ui->WLD_PROPS_LVLFile->setModified(true);
        on_WLD_PROPS_LVLFile_editingFinished();

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        Q_QRegExp lvlext = Q_QRegExp(".*\\.(lvl|lvlx)$");
        lvlext.setPatternSyntax(QRegExp::RegExp2);
        lvlext.setCaseSensitivity(Qt::CaseInsensitive);
#else
        Q_QRegExp lvlext = QRegularExpression(".*\\.(lvl|lvlx)$", QRegularExpression::CaseInsensitiveOption);
#endif

        //Attempt to read level title:
        QString FilePath = dirPath + "/" + levelList.currentFile();
        QFile file(FilePath);

        if(!file.open(QIODevice::ReadOnly)) return;

        LevelData getLevelHead;
        getLevelHead.LevelName = "";

        if(lvlext.Q_QRegExpMatch(FilePath))
        {
            if(!FileFormats::OpenLevelFileHeader(FilePath, getLevelHead))
                return;
        }

        file.close();
        if(!getLevelHead.LevelName.isEmpty())
        {
            ui->WLD_PROPS_LVLTitle->setText(getLevelHead.LevelName);
            ui->WLD_PROPS_LVLTitle->setModified(true);
            on_WLD_PROPS_LVLTitle_editingFinished();
        }
    }

}

void WLD_ItemProps::levelExitCodeSelected(QAction *exitCode)
{
    if(m_lockSettings || !exitCode)
        return;

    QMenu *s = qobject_cast<QMenu*>(sender());
    ExitCodesSides side = EXITCODE__INVALID;
    HistorySettings::WorldSettingSubType histSide = HistorySettings::SETTING_PATHBYTOP;
    int finalIndex = exitCode->data().toInt();

    for(int i = 0; i < 4; ++i)
    {
        if(s == &m_exitCodes[i])
        {
            side = static_cast<ExitCodesSides>(i);
            break;
        }
    }

    s_setExitCodeMenuCode(&m_exitCodes[side], finalIndex);
    s_updateToolButton(m_exitButtons[side], &m_exitCodes[side], m_exitButtonIcons[side]);

    switch(side)
    {
    case EXITCODE_LEFT:
        histSide = HistorySettings::SETTING_PATHBYLEFT;
        break;
    case EXITCODE_RIGHT:
        histSide = HistorySettings::SETTING_PATHBYRIGHT;
        break;
    case EXITCODE_TOP:
        histSide = HistorySettings::SETTING_PATHBYTOP;
        break;
    case EXITCODE_BOTTOM:
        histSide = HistorySettings::SETTING_PATHBYBOTTOM;
        break;
    default:
        Q_ASSERT(false); // INVALID SIDE VALUE
        break;
    }

    if(m_currentLevelArrayId < 0)
    {
        switch(side)
        {
        case EXITCODE_LEFT:
            WldPlacingItems::levelSet.left_exit = finalIndex;
            break;
        case EXITCODE_RIGHT:
            WldPlacingItems::levelSet.right_exit = finalIndex;
            break;
        case EXITCODE_TOP:
            WldPlacingItems::levelSet.top_exit = finalIndex;
            break;
        case EXITCODE_BOTTOM:
            WldPlacingItems::levelSet.bottom_exit = finalIndex;
            break;
        default:
            Q_ASSERT(false); // INVALID SIDE VALUE
            break;
        }
    }
    else
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit)
            return;

        QList<QGraphicsItem *> items = edit->scene->selectedItems();

        for(QGraphicsItem *item : items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                ItemLevel *l = qgraphicsitem_cast<ItemLevel*>(item);
                Q_ASSERT(l);
                selData.levels.push_back(l->m_data);

                switch(side)
                {
                case EXITCODE_LEFT:
                    l->m_data.left_exit = finalIndex;
                    break;
                case EXITCODE_RIGHT:
                    l->m_data.right_exit = finalIndex;
                    break;
                case EXITCODE_TOP:
                    l->m_data.top_exit = finalIndex;
                    break;
                case EXITCODE_BOTTOM:
                    l->m_data.bottom_exit = finalIndex;
                    break;
                default:
                    Q_ASSERT(false); // INVALID SIDE VALUE
                    break;
                }

                l->arrayApply();
            }
        }

        edit->scene->m_history->addChangeSettingsHistory(selData, histSide, QVariant(finalIndex));
    }
}


//void MainWindow::on_WLD_PROPS_GotoX_textEdited(const QString &arg1)
void WLD_ItemProps::on_WLD_PROPS_GotoX_editingFinished()
{
    if(m_lockSettings) return;

    if(!ui->WLD_PROPS_GotoX->isModified()) return;
    ui->WLD_PROPS_GotoX->setModified(false);

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.gotox = (ui->WLD_PROPS_GotoX->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoX->text().toInt();
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.gotox = (ui->WLD_PROPS_GotoX->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoX->text().toInt();
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_GOTOX, QVariant((ui->WLD_PROPS_GotoX->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoX->text().toInt()));
    }

}

//void MainWindow::on_WLD_PROPS_GotoY_textEdited(const QString &arg1)
void WLD_ItemProps::on_WLD_PROPS_GotoY_editingFinished()
{
    if(m_lockSettings) return;

    if(!ui->WLD_PROPS_GotoY->isModified()) return;
    ui->WLD_PROPS_GotoY->setModified(false);

    if(m_currentLevelArrayId < 0)
        WldPlacingItems::levelSet.gotoy = (ui->WLD_PROPS_GotoY->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoY->text().toInt();
    else if(mw()->activeChildWindow() == MainWindow::WND_World)
    {
        WorldData selData;
        WorldEdit *edit = mw()->activeWldEditWin();
        if(!edit) return;
        QList<QGraphicsItem *> items = edit->scene->selectedItems();
        foreach(QGraphicsItem *item, items)
        {
            if(item->data(ITEM_TYPE).toString() == "LEVEL")
            {
                selData.levels.push_back(((ItemLevel *) item)->m_data);
                ((ItemLevel *)item)->m_data.gotoy = (ui->WLD_PROPS_GotoY->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoY->text().toInt();
                ((ItemLevel *)item)->arrayApply();
            }
        }
        edit->scene->m_history->addChangeSettingsHistory(selData, HistorySettings::SETTING_GOTOY, QVariant((ui->WLD_PROPS_GotoY->text().isEmpty()) ? -1 : ui->WLD_PROPS_GotoY->text().toInt()));
    }
}

void WLD_ItemProps::acceptWarpToCoordinates(QPoint p)
{
    ui->WLD_PROPS_GotoX->setText(QString::number(p.x()));
    ui->WLD_PROPS_GotoY->setText(QString::number(p.y()));

    ui->WLD_PROPS_GotoX->setModified(true);
    on_WLD_PROPS_GotoX_editingFinished();
    ui->WLD_PROPS_GotoY->setModified(true);
    on_WLD_PROPS_GotoY_editingFinished();

    ui->WLD_PROPS_GetPoint->setChecked(false);
    ui->WLD_PROPS_GetPoint->setCheckable(false);
}

void WLD_ItemProps::on_WLD_PROPS_GetPoint_clicked()
{
    if(m_lockSettings) return;

    if(m_currentLevelArrayId < 0)
    {
        QMessageBox::information(this, QString(tr("Placement mode")), tr("Place item on the map first and call 'Properties' context menu item."), QMessageBox::Ok);
        return;
    }
    else
    {
        if(mw()->activeChildWindow(mw()->LastActiveSubWindow) == 3)
        {
            WorldEdit *edit = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
            if(!edit) return;

            if(ui->WLD_PROPS_GetPoint->isCheckable())
            {
                ui->WLD_PROPS_GetPoint->setChecked(false);
                ui->WLD_PROPS_GetPoint->setCheckable(false);

                edit->scene->SwitchEditingMode(WldScene::MODE_Selecting);
            }
            else
            {
                WorldEdit *edit = mw()->activeWldEditWin(mw()->LastActiveSubWindow);
                if(!edit) return;

                ui->WLD_PROPS_GetPoint->setCheckable(true);
                ui->WLD_PROPS_GetPoint->setChecked(true);

                //activeWldEditWin()->changeCursor(WorldEdit::MODE_PlaceItem);
                edit->scene->SwitchEditingMode(WldScene::MODE_SetPoint);

                WldPlacingItems::placingMode = WldPlacingItems::PMODE_Brush;

                //WldPlacingItems::squarefillingMode = false;
                mw()->ui->actionRectFill->setChecked(false);
                mw()->ui->actionRectFill->setEnabled(false);

                //WldPlacingItems::lineMode = false;
                mw()->ui->actionLine->setChecked(false);
                mw()->ui->actionLine->setEnabled(false);
                if(ui->WLD_PROPS_GotoX->text().isEmpty() || ui->WLD_PROPS_GotoY->text().isEmpty())
                    edit->scene->m_pointSelector.m_pointNotPlaced = true;
                else
                {
                    edit->scene->m_pointSelector.m_pointNotPlaced = false;
                    edit->scene->m_pointSelector.m_pointCoord = QPoint(ui->WLD_PROPS_GotoX->text().toInt(), ui->WLD_PROPS_GotoY->text().toInt());
                }

                edit->scene->setItemPlacer(5);
                edit->setFocus();
            }
        }
    }
}

