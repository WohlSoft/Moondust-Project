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

#ifndef CONFIG_BLOCK_H
#define CONFIG_BLOCK_H

#include <QString>
#include <QList>

class QSettings;

struct BlockSetup
{
    BlockSetup()
    {
        id = 0;
        grid = 32;
    }

    bool parse(QSettings *setup, QString blockImgPath, unsigned int defaultGrid, BlockSetup *merge_with = nullptr, QString *error = nullptr);

    unsigned long   id;
    QString         image_n;
    QString         mask_n;
    QString         name;
    //    grid=32               ; 32 | 16 Default="32"
    unsigned int    grid;
    QString         group;
    QString         category;
    bool            sizable;
    int             danger;
    int             collision;
    bool            slopeslide;
    int             phys_shape;
    enum shapes
    {
        SHAPE_rect = 0,
        /*     __
         *    |  |
         *    |  |
         *    |__|
        */

        SHAPE_triangle_right_bottom = -1,
        /*
         *      /|
         *     / |
         *    /__|
         */

        SHAPE_triangle_left_bottom = 1,
        /*
         *    |\
         *    | \
         *    |__\
         */

        SHAPE_triangle_right_top = -2,
        /*    ___
         *    \  |
         *     \ |
         *      \|
         */

        SHAPE_triangle_left_top = 2,
        /*    ___
         *   |  /
         *   | /
         *   |/
         */

        SHAPE_polygonal = 3,
        SHAPE_cyrcle = 4
    };
    bool            lava;
    bool            destroyable;
    bool            destroyable_by_bomb;
    bool            destroyable_by_fireball;
    bool            spawn; //split string by "-" in != "0"
    int             spawn_obj; // 1 - NPC, 2 - block, 3 - BGO
    enum spawnes
    {
        SPAWN_Nothing = 0,
        SPAWN_NPC = 1,
        SPAWN_Block = 2,
        SPAWN_BGO = 3
    };
    unsigned long   spawn_obj_id;
    unsigned int    effect;
    bool            bounce;
    bool            hitable;
    unsigned int    transfororm_on_hit_into;

    //Toggable Switch blocks
    bool            switch_Button;
    bool            switch_Block;
    unsigned int    switch_ID;
    unsigned int    switch_transform;

    //Playable character Switch/Filter blocks
    bool            plSwitch_Button;
    unsigned int    plSwitch_Button_id; //Target Character ID
    QList<int>      plSwitch_frames_true;
    QList<int>      plSwitch_frames_false;

    bool            plFilter_Block;   //Target Character ID
    unsigned int    plFilter_Block_id;//Target Character ID
    QList<int>      plFilter_frames_true;
    QList<int>      plFilter_frames_false;

    unsigned int    view;
    bool            animated;
    bool            animation_rev; //Reverse animation
    bool            animation_bid; //Bidirectional animation
    unsigned int    frames;
    int             framespeed;

    //! Play custom sound on hit
    int             hit_sound_id;
    //! Play custom sound on destroy
    int             destroy_sound_id;

    unsigned int    frame_h; //Hegth of the frame. Calculating automatically

    QList<int>      frame_sequence;//Custom editor specific frame sequence

    unsigned int    display_frame;

    //Editor defaults
    bool default_slippery; //Slippery flag
    bool default_slippery_value;

    bool default_invisible; //Invisible flag
    bool default_invisible_value;

    bool default_content; //Content value
    long default_content_value;
};


#endif // CONFIG_BLOCK_H
