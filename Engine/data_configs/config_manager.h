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

#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "../common_features/app_path.h"
#include "../common_features/pge_texture.h"
#include "../common_features/simple_animator.h"

#include "obj_block.h"
#include "obj_bgo.h"
#include "obj_bg.h"
#include "obj_player.h"
#include "obj_effect.h"

#include <QMap>
#include <QSettings>
#include <QFile>

#include "custom_data.h"


struct DataFolders
{
    QString worlds;

    QString music;
    QString sounds;

    QString glevel;
    QString gworld;
    QString gplayble;

    QString gcommon;

    QString gcustom;
};


struct WorldMapData
{
    QString backgroundImg;
    int viewport_x; //World map view port
    int viewport_y;
    int viewport_w;
    int viewport_h;
    enum titleAlign{
        align_left=0,
        align_center,
        align_right
    };

    int title_x; //Title of level
    int title_y;
    int title_w; //max width of title
    titleAlign title_align;

    bool points_en;
    int points_x;
    int points_y;

    bool health_en;
    int health_x;
    int health_y;

    bool star_en;
    int star_x;
    int star_y;

    bool coin_en;
    int coin_x;
    int coin_y;

    bool portrait_en;
    int portrait_x;
    int portrait_y;
};

/**********Loading screen******************/
struct FontsSetup
{
    bool double_pixled;
    QString fontname;
};

/******************************************/

/**********Loading screen******************/
struct LoadingScreenAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int x;
    int y;
};

struct LoadingScreenSetup
{
    QString backgroundImg;
    QColor backgroundColor;
    int updateDelay;
    QVector<LoadingScreenAdditionalImage > AdditionalImages;
};
/******************************************/


/************Title screen******************/
struct TitleScreenAdditionalImage
{
    enum align{
        NO_ALIGN=0,
        LEFT_ALIGN,
        TOP_ALIGN,
        RIGHT_ALIGN,
        BOTTOM_ALIGN,
        CENTER_ALIGN
    };

    QString imgFile;
    bool animated;
    int frames;
    unsigned int framespeed;
    align align_to;
    int x;
    int y;
    bool center_x;
    bool center_y;
};

struct TitleScreenSetup
{
    QString backgroundImg;
    QColor backgroundColor;
    QVector<TitleScreenAdditionalImage > AdditionalImages;
};
/******************************************/


/**************Cursors*********************/
struct MainCursors
{
    QString normal;
    QString rubber;
};
/******************************************/

struct MessageBoxSetup
{
    QString sprite;
    int borderWidth;
};

struct MenuSetup
{
    QString selector;
    QString scrollerUp;
    QString scrollerDown;
};

////////////////////Common items///////////////////////////
struct obj_music
{
    unsigned long id;
    QString name;
    QString file;
};

struct obj_sound
{
    unsigned long id;
    QString name;
    QString file;
    bool hidden;
};




class ConfigManager
{
public:
    ConfigManager();

    static DataFolders dirs;
    static QString config_id;
    static QString config_dir;
    static QString data_dir;
    static int default_grid;

    //Common Data
    static QList<PGE_Texture > common_textures;

    static unsigned int screen_width;
    static unsigned int screen_height;

    enum screenType{
        SCR_Static=0, //Static / Scalable screen
        SCR_Dynamic   //Dynamic screen
    };
    static screenType screen_type;

    //Fonts
    static FontsSetup setup_fonts;
    //curors
    static MainCursors setup_cursors;
    //MessageBox setup
    static MessageBoxSetup setup_message_box;
    //Menu setup
    static MenuSetup setup_menus;


    //LoadingScreen
    static LoadingScreenSetup setup_LoadingScreen;

    //Title Screen
    static TitleScreenSetup setup_TitleScreen;

    //World map data
    static WorldMapData setup_WorldMap;


    static unsigned long music_custom_id;
    static unsigned long music_w_custom_id;
    static QList<obj_music > main_music_lvl;
    static QList<obj_music > main_music_wld;
    static QList<obj_music > main_music_spc;

    static QList<obj_sound > main_sound;


    static void setConfigPath(QString p);
    //Load settings
    static bool loadBasics();
    static bool unloadLevelConfigs();


    //Level config Data

    /*****Level blocks************/
    static bool loadLevelBlocks();
    static long getBlockTexture(long blockID);
    /*****************************/
    static QList<obj_block >     lvl_blocks;
    static QMap<long, obj_block>   lvl_block_indexes;
    static CustomDirManager Dir_Blocks;
    static QList<SimpleAnimator > Animator_Blocks;
    /*****Level blocks************/

    /*****Level BGO************/
    static bool loadLevelBGO();
    static long getBgoTexture(long bgoID);
    /*****************************/
    static QList<obj_bgo >     lvl_bgo;
    static QMap<long, obj_bgo>   lvl_bgo_indexes;
    static CustomDirManager Dir_BGO;
    static QList<SimpleAnimator > Animator_BGO;
    /*****Level BGO************/


    /*****Level Backgrounds************/
    static bool loadLevelBackG();
    static long getBGTexture(long bgID, bool isSecond=false);
    /*****************************/
    static QList<obj_BG >     lvl_bg;
    static QMap<long, obj_BG>   lvl_bg_indexes;
    static CustomDirManager Dir_BG;
    static QList<SimpleAnimator > Animator_BG;
    /*****Level Backgrounds************/



    /*****Level Effects************/
    static bool loadLevelEffects();
    static long getEffectTexture(long effectID);
    /*****************************/
    static QList<obj_effect >    lvl_effects;
    static QMap<long, obj_effect>   lvl_effects_indexes;
    static CustomDirManager Dir_EFFECT;
    static QList<SimpleAnimator > Animator_EFFECT;
    /*****Level Effects************/


    /********Playable characters*******/
    static QMap<int, obj_player > playable_characters;
    /********Playable characters*******/



    /***********Texture banks*************/
    static QList<PGE_Texture > level_textures;
    static QList<PGE_Texture > world_textures;
    /***********Texture banks*************/



    static void addError(QString bug, QtMsgType level=QtWarningMsg);

    static QStringList errorsList;

    static QString PathLevelBGO();
    static QString PathLevelBG();
    static QString PathLevelBlock();
    static QString PathLevelNPC();
    static QString PathLevelEffect();

    static QString PathCommonGFX();

    static QString PathWorldTiles();
    static QString PathWorldScenery();
    static QString PathWorldPaths();
    static QString PathWorldLevels();

    static QString PathWorldMusic();
    static QString PathWorldSound();


private:
    static void checkForImage(QString &imgPath, QString root);
    //special paths
    static QString imgFile, imgFileM;
    static QString tmpstr;
    static QStringList tmp;

    static unsigned long total_data;
    static QString bgoPath;
    static QString BGPath;
    static QString blockPath;
    static QString npcPath;
    static QString effectPath;

    static QString tilePath;
    static QString scenePath;
    static QString pathPath;
    static QString wlvlPath;

    static QString commonGPath;


};


#endif // CONFIG_MANAGER_H
