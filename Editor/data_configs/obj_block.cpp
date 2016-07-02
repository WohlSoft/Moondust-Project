/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QFile>

#include <common_features/graphics_funcs.h>
#include <common_features/util.h>
#include <main_window/global_settings.h>

#include "data_configs.h"


obj_block::obj_block()
{
    isValid     = false;
    animator_id = 0;
    cur_image   = nullptr;
}


void obj_block::copyTo(obj_block &block)
{
    /* for internal usage */
    block.isValid         = isValid;
    block.animator_id     = animator_id;
    block.cur_image       = cur_image;
    if(cur_image==nullptr)
        block.cur_image   = &image;
    block.frame_h         = frame_h;
    /* for internal usage */

    block.id=id;
    block.image_n=image_n;
    block.mask_n=mask_n;
    block.name=name;
    //    grid=32				; 32 | 16 Default="32"
    block.grid=grid;
    block.group=group;
    block.category=category;
    block.sizable=sizable;
    block.danger=danger;
    block.collision=collision;
    block.slopeslide=slopeslide;
    block.phys_shape=phys_shape;
    block.lava=lava;
    block.destroyable=destroyable;
    block.destroyable_by_bomb=destroyable_by_bomb;
    block.destroyable_by_fireball=destroyable_by_fireball;
    block.spawn=spawn; //split string by "-" in != "0"
    block.spawn_obj=spawn_obj; // 1 - NPC, 2 - block, 3 - BGO
    block.spawn_obj_id=spawn_obj_id;
    block.effect=effect;
    block.bounce=bounce;
    block.hitable=hitable;
    block.transfororm_on_hit_into=transfororm_on_hit_into;

    block.view=view;
    block.animated=animated;
    block.animation_rev = animation_rev; //Reverse animation
    block.animation_bid = animation_bid; //Bidirectional animation
    block.frames=frames;
    block.framespeed=framespeed;

    block.frame_h=frame_h; //Hegth of the frame. Calculating automatically

    block.frame_sequence = frame_sequence;

    block.display_frame=display_frame;

    //Editor defaults
    block.default_slippery=default_slippery; //Slippery flag
    block.default_slippery_value=default_slippery_value;

    block.default_invisible=default_invisible; //Invisible flag
    block.default_invisible_value=default_invisible_value;

    block.default_content=default_content; //Content value
    block.default_content_value=default_content_value;

}

/*!
 * \brief Loads signe block configuration
 * \param sblock Target block structure
 * \param section Name of INI-section where look for a block
 * \param merge_with Source of already loaded block structure to provide default settings per every block
 * \param iniFile INI-file where look for a block
 * \param setup loaded INI-file descriptor to load from global nested INI-file
 * \return true on success loading, false if error has occouped
 */
bool dataconfigs::loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with, QString iniFile, QSettings *setup)
{
    bool valid=true;
    bool internal=!setup;
    QString tmpStr, errStr;
    if(internal) setup=new QSettings(iniFile, QSettings::IniFormat);

    if(!openSection( setup, section ))
        return false;

    sblock.name =       setup->value("name", (merge_with? merge_with->name : section) ).toString();

        if(sblock.name=="")
        {
            addError(QString("%1 Item name isn't defined").arg(section.toUpper()));
            valid=false;
            goto abort;
        }

        sblock.group =      setup->value("group", (merge_with? merge_with->group : "_NoGroup")).toString();
        sblock.category =   setup->value("category", (merge_with? merge_with->category : "_Other")).toString();
        sblock.grid =       setup->value("grid", (merge_with ? merge_with->grid : default_grid)).toUInt();

        sblock.image_n =           setup->value("image", (merge_with ? merge_with->image_n : "")).toString();
        /***************Load image*******************/
        GraphicsHelps::loadMaskedImage(blockPath,
           sblock.image_n, sblock.mask_n,
           sblock.image,
           errStr);

        if(!errStr.isEmpty())
        {
            valid=false;
            addError(QString("%1 %2").arg(section.toUpper()).arg(errStr));
            //goto abort;
        }
        /***************Load image*end***************/

        sblock.sizable =                setup->value("sizable", (merge_with? merge_with->sizable : false)).toBool();
        sblock.danger =                 setup->value("danger", (merge_with? merge_with->danger : 0)).toInt();
        sblock.collision =              setup->value("collision", (merge_with? merge_with->collision : 1)).toInt();
        sblock.slopeslide =             setup->value("slope-slide", (merge_with? merge_with->slopeslide : 0)).toBool();
        sblock.phys_shape =             setup->value("shape-type", (merge_with? merge_with->phys_shape : 0)).toInt();
        sblock.lava =                   setup->value("lava", (merge_with? merge_with->lava : false)).toBool();
        sblock.destroyable =            setup->value("destroyable", (merge_with? merge_with->destroyable : false)).toBool();
        sblock.destroyable_by_bomb =    setup->value("destroyable-by-bomb", (merge_with? merge_with->destroyable_by_bomb : false)).toBool();
        sblock.destroyable_by_fireball= setup->value("destroyable-by-fireball", (merge_with? merge_with->destroyable_by_fireball : false)).toBool();

        tmpStr = setup->value("spawn-on-destroy", "0").toString();
        if(tmpStr != "0" )
        {
            QStringList tmpStrL =  tmpStr.split("-", QString::SkipEmptyParts);
            if(tmpStrL.size()==2)
            {
                if(tmpStrL[0]=="npc")
                    sblock.spawn_obj = 1;
                else
                if(tmpStrL[0]=="block")
                     sblock.spawn_obj = 2;
                else
                if(tmpStrL[0]=="bgo")
                     sblock.spawn_obj = 3;
                // 1 - NPC, 2 - block, 3 - BGO
                sblock.spawn_obj_id = tmpStrL[1].toUInt();
            }
            else // if syntax error in config
            {
                sblock.spawn = false;
                sblock.spawn_obj = 0;
                sblock.spawn_obj_id = 0;
            }
        }
        else
        {
            sblock.spawn = (merge_with? merge_with->spawn : 0);
            sblock.spawn_obj = (merge_with? merge_with->spawn_obj : 0);
            sblock.spawn_obj_id = (merge_with? merge_with->spawn_obj_id : 0);
        }

        //sblock.effect=                  setup->value("destroy-effect", (merge_with? merge_with->effect : 1)).toInt();
        //sblock.bounce =                 setup->value("bounce", "0").toBool();
        //sblock.hitable =                setup->value("hitable", "0").toBool();
        //sblock.transfororm_on_hit_into= setup->value("transform-onhit-into", "2").toInt();

        tmpStr = setup->value("view", "0").toString();
        if(tmpStr!="0")
        {
            sblock.view = 0;//0 by default
            if(tmpStr=="background")
                sblock.view = 0;
            else if(tmpStr=="foreground")
                sblock.view = 1;
        } else {
            sblock.view = merge_with ? merge_with->view : 0;
        }

        sblock.animated =               setup->value("animated", (merge_with? merge_with->animated : false)).toBool();
        sblock.animation_rev =          setup->value("animation-reverse", (merge_with? merge_with->animation_rev : false)).toBool(); //Reverse animation
        sblock.animation_bid =          setup->value("animation-bidirectional", (merge_with? merge_with->animation_bid : false)).toBool(); //Bidirectional animation
        sblock.frames =                 setup->value("frames", (merge_with? merge_with->frames : 1)).toUInt();
        sblock.framespeed =             setup->value("framespeed", (merge_with? merge_with->framespeed : 125)).toInt();

        sblock.frame_h =               (sblock.animated ?
                                            uint(qRound(
                                                qreal(sblock.image.height())
                                                /sblock.frames))
                                          : uint(sblock.image.height()) );

        {//Retreiving frame sequence from playable character switch/filter blocks
            tmpStr.clear();
            bool playerSwitch = setup->value("player-switch-button", false).toBool();
            bool playerFilter = setup->value("player-filter-block", false).toBool();
            if(playerSwitch)
            {
                tmpStr = setup->value("player-switch-frames-false", "").toString();
            }
            else if(playerFilter)
            {
                tmpStr = setup->value("player-filter-frames-false", "").toString();
            } else {
                tmpStr = setup->value("frame-sequence", "").toString();
            }
        }

        sblock.frame_sequence.clear();
        if(tmpStr.isEmpty())
        {
            if(merge_with)
                sblock.frame_sequence = merge_with->frame_sequence;
        }
        else
            util::CSV2IntArr(tmpStr, sblock.frame_sequence);

        sblock.display_frame =          setup->value("display-frame", 0).toUInt();

        long iTmp;
        iTmp =  setup->value("default-invisible", (merge_with? merge_with->default_invisible : -1)).toInt();
            sblock.default_invisible = (iTmp>=0);
            sblock.default_invisible_value = (iTmp>=0) ? bool(iTmp) : false;

        iTmp = setup->value("default-slippery", (merge_with? merge_with->default_slippery : -1)).toInt();
            sblock.default_slippery = (iTmp>=0);
            sblock.default_slippery_value  = (iTmp>=0) ? bool(iTmp) : false;

        iTmp = setup->value("default-npc-content", (merge_with? merge_with->default_content : -1)).toInt();
            sblock.default_content = (iTmp>=0);
            sblock.default_content_value   = (iTmp>=0) ? (iTmp<1000? iTmp*-1 : iTmp-1000) : 0;

        sblock.isValid = true;

    abort:
        closeSection(setup);
        if(internal) delete setup;
    return valid;
}


void dataconfigs::loadLevelBlocks()
{
    unsigned int i;

    obj_block sblock;
    unsigned long block_total=0;

    QString block_ini = getFullIniPath("lvl_blocks.ini");
    if(block_ini.isEmpty())
        return;

    QSettings blockset(block_ini, QSettings::IniFormat);
    blockset.setIniCodec("UTF-8");

    main_block.clear();   //Clear old
    //index_blocks.clear();

    if(!openSection(&blockset, "blocks-main")) return;
        block_total = blockset.value("total", 0).toUInt();
        total_data +=block_total;
    closeSection(&blockset);

    emit progressPartNumber(2);
    emit progressMax(int(block_total));
    emit progressValue(0);
    emit progressTitle(QObject::tr("Loading Blocks..."));

    ConfStatus::total_blocks = signed(block_total);

    main_block.allocateSlots(int(block_total));

    if(ConfStatus::total_blocks==0)
    {
        addError(QString("ERROR LOADING lvl_blocks.ini: number of items not define, or empty config"), PGE_LogLevel::Critical);
        return;
    }

    for(i=1; i<=block_total; i++)
    {
        emit progressValue(int(i));

        bool valid = loadLevelBlock(sblock, QString("block-%1").arg(i), 0, "", &blockset);
        sblock.id = i;
        main_block.storeElement(int(i), sblock, valid);

        if( blockset.status()!=QSettings::NoError)
        {
            addError(QString("ERROR LOADING lvl_blocks.ini N:%1 (block-%2)").arg(blockset.status()).arg(i), PGE_LogLevel::Critical);
            break;
        }
    }

    if(unsigned(main_block.stored()) < block_total)
    {
        addError(QString("Not all blocks loaded! Total: %1, Loaded: %2)").arg(block_total).arg(main_block.size()), PGE_LogLevel::Warning);
    }
}
