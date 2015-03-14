/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef LVL_FILEDATA_H
#define LVL_FILEDATA_H

#include <QVector>
#include <QString>
#include <QStringList>

#include "meta_filedata.h"

//////////////////////Level file Data//////////////////////
struct LevelSection
{
    int id;
    long size_top;
    long size_bottom;
    long size_left;
    long size_right;
    unsigned int music_id;
    long bgcolor;
    bool IsWarp;
    bool OffScreenEn;
    unsigned int background;
    bool noback;
    bool underwater;
    QString music_file;

    //Editing:
    long PositionX;
    long PositionY;
};

struct PlayerPoint
{
    unsigned int id;
    long x;
    long y;
    long h;
    long w;
    int direction;
};

struct LevelBlock
{
    long x;
    long y;
    long h;
    long w;
    unsigned long id; //Block ID
    long npc_id;
    bool invisible;
    bool slippery;
    QString layer;
    QString event_destroy;
    QString event_hit;
    QString event_no_more;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct LevelBGO
{
    //SMBX64
    long x;
    long y;
    unsigned long id; //Block ID
    QString layer;

    //Extended
    enum zmodes{
        Background2=-2,
        Background1=-1,
        ZDefault=0,
        Foreground1=1,
        Foreground2=2
    };

    int   z_mode;//!< Mode of displaying of BGO
    qreal z_offset; //Offset of Z-value from default

    //editing
    long smbx64_sp;
    long smbx64_sp_apply; // Used only in save file process
    unsigned int array_id;
    unsigned int index;

};

struct LevelNPC
{
    long x;
    long y;
    int direct;
    unsigned long id;
    long special_data;
    long special_data2;
    bool generator;
    int generator_direct;
    int generator_type;
    int generator_period;
    QString msg;
    bool friendly;
    bool nomove;
    bool legacyboss;
    QString layer;
    QString event_activate;
    QString event_die;
    QString event_talk;
    QString event_nomore;
    QString attach_layer;

    //editing
    unsigned int array_id;
    unsigned int index;
    bool is_star;
};

struct LevelDoor
{
    long ix;    //Entrance x
    long iy;    //Entrance y
    bool isSetIn;//Entrance is placed

    long ox;    //Exit x
    long oy;    //Exit y
    bool isSetOut;//Exit is placed

    int idirect;
    int odirect;
    int type;
    QString lname;
    long warpto;
    bool lvl_i;
    bool lvl_o;
    long world_x;
    long world_y;
    int stars;
    QString layer;
    bool unknown;
    bool novehicles;
    bool allownpc;
    bool locked;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct LevelPhysEnv
{
    long x;
    long y;
    long h;
    long w;
    long unknown;
    bool quicksand;
    QString layer;

    //editing
    unsigned int array_id;
    unsigned int index;
};

struct LevelLayer
{
    QString name;
    bool hidden;
    bool locked;

    //editing
    unsigned int array_id;
};

struct LevelEvent_layers
{
    QString hide;
    QString show;
    QString toggle;
};

struct LevelEvent_Sets
{
    long music_id;
    long background_id;
    long position_left;
    long position_top;
    long position_bottom;
    long position_right;
};

struct LevelSMBX64Event
{
    QString name;
    QString msg;
    long sound_id;
    long end_game;
    QVector<LevelEvent_layers > layers;

    bool nosmoke;
    QStringList layers_hide;
    QStringList layers_show;
    QStringList layers_toggle;

    QVector<LevelEvent_Sets > sets;
    QString trigger;
    long trigger_timer;

    bool ctrl_up;
    bool ctrl_down;
    bool ctrl_left;
    bool ctrl_right;
    bool ctrl_jump;
    bool ctrl_altjump;
    bool ctrl_run;
    bool ctrl_altrun;

    bool ctrl_start;
    bool ctrl_drop;

    bool autostart;

    QString movelayer;
    float layer_speed_x;
    float layer_speed_y;
    float move_camera_x;
    float move_camera_y;
    long scroll_section;

    //editing
    unsigned int array_id;
};


struct LevelData
{
    int stars;
    bool ReadFileValid;
    QString LevelName;
    QVector<LevelSection > sections;       //Sections
    QVector<PlayerPoint > players;         //Players
    QVector<LevelBlock > blocks;           //Blocks
    unsigned int blocks_array_id;   //latest array_id
    QVector<LevelBGO > bgo;                //Background objects
    unsigned int bgo_array_id;   //latest array_id
    QVector<LevelNPC > npc;                //NPCs
    unsigned int npc_array_id;   //latest array_id
    QVector<LevelDoor > doors;            //Warps and Doors
    unsigned int doors_array_id;   //latest array_id
    QVector<LevelPhysEnv > physez;            //Physical Environment zones
    unsigned int physenv_array_id;   //latest array_id
    QVector<LevelLayer > layers;          //Layers
    unsigned int layers_array_id;   //latest array_id
    QVector<LevelSMBX64Event > events;          //Events
    unsigned int events_array_id;   //latest array_id

    //meta:
    MetaData metaData;

    //editing:
    int CurSection;
    bool playmusic;
    bool modified;
    bool untitled;
    bool smbx64strict;
    QString filename;
    QString path;

    //Useful functions
    bool eventIsExist(QString title);
    bool layerIsExist(QString title);
};



#endif // LVL_FILEDATA_H
