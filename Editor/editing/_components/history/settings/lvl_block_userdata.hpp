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

#ifndef MOONDUST_LVL_BLOCK_USERDATA_HPP
#define MOONDUST_LVL_BLOCK_USERDATA_HPP

#include "../../../_scenes/level/lvl_history_manager.h"

class BlockHistory_UserData : public HistoryElementCustomSetting
{
public:
    BlockHistory_UserData() : HistoryElementCustomSetting() {}
    virtual ~BlockHistory_UserData() {}

    virtual void undo(const void *sourceItem, QVariant *, QGraphicsItem *item)
    {
        const LevelBlock * it = reinterpret_cast<const LevelBlock*>(sourceItem);
        ItemBlock* ite = qgraphicsitem_cast<ItemBlock*>(item);
        ite->m_data.meta.custom_params = it->meta.custom_params;
        ite->arrayApply();
    }
    virtual void redo(const void *, QVariant *mod, QGraphicsItem *item)
    {
        ItemBlock* ite = qgraphicsitem_cast<ItemBlock*>(item);
        ite->m_data.meta.custom_params = mod->toString();
        ite->arrayApply();
    }
    virtual QString getHistoryName()
    {
        return QObject::tr("Block user data change");
    }
};

#endif //MOONDUST_LVL_BLOCK_USERDATA_HPP
