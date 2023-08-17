/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QInputDialog>
#include <QStandardItemModel>

#include <editing/_scenes/level/lvl_history_manager.h>
#include <editing/_scenes/level/lvl_item_placing.h>
#include "file_list_browser/levelfilelist.h"
#include <editing/_dialogs/wld_setpoint.h>
#include <PGE_File_Formats/file_formats.h>
#include <main_window/dock/lvl_item_properties.h>
#include <common_features/direction_switch_widget.h>
#include <common_features/logger.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "lvl_warp_props.h"
#include "ui_lvl_warp_props.h"

void MainWindow::on_actionWarpsAndDoors_triggered(bool checked)
{
    dock_LvlWarpProps->setVisible(checked);
    if(checked) dock_LvlWarpProps->raise();
}


LvlWarpBox::LvlWarpBox(QWidget *parent) :
    QDockWidget(parent),
    MWDock_Base(parent),
    ui(new Ui::LvlWarpBox)
{
    setVisible(false);
    setAttribute(Qt::WA_ShowWithoutActivating);
    ui->setupUi(this);
    m_lockSettings = false;

    ui->entrance->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/box.png"));
    ui->entrance->setPixmap(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/black_right.png"));
    ui->entrance->setPixmap(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/black_left.png"));
    ui->entrance->setPixmap(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/black_down.png"));
    ui->entrance->setPixmap(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/black_up.png"));
    ui->entrance->setPixmapOn(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/red_right.png"));
    ui->entrance->setPixmapOn(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/red_left.png"));
    ui->entrance->setPixmapOn(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/red_down.png"));
    ui->entrance->setPixmapOn(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/red_up.png"));
    ui->entrance->mapValue(DirectionSwitchWidget::S_LEFT, LevelDoor::ENTRANCE_RIGHT);
    ui->entrance->mapValue(DirectionSwitchWidget::S_RIGHT, LevelDoor::ENTRANCE_LEFT);
    ui->entrance->mapValue(DirectionSwitchWidget::S_TOP, LevelDoor::ENTRANCE_DOWN);
    ui->entrance->mapValue(DirectionSwitchWidget::S_BOTTOM, LevelDoor::ENTRANCE_UP);

    ui->exit->setPixmap(DirectionSwitchWidget::S_CENTER, QPixmap(":/arrows/box.png"));
    ui->exit->setPixmap(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/black_left.png"));
    ui->exit->setPixmap(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/black_right.png"));
    ui->exit->setPixmap(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/black_up.png"));
    ui->exit->setPixmap(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/black_down.png"));
    ui->exit->setPixmapOn(DirectionSwitchWidget::S_LEFT, QPixmap(":/arrows/green_left.png"));
    ui->exit->setPixmapOn(DirectionSwitchWidget::S_RIGHT, QPixmap(":/arrows/green_right.png"));
    ui->exit->setPixmapOn(DirectionSwitchWidget::S_TOP, QPixmap(":/arrows/green_up.png"));
    ui->exit->setPixmapOn(DirectionSwitchWidget::S_BOTTOM, QPixmap(":/arrows/green_down.png"));
    ui->exit->mapValue(DirectionSwitchWidget::S_LEFT, LevelDoor::EXIT_LEFT);
    ui->exit->mapValue(DirectionSwitchWidget::S_RIGHT, LevelDoor::EXIT_RIGHT);
    ui->exit->mapValue(DirectionSwitchWidget::S_TOP, LevelDoor::EXIT_UP);
    ui->exit->mapValue(DirectionSwitchWidget::S_BOTTOM, LevelDoor::EXIT_DOWN);

    re_translate_widgets();

    QRect mwg = mw()->geometry();
    int GOffset = 10;
    mw()->addDockWidget(Qt::RightDockWidgetArea, this);
    connect(mw(), SIGNAL(languageSwitched()), this, SLOT(re_translate()));
    connect(this, SIGNAL(visibilityChanged(bool)), mw()->ui->actionWarpsAndDoors, SLOT(setChecked(bool)));
    setFloating(true);
    setGeometry(
        mwg.right() - width() - GOffset,
        mwg.y() + 120,
        width(),
        height()
    );

    connect(ui->entrance, &DirectionSwitchWidget::clicked, this, &LvlWarpBox::entrance_clicked);
    connect(ui->exit, &DirectionSwitchWidget::clicked, this, &LvlWarpBox::exit_clicked);
    // TODO: Make nicer names for all other buttons and actions

    connect(ui->entryAdd, SIGNAL(clicked()), this, SLOT(addWarpEntry()));
    connect(ui->entryRemove, SIGNAL(clicked()), this, SLOT(removeWarpEntry()));
    connect(ui->entryClone, SIGNAL(clicked()), this, SLOT(duplicateWarpEntry()));

    connect(mw(), SIGNAL(setSMBX64Strict(bool)),
            this, SLOT(setSMBX64Strict(bool)));

    m_lastVisibilityState = isVisible();
    mw()->docks_level.
    addState(this, &m_lastVisibilityState);
}

LvlWarpBox::~LvlWarpBox()
{
    delete ui;
}

void LvlWarpBox::re_translate_widgets()
{
    ui->entrance->mapToolTip(DirectionSwitchWidget::S_LEFT, tr("Right"));
    ui->entrance->mapToolTip(DirectionSwitchWidget::S_RIGHT, tr("Left"));
    ui->entrance->mapToolTip(DirectionSwitchWidget::S_TOP, tr("Down"));
    ui->entrance->mapToolTip(DirectionSwitchWidget::S_BOTTOM, tr("Up"));
    ui->exit->mapToolTip(DirectionSwitchWidget::S_LEFT, tr("Left"));
    ui->exit->mapToolTip(DirectionSwitchWidget::S_RIGHT, tr("Right"));
    ui->exit->mapToolTip(DirectionSwitchWidget::S_TOP, tr("Up"));
    ui->exit->mapToolTip(DirectionSwitchWidget::S_BOTTOM, tr("Down"));
}

QComboBox *LvlWarpBox::cbox_layer()
{
    return ui->WarpLayer;
}

QComboBox *LvlWarpBox::cbox_event_enter()
{
    return ui->WarpEnterEvent;
}

void LvlWarpBox::setSettingsLock(bool locked)
{
    m_lockSettings = locked;
}

void LvlWarpBox::setSMBX64Strict(bool en)
{
    DataConfig &c = mw()->configs;
    bool shown = !en;

    {
        auto *model = qobject_cast<QStandardItemModel *>(ui->WarpType->model());
        QStandardItem *item = model->item(3);//Portal warp item
        if(c.editor.supported_features.level_warp_portal == EditorSetup::FeaturesSupport::F_ENABLED)
            item->setFlags(item->flags() | Qt::ItemIsEnabled);
        else
            item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }

    // Disable/Hide all non-supported in SMBX 1.3 parameters
    ui->transitEffect->setEnabled(shown && (c.editor.supported_features.level_warp_transit_type == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->transitEffect->setHidden(c.editor.supported_features.level_warp_transit_type == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->transitEffectLabel->setEnabled(ui->transitEffect->isEnabled());
    ui->transitEffectLabel->setHidden(ui->transitEffect->isHidden());

    ui->WarpTwoWay->setEnabled(shown && (c.editor.supported_features.level_warp_two_way == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpTwoWay->setHidden(c.editor.supported_features.level_warp_two_way == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpNeedAStarsMsg->setEnabled(shown && (c.editor.supported_features.level_warp_needstars_message == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpNeedAStarsMsg->setHidden(c.editor.supported_features.level_warp_needstars_message == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpNeedAStarsMsg_label->setEnabled(shown && (c.editor.supported_features.level_warp_needstars_message == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpNeedAStarsMsg_label->setHidden(c.editor.supported_features.level_warp_needstars_message == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpHideStars->setEnabled(shown && (c.editor.supported_features.level_warp_hide_stars == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpHideStars->setHidden(c.editor.supported_features.level_warp_hide_stars == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpBombNeed->setEnabled(shown && (c.editor.supported_features.level_warp_bomb_exit == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpBombNeed->setHidden(c.editor.supported_features.level_warp_bomb_exit == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpHideLevelEnterScreen->setEnabled(shown && (c.editor.supported_features.level_warp_hide_interlevel_scene == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpHideLevelEnterScreen->setHidden(c.editor.supported_features.level_warp_hide_interlevel_scene == EditorSetup::FeaturesSupport::F_HIDDEN);

    ui->WarpAllowNPC_IL->setEnabled(shown && (c.editor.supported_features.level_warp_allow_interlevel_npc == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpAllowNPC_IL->setHidden(c.editor.supported_features.level_warp_allow_interlevel_npc == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpSpecialStateOnly->setEnabled(shown && (c.editor.supported_features.level_warp_allow_sp_state_only == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->WarpSpecialStateOnly->setHidden(c.editor.supported_features.level_warp_allow_sp_state_only == EditorSetup::FeaturesSupport::F_HIDDEN);

    ui->warpNeedsFloor->setEnabled(shown && (c.editor.supported_features.level_warp_allow_needs_floor == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->warpNeedsFloor->setHidden(c.editor.supported_features.level_warp_allow_needs_floor == EditorSetup::FeaturesSupport::F_HIDDEN);

    ui->warpBoxCannon->setEnabled(shown && (c.editor.supported_features.level_warp_cannon_exit == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->warpBoxCannon->setHidden(c.editor.supported_features.level_warp_cannon_exit == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpEnableCannon->setDisabled(en);
    ui->WarpCannonSpeed->setDisabled(en);

    ui->warpBoxEnterEvent->setEnabled(shown && (c.editor.supported_features.level_warp_on_enter_event == EditorSetup::FeaturesSupport::F_ENABLED));
    ui->warpBoxEnterEvent->setHidden(c.editor.supported_features.level_warp_on_enter_event == EditorSetup::FeaturesSupport::F_HIDDEN);
    ui->WarpEnterEvent->setDisabled(en);
    ui->WarpEnterEvent_label->setDisabled(en);
}

void LvlWarpBox::re_translate()
{
    m_lockSettings = true;
    int doorType = ui->WarpType->currentIndex(); //backup combobox's index
    ui->retranslateUi(this);
    re_translate_widgets();
    ui->WarpType->setCurrentIndex(doorType); //restore combobox's index
    m_lockSettings = false;
}

void LvlWarpBox::init()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    ui->warpsList->clear();
    auto *edit = mw()->activeLvlEditWin();
    Q_ASSERT(edit);

    for(LevelDoor &door : edit->LvlData.doors)
        ui->warpsList->addItem(doorTitle(door), door.meta.array_id);

    if(ui->warpsList->count() <= 0)
        setDoorData(-1);
    else
        setDoorData(ui->warpsList->currentIndex());
}

QString LvlWarpBox::doorTitle(LevelDoor &door)
{
    return QString("%1: x%2y%3 <=> x%4y%5")
           .arg(door.meta.array_id).arg(door.ix).arg(door.iy).arg(door.ox).arg(door.oy);
}


void LvlWarpBox::SwitchToDoor(long arrayID)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    show();
    raise();
    ui->warpsList->setCurrentIndex(ui->warpsList->findData(QString::number(arrayID)));
}


static LevelDoor *findWarp(LevelData &level, unsigned int id)
{
    for(LevelDoor &d : level.doors)
    {
        if(d.meta.array_id == id)
            return &d;
    }

    return nullptr;
}

void LvlWarpBox::setDoorData(long index)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    long cIndex = (index == -2) ? ui->warpsList->currentIndex() : index;

    qDebug() << "Current warp indes is " << ui->warpsList->currentIndex();

    m_lockSettings = true;
    LevelEdit *le = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(le->LvlData, warpId);
    if(w)
    {
        LevelDoor &door = *w;
        ui->warpsList->setItemText(static_cast<int>(cIndex), doorTitle(door));
        ui->entryRemove->setEnabled(true);

        ui->warpBoxDirect->setEnabled(true);
        ui->warpBoxMain->setEnabled(true);
        ui->warpBoxWD->setEnabled(true);
        ui->warpBoxWTL->setEnabled(true);
        ui->warpBoxWTW->setEnabled(true);
        ui->warpBoxCannon->setEnabled(true);

        ui->WarpTwoWay->setChecked(door.two_way);

        ui->WarpAllowNPC->setChecked(door.allownpc);
        ui->WarpLock->setChecked(door.locked);
        ui->WarpNoVehicles->setChecked(door.novehicles);
        ui->WarpType->setCurrentIndex(door.type);
        ui->transitEffect->setCurrentIndex(door.transition_effect);

        ui->WarpEntrancePlaced->setChecked(door.isSetIn);
        ui->WarpExitPlaced->setChecked(door.isSetOut);

        ui->WarpNeedAStars->setValue(door.stars);
        ui->WarpNeedAStarsMsg->setText(door.stars_msg);
        ui->WarpHideStars->setChecked(door.star_num_hide);
        ui->WarpBombNeed->setChecked(door.need_a_bomb);
        ui->WarpSpecialStateOnly->setChecked(door.special_state_required);
        ui->warpNeedsFloor->setChecked(door.stood_state_required);

        ui->WarpEntranceGrp->setEnabled(door.type == 1);
        ui->WarpExitGrp->setEnabled(door.type == 1);

        ui->WarpLayer->setCurrentText(door.layer);
        if(ui->WarpLayer->currentIndex() < 0)
            ui->WarpLayer->setCurrentIndex(0);

        if(door.event_enter.isEmpty())
            ui->WarpEnterEvent->setCurrentIndex(0);
        else
        {
            ui->WarpEnterEvent->setCurrentText(door.event_enter);
            if(ui->WarpEnterEvent->currentIndex() < 0)
                ui->WarpEnterEvent->setCurrentIndex(0);
        }

        ui->entrance->setDirection(door.idirect);
        ui->exit->setDirection(door.odirect);

        ui->WarpEnableCannon->setChecked(door.cannon_exit);
        ui->WarpCannonSpeed->setEnabled(door.cannon_exit && !le->LvlData.meta.smbx64strict);
        ui->WarpCannonSpeed->setValue(door.cannon_exit_speed);

        ui->WarpToMapX->setText((door.world_x != -1) ? QString::number(door.world_x) : "");
        ui->WarpToMapY->setText((door.world_y != -1) ? QString::number(door.world_y) : "");

        ui->WarpLevelExit->setChecked(door.lvl_o);
        ui->WarpLevelEntrance->setChecked(door.lvl_i);

        ui->WarpSetEntrance->setEnabled(((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_o) && (!door.lvl_i)));
        ui->WarpSetExit->setEnabled(((!door.lvl_o) && (!door.lvl_i)) || ((door.lvl_i)));

        ui->WarpLevelFile->setText(door.lname);
        ui->WarpToExitNu->setValue(static_cast<int>(door.warpto));

        ui->WarpHideLevelEnterScreen->setChecked(door.hide_entering_scene);
        ui->WarpAllowNPC_IL->setChecked(door.allownpc_interlevel);
    }
    else
    {
        ui->entryRemove->setEnabled(false);

        ui->warpBoxDirect->setEnabled(false);
        ui->warpBoxMain->setEnabled(false);
        ui->warpBoxWD->setEnabled(false);
        ui->warpBoxWTL->setEnabled(false);
        ui->warpBoxWTW->setEnabled(false);
        ui->warpBoxCannon->setEnabled(false);
    }

    m_lockSettings = false;
}


///////////////////////////////////////////////////////////////
///////////////////// Door Control/////////////////////////////
///////////////////////////////////////////////////////////////
void LvlWarpBox::on_warpsList_currentIndexChanged(int index)
{
    setDoorData(index);
}

void LvlWarpBox::addWarpEntry()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    LevelDoor newDoor = FileFormats::CreateLvlWarp();
    newDoor.meta.array_id = edit->LvlData.doors_array_id++;
    newDoor.meta.index = static_cast<unsigned int>(edit->LvlData.doors.size());
    newDoor.type = GlobalSettings::LvlItemDefaults.warp_type;//Apply initial warp type value
    edit->LvlData.doors.push_back(newDoor);
    edit->LvlData.meta.modified = true;

    edit->scene->m_history->addAddWarp(static_cast<int>(newDoor.meta.array_id),
                                       ui->warpsList->count(),
                                       static_cast<int>(newDoor.meta.index));

    ui->warpsList->addItem(doorTitle(newDoor), newDoor.meta.array_id);
    ui->warpsList->setCurrentIndex(ui->warpsList->count() - 1);
    ui->entryRemove->setEnabled(true);
}

void LvlWarpBox::removeWarpEntry()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        edit->scene->m_history->addRemoveWarp(*w);
    edit->scene->doorPointsSync(warpId, true);

    for(int i = 0; i < edit->LvlData.doors.size(); i++)
    {
        if(edit->LvlData.doors[i].meta.array_id == warpId)
        {
            edit->LvlData.doors.removeAt(i);
            break;
        }
    }

    ui->warpsList->removeItem(ui->warpsList->currentIndex());

    if(ui->warpsList->count() <= 0)
        setWarpRemoveButtonEnabled(false);

    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::duplicateWarpEntry()
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();
    auto *w = findWarp(edit->LvlData, warpId);
    Q_ASSERT(w);

    LevelDoor newDoor = *w;
    newDoor.meta.array_id = edit->LvlData.doors_array_id++;
    newDoor.meta.index = static_cast<unsigned int>(edit->LvlData.doors.size());
    newDoor.isSetIn = false;
    newDoor.isSetOut = false;

    edit->LvlData.doors.push_back(newDoor);
    edit->LvlData.meta.modified = true;

    edit->scene->m_history->addAddWarp(static_cast<int>(newDoor.meta.array_id),
                                       ui->warpsList->count(),
                                       static_cast<int>(newDoor.meta.index));

    ui->warpsList->addItem(doorTitle(newDoor), newDoor.meta.array_id);
    ui->warpsList->setCurrentIndex(ui->warpsList->count() - 1);
    ui->entryRemove->setEnabled(true);
}

void LvlWarpBox::on_WarpSetEntrance_clicked()
{
    //placeDoorEntrance
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    bool placed = false;
    unsigned int array_id = 0;
    unsigned int warpId = getWarpId();

    LogDebugQD("Warp: Trying to find a warp " + QString::number(warpId));
    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        placed = w->isSetIn;
        array_id = w->meta.array_id;
        LogDebugQD("Warp: Placed warp found: " + QString::number(array_id));
    }

    if(placed)
    {
        LogDebugQD("Warp: Go to location: " + QString::number(w->ix) + "x" + QString::number(w->iy));
        edit->goTo(w->ix, w->iy, true, QPoint(0, 0), true);
        //deselect all and select placed one
        LogDebugQD("Warp: Clear scene selection");
        edit->scene->clearSelection();
        for(QGraphicsItem *item : edit->scene->items())
        {
            if(item->data(ITEM_TYPE).toString() == "Door_enter")
            {
                if(item->data(ITEM_ARRAY_ID).toUInt() == array_id)
                {
                    LogDebugQD("Warp: Select the warp entry");
                    item->setSelected(true);
                    break;
                }
            }
        }

        LogDebugQD("Warp: found a placed warp entrance");
        return;
    }

    mw()->resetEditmodeButtons();

    edit->scene->clearSelection();
    edit->changeCursor(LevelEdit::MODE_PlaceItem);
    edit->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
    edit->scene->setItemPlacer(4, warpId, LvlPlacingItems::DOOR_Entrance);

    mw()->dock_LvlItemProps->hide();

    edit->setFocus();
}

void LvlWarpBox::on_WarpSetExit_clicked()
{
    //placeDoorEntrance
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    bool placed = false;
    unsigned int array_id = 0;
    unsigned int warpId = getWarpId();

    LogDebugQD("Warp: Trying to find a warp " + QString::number(warpId));
    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        placed = w->isSetOut;
        array_id = w->meta.array_id;
        LogDebugQD("Warp: Placed warp found: " + QString::number(array_id));
    }

    if(placed)
    {
        LogDebugQD("Warp: Go to location: " + QString::number(w->ox) + "x" + QString::number(w->oy));
        edit->goTo(w->ox, w->oy, true, QPoint(0, 0), true);
        //deselect all and select placed one
        LogDebugQD("Warp: Clear scene selection");
        edit->scene->clearSelection();
        for(QGraphicsItem *item : edit->scene->items())
        {
            if(item->data(ITEM_TYPE).toString() == "Door_exit")
            {
                if(item->data(ITEM_ARRAY_ID).toUInt() == array_id)
                {
                    LogDebugQD("Warp: Select the warp entry");
                    item->setSelected(true);
                    break;
                }
            }
        }

        LogDebugQD("Warp: found a placed warp exit");
        return;
    }

    mw()->resetEditmodeButtons();

    edit->scene->clearSelection();
    edit->changeCursor(LevelEdit::MODE_PlaceItem);
    edit->scene->SwitchEditingMode(LvlScene::MODE_PlacingNew);
    edit->scene->setItemPlacer(4, warpId, LvlPlacingItems::DOOR_Exit);

    mw()->dock_LvlItemProps->hide();

    mw()->activeLvlEditWin()->setFocus();
}

void LvlWarpBox::on_WarpLayer_currentIndexChanged(const QString &arg1)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QVariant> dirData;
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        dirData.push_back(w->layer);
        dirData.push_back(arg1);
        w->layer = arg1;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_LAYER,
            QVariant(dirData));
    edit->scene->doorPointsSync(warpId);
    edit->scene->applyLayersVisible();
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpEnterEvent_currentIndexChanged(const QString &arg1)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    QList<QVariant> dirData;
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        dirData.push_back(w->event_enter);
        dirData.push_back(arg1);
        w->event_enter = arg1;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_EV_WARP_ENTER,
            QVariant(dirData));
    edit->scene->doorPointsSync((unsigned int)ui->warpsList->currentData().toInt());
    edit->scene->applyLayersVisible();
    edit->LvlData.meta.modified = true;
}


// ////////// Flags///////////
void LvlWarpBox::on_WarpTwoWay_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->two_way = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_TWOWAY,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpNoVehicles_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->novehicles = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_NOVEHICLE,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpAllowNPC_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->allownpc = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_ALLOWNPC,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpLock_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->locked = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_LOCKED,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpBombNeed_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->need_a_bomb = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_NEED_A_BOMB,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpSpecialStateOnly_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->special_state_required = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_W_SPECIAL_STATE_REQUIRED,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_warpNeedsFloor_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->stood_state_required = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_W_NEEDS_FLOOR,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}


/////Door props
void LvlWarpBox::on_WarpType_currentIndexChanged(int index)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> warpTypeData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        warpTypeData.push_back(w->type);
        warpTypeData.push_back(index);
        w->type = index;
    }

    ui->WarpEntranceGrp->setEnabled(index == LevelDoor::WARP_PIPE);
    ui->WarpExitGrp->setEnabled(index == LevelDoor::WARP_PIPE);

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_WARPTYPE,
            QVariant(warpTypeData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_transitEffect_currentIndexChanged(int index)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> warpTransitTypeData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        warpTransitTypeData.push_back(w->transition_effect);
        warpTransitTypeData.push_back(index);
        w->transition_effect = index;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_TRANSITTYPE,
            QVariant(warpTransitTypeData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_transitEffect_customContextMenuRequested(const QPoint &p)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();

    QMenu m;
    QAction *applyToAll = m.addAction(tr("Apply to all"));

    QAction *answer = m.exec(ui->transitEffect->mapToGlobal(p));

    unsigned int warpId = getWarpId();
    auto *w = findWarp(edit->LvlData, warpId);
    Q_ASSERT(w);

    if(answer == applyToAll)
    {
        int ret = QMessageBox::question(this,
                                        tr("Apply to all entries"),
                                        tr("Do you really want to apply this setting to all objects?"),
                                        QMessageBox::Yes|QMessageBox::No);
        if(ret != QMessageBox::Yes)
            return;

        int transition_effect = w->transition_effect;
        for(auto &d : edit->LvlData.doors)
        {
            QList<QVariant> warpTransitTypeData;
            warpTransitTypeData.push_back(d.transition_effect);
            warpTransitTypeData.push_back(transition_effect);

            d.transition_effect = transition_effect;

            edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
                    HistorySettings::SETTING_TRANSITTYPE,
                    QVariant(warpTransitTypeData));
            edit->scene->doorPointsSync(d.meta.array_id);
        }
        edit->LvlData.meta.modified = true;
    }
}

void LvlWarpBox::on_WarpNeedAStars_valueChanged(int arg1)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> starData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        starData.push_back(w->stars);
        starData.push_back(arg1);
        w->stars = arg1;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_NEEDASTAR,
            QVariant(starData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;

}

void LvlWarpBox::on_WarpNeedAStarsMsg_editingFinished()
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> starData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        QString msg = ui->WarpNeedAStarsMsg->text();
        starData.push_back(w->stars_msg);
        starData.push_back(msg);
        w->stars_msg = msg;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_NEEDASTAR_MSG,
            QVariant(starData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpHideStars_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->star_num_hide = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_HIDE_STAR_NUMBER,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::entrance_clicked(int direction)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> dirData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        dirData.push_back(w->idirect);
        dirData.push_back(direction);
        w->idirect = direction;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_ENTRDIR,
            QVariant(dirData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::exit_clicked(int direction)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> dirData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        dirData.push_back(w->odirect);
        dirData.push_back(direction);
        w->odirect = direction;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_EXITDIR,
            QVariant(dirData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}


void LvlWarpBox::on_WarpEnableCannon_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->cannon_exit = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_ENABLE_CANNON,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpCannonSpeed_valueChanged(double arg1)
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> starData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        starData.push_back(w->cannon_exit_speed);
        starData.push_back((float)arg1);
        w->cannon_exit_speed = arg1;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_CANNON_SPEED,
            QVariant(starData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}


void LvlWarpBox::on_WarpToMapX_editingFinished()//_textEdited(const QString &arg1)
{
    if(m_lockSettings)
        return;

    if(!ui->WarpToMapX->isModified())
        return;

    ui->WarpToMapX->setModified(false);

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    QString arg1 = ui->WarpToMapX->text();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        if(arg1.isEmpty())
            w->world_x = -1;
        else
            w->world_x = arg1.toInt();
        edit->LvlData.meta.modified = true;
    }

    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpToMapY_editingFinished()//_textEdited(const QString &arg1)
{
    if(m_lockSettings)
        return;

    if(!ui->WarpToMapY->isModified())
        return;
    ui->WarpToMapY->setModified(false);

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    QString arg1 = ui->WarpToMapY->text();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        if(arg1.isEmpty())
            w->world_y = -1;
        else
            w->world_y = arg1.toInt();
        edit->LvlData.meta.modified = true;
    }

    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpGetXYFromWorldMap_clicked()
{
    if(m_lockSettings)
        return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QString woldMapsPath = mw()->activeLvlEditWin()->LvlData.meta.path;
    QString woldMapsFile;
    QStringList filters;
    QStringList files;
    QDir levelDir(woldMapsPath);

    filters << "*.wld" << "*.wldx";
    levelDir.setSorting(QDir::Name);
    levelDir.setNameFilters(filters);

    files = levelDir.entryList(filters);
    if(files.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("World map files not found"),
                             tr("You haven't available world map files with this level file.\n"
                                "Please, put this level file with a world map, "
                                "or create new world map in the same fomder with this level file.\n"
                                "File path: %1").arg(woldMapsPath), QMessageBox::Ok);
        return;
    }

    bool ok = true;
    if(files.count() == 1)
        woldMapsFile = files.first();
    else
    {
        woldMapsFile = QInputDialog::getItem(this,
                                             tr("Select world map file"),
                                             tr("Found more than one world map files.\n"
                                                "Please, select necessary world map in a list:"),
                                             files, 0, false, &ok);
    }

    if(woldMapsFile.isEmpty() || !ok)
        return;

    QString wldPath = QString("%1/%2")
                      .arg(woldMapsPath)
                      .arg(woldMapsFile);


    QFile file(wldPath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this,
                              tr("File open error"),
                              tr("Can't open the file!"),
                              QMessageBox::Ok);
        return;
    }
    WorldData FileData;
    if(!FileFormats::OpenWorldFile(wldPath, FileData))
    {
        mw()->formatErrorMsgBox(wldPath,
                                FileData.meta.ERROR_info,
                                FileData.meta.ERROR_linenum,
                                FileData.meta.ERROR_linedata);
        return;
    }

    std::unique_ptr<WLD_SetPoint> pointDialog;
    pointDialog.reset(new WLD_SetPoint(mw()));

    pointDialog->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    pointDialog->setGeometry(util::alignToScreenCenter(pointDialog->size()));

    if(pointDialog->loadFile(FileData, mw()->configs, GlobalSettings::LvlOpts))
    {
        pointDialog->resetPosition();
        if(ui->WarpToMapX->text().isEmpty() || ui->WarpToMapY->text().isEmpty())
            pointDialog->m_mapPointIsNull = true;
        else
        {
            pointDialog->pointSelected(
                QPoint(ui->WarpToMapX->text().toInt(),
                       ui->WarpToMapY->text().toInt())
            );
            pointDialog->goTo(ui->WarpToMapX->text().toInt() + 16,
                              ui->WarpToMapY->text().toInt() + 16,
                              QPoint(-qRound(qreal(pointDialog->gViewPort()->width()) / 2),
                                     -qRound(qreal(pointDialog->gViewPort()->height()) / 2))
                             );
            pointDialog->m_scene->m_pointSelector.setPoint(pointDialog->m_mapPoint);
        }

        if(pointDialog->exec() == QDialog::Accepted)
        {
            ui->WarpToMapX->setText(QString::number(pointDialog->m_mapPoint.x()));
            ui->WarpToMapY->setText(QString::number(pointDialog->m_mapPoint.y()));
            ui->WarpToMapX->setModified(true);
            ui->WarpToMapY->setModified(true);
            on_WarpToMapX_editingFinished();
            on_WarpToMapY_editingFinished();
        }
    }
}


/////Door mode (Level Entrance / Level Exit)
void LvlWarpBox::on_WarpLevelExit_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> extraData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    bool exists = false;
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        exists = true;
        extraData.push_back(checked);
        if(checked)
        {
            extraData.push_back(static_cast<int>(w->ox));
            extraData.push_back(static_cast<int>(w->oy));
        }
        w->lvl_o = checked;
    }

    if(!exists)
        return;

    //Disable placing door point, if it not avaliable
    ui->WarpSetEntrance->setEnabled((!w->lvl_o && !w->lvl_i) || (w->lvl_o && !w->lvl_i));
    //Disable placing door point, if it not avaliable
    ui->WarpSetExit->setEnabled((!w->lvl_o && !w->lvl_i) || w->lvl_i);

    bool iPlaced = w->isSetIn;
    bool oPlaced = w->isSetOut;

    edit->scene->doorPointsSync((unsigned int)ui->warpsList->currentData().toInt());

    //Unset placed point, if not it avaliable
    if(!((!w->lvl_o && !w->lvl_i) || w->lvl_i))
    {
        oPlaced = false;
        ui->WarpExitPlaced->setChecked(false);
        w->ox = w->ix;
        w->oy = w->iy;
    }

    w->isSetIn = iPlaced;
    w->isSetOut = oPlaced;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_LEVELEXIT,
            QVariant(extraData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpLevelEntrance_clicked(bool checked)
{
    if(m_lockSettings) return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> extraData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    bool exists = false;
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        exists = true;
        extraData.push_back(checked);
        if(checked)
        {
            extraData.push_back(static_cast<int>(w->ix));
            extraData.push_back(static_cast<int>(w->iy));
        }
        w->lvl_i = checked;
    }

    if(!exists) return;

    //Disable placing door point, if it not avaliable
    ui->WarpSetEntrance->setEnabled((!w->lvl_o && !w->lvl_i) || (w->lvl_o && !w->lvl_i));
    //Disable placing door point, if it not avaliable
    ui->WarpSetExit->setEnabled((!w->lvl_o && !w->lvl_i) || w->lvl_i);

    bool iPlaced = w->isSetIn;
    bool oPlaced = w->isSetOut;

    edit->scene->doorPointsSync((unsigned int)ui->warpsList->currentData().toInt());

    //Unset placed point, if not it avaliable
    if(!((!w->lvl_o && !w->lvl_i) || (w->lvl_o && !w->lvl_i)))
    {
        iPlaced = false;
        ui->WarpEntrancePlaced->setChecked(false);
        w->ix = w->ox;
        w->iy = w->oy;
    }

    w->isSetIn = iPlaced;
    w->isSetOut = oPlaced;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_LEVELENTR,
            QVariant(extraData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}


void LvlWarpBox::on_WarpBrowseLevels_clicked()
{
    if(m_lockSettings) return;

    QString dirPath;
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    if(!edit) return;

    dirPath = edit->LvlData.meta.path;

    if(edit->isUntitled())
    {
        QMessageBox::information(this,
                                 tr("Please save the file"),
                                 tr("Please save the file before selecting levels."),
                                 QMessageBox::Ok);
        return;
    }

    LevelFileList levelList(dirPath, ui->WarpLevelFile->text());
    if(levelList.exec() == QDialog::Accepted)
    {
        ui->WarpLevelFile->setText(levelList.currentFile());
        ui->WarpLevelFile->setModified(true);
        on_WarpLevelFile_editingFinished();
    }
}


void LvlWarpBox::on_WarpHideLevelEnterScreen_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->hide_entering_scene = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_HIDE_LEVEL_ENTER_SCENE,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpAllowNPC_IL_clicked(bool checked)
{
    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->allownpc_interlevel = checked;

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_ALLOWNPC_IL,
            QVariant(checked));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

unsigned int LvlWarpBox::getWarpId()
{
    QVariant w = ui->warpsList->currentData();
    if(!w.isValid())
        return 0;
    return w.toUInt();
}

void LvlWarpBox::on_WarpLevelFile_editingFinished()
{
    if(m_lockSettings) return;

    if(!ui->WarpLevelFile->isModified()) return;
    ui->WarpLevelFile->setModified(false);

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
        w->lname = ui->WarpLevelFile->text();

    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

void LvlWarpBox::on_WarpToExitNu_valueChanged(int arg1)
{
    if(m_lockSettings) return;

    if(mw()->activeChildWindow() != MainWindow::WND_Level)
        return;

    QList<QVariant> warpToData;
    LevelEdit *edit = mw()->activeLvlEditWin();
    unsigned int warpId = getWarpId();

    auto *w = findWarp(edit->LvlData, warpId);
    if(w)
    {
        warpToData.push_back((int)w->warpto);
        warpToData.push_back(arg1);
        w->warpto = arg1;
    }

    edit->scene->m_history->addChangeWarpSettings(static_cast<int>(warpId),
            HistorySettings::SETTING_LEVELWARPTO,
            QVariant(warpToData));
    edit->scene->doorPointsSync(warpId);
    edit->LvlData.meta.modified = true;
}

QComboBox *LvlWarpBox::getWarpList()
{
    return ui->warpsList;
}

void LvlWarpBox::removeItemFromWarpList(int index)
{
    ui->warpsList->removeItem(index);
}


void LvlWarpBox::setWarpRemoveButtonEnabled(bool isEnabled)
{
    ui->entryRemove->setEnabled(isEnabled);
}

