/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "config_block.h"

#include <QSettings>
#include "../image_size.h"
#include "../../number_limiter.h"
#include "../../csv_2_number_array.h"

bool BlockSetup::parse(QSettings *setup, QString blockImgPath, int defaultGrid, BlockSetup *merge_with, QString *error)
{
    int errCode=PGE_ImageInfo::ERR_OK;
    QString section;
    /*************Buffers*********************/
    QString tmpStr;
    int w, h;
    /*************Buffers*********************/
    if(!setup)
    {
        if(error)
            *error = "setup QSettings is null!";
        return false;
    }
    section = setup->group();

    name =       setup->value("name", (merge_with? merge_with->name : section) ).toString();
    if( name.isEmpty() )
    {
        if(error)
            *error = QString("%1 Item name isn't defined").arg(section.toUpper());
        return false;
    }

    group =      setup->value("group", (merge_with? merge_with->group : "_NoGroup")).toString();
    category =   setup->value("category", (merge_with? merge_with->category : "_Other")).toString();
    grid =       setup->value("grid", (merge_with ? merge_with->grid : defaultGrid)).toUInt();

    image_n =           setup->value("image", (merge_with ? merge_with->image_n : "")).toString();

    if(!merge_with && !PGE_ImageInfo::getImageSize(blockImgPath + image_n, &w, &h, &errCode))
    {
        if(error)
        {
            switch(errCode)
            {
            case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
                *error = "Unsupported or corrupted file format: " + blockImgPath + image_n;
                break;
            case PGE_ImageInfo::ERR_NOT_EXISTS:
                *error = "image file is not exist: " + blockImgPath + image_n;
                break;
            case PGE_ImageInfo::ERR_CANT_OPEN:
                *error = "Can't open image file: " + blockImgPath + image_n;
                break;
            }
        }
        return false;
    }
    mask_n = PGE_ImageInfo::getMaskName(image_n);

    sizable =                setup->value("sizable", (merge_with? merge_with->sizable : false)).toBool();
    danger =                 setup->value("danger", (merge_with? merge_with->danger : 0)).toInt();
    collision =              setup->value("collision", (merge_with? merge_with->collision : 1)).toInt();
    slopeslide =             setup->value("slope-slide", (merge_with? merge_with->slopeslide : 0)).toBool();
    phys_shape =             setup->value("shape-type", (merge_with? merge_with->phys_shape : 0)).toInt();
    lava =                   setup->value("lava", (merge_with? merge_with->lava : false)).toBool();
    destroyable =            setup->value("destroyable", (merge_with? merge_with->destroyable : false)).toBool();
    destroyable_by_bomb =    setup->value("destroyable-by-bomb", (merge_with? merge_with->destroyable_by_bomb : false)).toBool();
    destroyable_by_fireball= setup->value("destroyable-by-fireball", (merge_with? merge_with->destroyable_by_fireball : false)).toBool();

    tmpStr = setup->value("spawn-on-destroy", "0").toString();
    if(tmpStr != "0" )
    {
        QStringList tmpStrL =  tmpStr.split("-", QString::SkipEmptyParts);
        if(tmpStrL.size()==2)
        {
            if(tmpStrL[0]=="npc")
                spawn_obj = 1;
            else
            if(tmpStrL[0]=="block")
                 spawn_obj = 2;
            else
            if(tmpStrL[0]=="bgo")
                 spawn_obj = 3;
            // 1 - NPC, 2 - block, 3 - BGO
            spawn_obj_id = tmpStrL[1].toUInt();
        }
        else // if syntax error in config
        {
            spawn = false;
            spawn_obj = 0;
            spawn_obj_id = 0;
        }
    }
    else
    {
        spawn = (merge_with? merge_with->spawn : 0);
        spawn_obj = (merge_with? merge_with->spawn_obj : 0);
        spawn_obj_id = (merge_with? merge_with->spawn_obj_id : 0);
    }

    effect  =                setup->value("destroy-effect", (merge_with? merge_with->effect : 1)).toInt();
    bounce  =                setup->value("bounce", merge_with? merge_with->bounce : false).toBool();
    hitable =                setup->value("hitable", merge_with? merge_with->hitable : false).toBool();
    transfororm_on_hit_into= setup->value("transform-onhit-into", merge_with? merge_with->transfororm_on_hit_into : 2).toInt();

    static int switchID=0;
    switch_Button    = setup->value("switch-button", merge_with? merge_with->switch_Button : false).toBool();
    switch_Block     = setup->value("switch-block", merge_with? merge_with->switch_Block : false).toBool();
    switch_ID        = setup->value("switch-id", merge_with? merge_with->switch_ID : switchID++).toInt();
    switch_transform = setup->value("switch-transform", merge_with? merge_with->switch_transform : 1).toInt();

    plSwitch_Button = setup->value("player-switch-button", merge_with? merge_with->plSwitch_Button : false).toBool();
    plSwitch_Button_id = setup->value("player-switch-button-id", merge_with? merge_with->plSwitch_Button_id : 0).toInt();
        NumberLimiter::apply(plSwitch_Button_id, 0);
    plSwitch_frames_true.clear();
    plSwitch_frames_false.clear();
    frame_sequence.clear();

    if(plSwitch_Button)
    {
        QString framesTrue = setup->value("player-switch-frames-true", "").toString();
        QString framesFalse = setup->value("player-switch-frames-false", "").toString();
        if(!framesTrue.isEmpty())
            CSV2NumArray(framesTrue, plSwitch_frames_true, 0);
        else if(merge_with)
            plSwitch_frames_true = merge_with->plSwitch_frames_true;

        if(!framesFalse.isEmpty())
            CSV2NumArray(framesFalse, plSwitch_frames_false, 0);
        else if(merge_with)
            plSwitch_frames_false = merge_with->plSwitch_frames_false;
        frame_sequence = plSwitch_frames_false;
    }

    plFilter_Block= setup->value("player-filter-block", merge_with? merge_with->plSwitch_Button : false).toBool();
    plFilter_Block_id= setup->value("player-filter-block-id", merge_with? merge_with->plFilter_Block_id : 0 ).toInt();
        NumberLimiter::apply(plFilter_Block_id, 0);
    plFilter_frames_true.clear();
    plFilter_frames_false.clear();

    if(plFilter_Block)
    {
        QString framesTrue = setup->value("player-filter-frames-true", "").toString();
        QString framesFalse = setup->value("player-filter-frames-false", "").toString();
        if(!framesTrue.isEmpty())
            CSV2NumArray(framesTrue, plFilter_frames_true, 0);
        else
            if(merge_with)
                plFilter_frames_true = merge_with->plFilter_frames_true;

        if(!framesFalse.isEmpty())
            CSV2NumArray(framesFalse,  plFilter_frames_false, 0);
        else if(merge_with)
            plFilter_frames_false = merge_with->plFilter_frames_false;
        frame_sequence = plFilter_frames_false;
    }

    if(setup->contains("view"))
    {
        tmpStr = setup->value("view", "").toString();
        view = 0;//0 by default
        if(tmpStr=="background")
            view = 0;
        else if(tmpStr=="foreground")
            view = 1;
    } else {
        view = merge_with ? merge_with->view : 0;
    }

    animated =               setup->value("animated", (merge_with? merge_with->animated : false)).toBool();
    animation_rev =          setup->value("animation-reverse", (merge_with? merge_with->animation_rev : false)).toBool(); //Reverse animation
    animation_bid =          setup->value("animation-bidirectional", (merge_with? merge_with->animation_bid : false)).toBool(); //Bidirectional animation
    frames =                 setup->value("frames", (merge_with? merge_with->frames : 1)).toUInt();
        NumberLimiter::apply(frames, 1u);
    framespeed =             setup->value("framespeed", (merge_with? merge_with->framespeed : 125)).toInt();
        NumberLimiter::apply(framespeed, 1);

    hit_sound_id = setup->value("hit-sound-id", merge_with? merge_with->hit_sound_id : 0).toInt();
        NumberLimiter::apply(hit_sound_id, 0);
    destroy_sound_id = setup->value("destroy-sound-id", merge_with? merge_with->destroy_sound_id : 0).toInt();
        NumberLimiter::apply(destroy_sound_id, 0);

    frame_h =               (animated ? (uint(h)/frames) : uint(h) );

    //Retreiving frame sequence from playable character switch/filter blocks
    if(!plSwitch_Button && !plFilter_Block)
    {
        if(setup->contains("frame-sequence"))
            CSV2NumArray(setup->value("frame-sequence", "0").toString(), frame_sequence, 0);
        else if(merge_with)
            frame_sequence = merge_with->frame_sequence;
    }

    display_frame =          setup->value("display-frame", 0).toUInt();

    long iTmp;
    iTmp =  setup->value("default-invisible", (merge_with? merge_with->default_invisible : -1)).toInt();
        default_invisible = (iTmp>=0);
        default_invisible_value = (iTmp>=0) ? bool(iTmp) : false;

    iTmp = setup->value("default-slippery", (merge_with? merge_with->default_slippery : -1)).toInt();
        default_slippery = (iTmp>=0);
        default_slippery_value  = (iTmp>=0) ? bool(iTmp) : false;

    iTmp = setup->value("default-npc-content", (merge_with? merge_with->default_content : -1)).toInt();
        default_content = (iTmp>=0);
        default_content_value   = (iTmp>=0) ? (iTmp<1000? iTmp*-1 : iTmp-1000) : 0;

    return true;
}
