/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef DATACONFIGS_H
#define DATACONFIGS_H
#include <QVector>
#include <QPixmap>
#include <QBitmap>

struct DataFolders
{
    QString worlds;

    QString music;
    QString sounds;

    QString glevel;
    QString gworld;
    QString gplayble;

    QString gcustom;
};

struct obj_bgo{
    /*
    [background-1]
    name="Smallest bush"		;background name, default="background-%n"
    type="scenery"			;Background type, default="Scenery"
    grid=32				; 32 | 16 Default="32"
    view=background			; background | foreground, default="background"
    image="background-1.gif"	;Image file with level file ; the image mask will be have *m.gif name.
    climbing=0			; default = 0
    animated = 0			; default = 0 - no
    frames = 1			; default = 1
    frame-speed=125			; default = 125 ms, etc. 8 frames per sec
    */
    unsigned long id;
    QString name;
    QString type;
    unsigned int grid;
    unsigned int view;
    QString image_n;
    QString mask_n;
    QPixmap image;
    QBitmap mask;
    bool climbing;
    bool animated;
    unsigned int frames;
    unsigned int framespeed;
};

struct obj_BG{
    /*
[background2-3]
image="background2-3.gif"//
name="Hills (3 + 2)"//
type=double-row//
repeat-h=2//
repeat-v=ZR//
attached=bottom//
animated=0..
frames=1..
magic=0..
magic-strips=1//
magic-splits=0//
magic-speeds=0//
second-image="background2-2.gif"//
second-repeat-h=4
second-repeat-v=ZR
second-attached=overfirst
    */

    unsigned long id;
    QString name;

    QString image_n;

    QPixmap image;
    unsigned int type;//convert from string
    float repeat_h;
    unsigned int repead_v;
    unsigned int attached;
    bool animated;
    unsigned int frames;

    bool magic;
    unsigned int magic_strips;
    QString magic_splits;
    QString magic_speeds;

    QString second_image_n;
    QPixmap second_image;

    float second_repeat_h;
    unsigned int second_repead_v;
    unsigned int second_attached;

};


class dataconfigs
{
public:
    dataconfigs();
    void loadconfigs();
    DataFolders dirs;

    QVector<obj_bgo > main_bgo;
    QVector<obj_BG > main_bg;

};

#endif // DATACONFIGS_H
