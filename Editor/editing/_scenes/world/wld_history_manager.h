/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef WLD_HISTORY_MANAGER_H
#define WLD_HISTORY_MANAGER_H

#include <QObject>
#include "items/item_tile.h"
#include "items/item_scene.h"
#include "items/item_path.h"
#include "items/item_level.h"
#include "items/item_music.h"
#include "items/item_point.h"

class WldHistoryManager : public QObject
{
    Q_OBJECT

    friend class WldScene;
    WldScene* m_scene;
    bool    historyChanged;
    int     historyIndex;
    QList<QSharedPointer<IHistoryElement> > operationList;
public:
    explicit WldHistoryManager(WldScene* scene, QObject* parent = nullptr);

    void addRemoveHistory(WorldData removedItems);
    void addPlaceHistory(WorldData placedItems);
    void addOverwriteHistory(WorldData removedItems, WorldData placedItems);
    void addMoveHistory(WorldData sourceMovedItems, WorldData targetMovedItems);
    void addChangeWorldSettingsHistory(HistorySettings::WorldSettingSubType subtype, QVariant extraData);
    void addChangeSettingsHistory(WorldData modifiedItems, HistorySettings::WorldSettingSubType subType, QVariant extraData);
    void addRotateHistory(WorldData rotatedItems, WorldData unrotatedItems);
    void addFlipHistory(WorldData flippedItems, WorldData unflippedItems);
    void addTransformHistory(WorldData transformedItems, WorldData sourceItems);

    //history information
    int  getHistroyIndex();
    bool canUndo();
    bool canRedo();

public slots:
    //history modifiers
    void historyBack();
    void historyForward();
    void updateHistoryBuffer();

signals:
    void refreshHistoryButtons();
    void showStatusMessage(QString text, int delay=2000);
};

#endif // WLD_HISTORY_MANAGER_H
