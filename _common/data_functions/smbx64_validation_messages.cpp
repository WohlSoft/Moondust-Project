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

#include "smbx64_validation_messages.h"
#include <QTranslator>

QString smbx64ErrMsgs(LevelData &lvl, int errorCode)
{
    QString message;
    //Sections limit
    if((errorCode&FileFormats::SMBX64EXC_SECTIONS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 section\n"
        "The maximum number of sections is %2.\n"
        "All boundaries and settings of more than 21 sections will be lost.").arg(lvl.sections.size()).arg(21)+"\n\n";
    }

    //Blocks limit
    if((errorCode&FileFormats::SMBX64EXC_BLOCKS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 blocks\n"
        "The maximum number of blocks is %2.").arg(lvl.blocks.size()).arg(16384)+"\n\n";
    }
    //BGO limits
    if((errorCode&FileFormats::SMBX64EXC_BGOS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Background Objects\n"
        "The maximum number of Background Objects is %2.").arg(lvl.bgo.size()).arg(8000)+"\n\n";
    }
    //NPC limits
    if((errorCode&FileFormats::SMBX64EXC_NPCS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 NPC's\n"
        "The maximum number of NPC's is %2.").arg(lvl.npc.size()).arg(5000)+"\n\n";
    }
    //Warps limits
    if((errorCode&FileFormats::SMBX64EXC_WARPS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Warps\n"
        "The maximum number of Warps is %2.").arg(lvl.doors.size()).arg(199)+"\n\n";
    }
    //Physical Environment zones
    if((errorCode&FileFormats::SMBX64EXC_WATERBOXES)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Water Boxes\n"
        "The maximum number of Water Boxes is %2.").arg(lvl.physez.size()).arg(450)+"\n\n";
    }
    //Layers limits
    if((errorCode&FileFormats::SMBX64EXC_LAYERS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Layers\n"
        "The maximum number of Layers is %2.").arg(lvl.layers.size()).arg(100)+"\n\n";
    }
    //Events limits
    if((errorCode&FileFormats::SMBX64EXC_EVENTS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Events\n"
        "The maximum number of Events is %2.").arg(lvl.events.size()).arg(100)+"\n\n";
    }

    //Append common message part
    if(!message.isEmpty())
    {
        message=QTranslator::tr("A some issues are found on preparing to save SMBX64 Level file format:\n\n%1"
        "Please remove excess elements (or settings) from this level or save file into LVLX format.").arg(message);
    }
    return message;
}

QString smbx64ErrMsgs(WorldData &wld, int errorCode)
{
    QString message;

    //Tiles limit
    if((errorCode&FileFormats::SMBX64EXC_TILES)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Tiles\n"
        "The maximum number of Tiles is %2.").arg(wld.tiles.size()).arg(20000)+"\n\n";
    }
    //Sceneries limit
    if((errorCode&FileFormats::SMBX64EXC_SCENERIES)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Sceneries\n"
        "The maximum number of Sceneries is %2.").arg(wld.scenery.size()).arg(5000)+"\n\n";
    }
    //Paths limit
    if((errorCode&FileFormats::SMBX64EXC_PATHS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Paths\n"
        "The maximum number of Paths is %2.").arg(wld.paths.size()).arg(2000)+"\n\n";
    }
    //Levels limit
    if((errorCode&FileFormats::SMBX64EXC_LEVELS)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Levels\n"
        "The maximum number of Levels is %2.").arg(wld.levels.size()).arg(400)+"\n\n";
    }

    //Music boxes limit
    if((errorCode&FileFormats::SMBX64EXC_MUSICBOXES)!=0)
    {
        message+=
        QTranslator::tr("SMBX64 standard isn't allows to save %1 Music Boxes\n"
        "The maximum number of Music Boxes is %2.").arg(wld.music.size()).arg(1000)+"\n\n";
    }

    //Append common message part
    if(!message.isEmpty())
    {
        message=QTranslator::tr("A some issues are found on preparing to save SMBX64 World map file format:\n\n%1"
        "Please remove excess elements (or settings) from this world map or save file into WLDX format.").arg(message);
    }

    return message;
}
