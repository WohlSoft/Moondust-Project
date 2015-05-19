/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../common_features/matrix_animator.h"

#include "setup_load_screen.h"
#include "setup_wld_scene.h"
#include "setup_title_screen.h"

#include "obj_block.h"
#include "obj_bgo.h"
#include "obj_bg.h"
#include "obj_player.h"
#include "obj_effect.h"
#include "obj_wld_items.h"

#include "obj_sound.h"
#include "obj_music.h"

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

/*****************Fonts********************/
struct FontsSetup
{
    bool double_pixled;
    QString fontname;
    QString rasterFontsFile;
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
    int     item_height;
    int     font_offset;
    QString font_name;
    int     font_id;
};

////////////////////Common items///////////////////////////

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
    friend struct LoadingScreenSetup;
    static LoadingScreenSetup setup_LoadingScreen;

    //Title Screen
    friend struct TitleScreenSetup;
    static TitleScreenSetup setup_TitleScreen;

    //World map data
    friend struct WorldMapSetup;
    static WorldMapSetup setup_WorldMap;


    /********Music and sounds*******/
    static bool loadMusic(QString rootPath, QString iniFile, bool isCustom=false);
    static QString getWldMusic(unsigned long musicID, QString customMusic="");
    static QString getLvlMusic(unsigned long musicID, QString customMusic="");
    static QString getSpecialMusic(unsigned long musicID);

    static unsigned long music_custom_id;
    static unsigned long music_w_custom_id;
    static QHash<int, obj_music> main_music_lvl;
    static QHash<int, obj_music> main_music_wld;
    static QHash<int, obj_music> main_music_spc;

    static bool loadSound(QString rootPath, QString iniFile, bool isCustom=false);
    static QString getSound(unsigned long sndID);
    static long getSoundByRole(obj_sound_role::roles role);
    static bool loadSoundRolesTable();

    static QHash<int, obj_sound > main_sound;
    static QHash<obj_sound_role::roles, long > main_sound_table;
    static void buildSoundIndex();
    static void clearSoundIndex();
    static QVector<obj_sound_index > main_sfx_index;
    /********Music and sounds*******/


    static void setConfigPath(QString p);
    //Load settings
    static bool loadBasics();
    static bool unloadLevelConfigs();
    static bool unloadWorldConfigs();
    static void unluadAll();

    /*================================Level config Data===========================*/

    /*****Level blocks************/
    static bool loadLevelBlocks();
    static long getBlockTexture(long blockID);
    /*****************************/
    static QMap<long, obj_block>   lvl_block_indexes;
    static CustomDirManager Dir_Blocks;
    static QList<SimpleAnimator > Animator_Blocks;
    /*****Level blocks************/

    /*****Level BGO************/
    static bool loadLevelBGO();
    static long getBgoTexture(long bgoID);
    /*****************************/
    static QMap<long, obj_bgo>   lvl_bgo_indexes;
    static CustomDirManager Dir_BGO;
    static QList<SimpleAnimator > Animator_BGO;
    /*****Level BGO************/


    /*****Level Backgrounds************/
    static bool loadLevelBackG();
    static long getBGTexture(long bgID, bool isSecond=false);
    /*****************************/
    static QMap<long, obj_BG>   lvl_bg_indexes;
    static CustomDirManager Dir_BG;
    static QList<SimpleAnimator > Animator_BG;
    /*****Level Backgrounds************/
    /*================================Level config Data===end=====================*/

    /*================================World config Data===========================*/
    /*****World Tiles************/
    static bool loadWorldTiles();
    static long getTileTexture(long tileID);
    /*****************************/
    static QMap<long, obj_w_tile>   wld_tiles;
    static CustomDirManager         Dir_Tiles;
    static QList<SimpleAnimator >   Animator_Tiles;
    /*****World Tiles************/

    /*****World Scenery************/
    static bool loadWorldScenery();
    static long getSceneryTexture(long sceneryID);
    /*****************************/
    static QMap<long, obj_w_scenery>   wld_scenery;
    static CustomDirManager         Dir_Scenery;
    static QList<SimpleAnimator >   Animator_Scenery;
    /*****World Scenery************/

    /*****World Paths************/
    static bool loadWorldPaths();
    static long getWldPathTexture(long pathID);
    /*****************************/
    static QMap<long, obj_w_path>   wld_paths;
    static CustomDirManager         Dir_WldPaths;
    static QList<SimpleAnimator >   Animator_WldPaths;
    /*****World Paths************/

    /*****World Levels************/
    static bool loadWorldLevels();
    static long getWldLevelTexture(long levelID);
    /*****************************/
    static QMap<long, obj_w_level>   wld_levels;
    static CustomDirManager         Dir_WldLevel;
    static QList<SimpleAnimator >   Animator_WldLevel;
    static wld_levels_Markers        marker_wlvl;
    /*****World Levels************/



    /*================================World config Data===end=====================*/

    /*****Level Effects************/
    static bool loadLevelEffects();
    static long getEffectTexture(long effectID);
    /*****************************/
    static QMap<long, obj_effect>   lvl_effects_indexes;
    static CustomDirManager Dir_EFFECT;
    static QList<SimpleAnimator > Animator_EFFECT;
    /*****Level Effects************/


    /********Playable characters*******/
    static long getLvlPlayerTexture(long playerID, int stateID);
    static void resetPlayableTexuresState();        //!< Sets all 'isInit' state to false for all textures for level textutes
    static void resetPlayableTexuresStateWld();     //!< Same but for world map player images
    static bool loadPlayableCharacters();           //!< Load lvl_characters.ini file
    /*****************************/
    static QHash<int, obj_player > playable_characters;
    static CustomDirManager Dir_PlayerWld;
    static CustomDirManager Dir_PlayerLvl;
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
    static QString PathLevelPlayable();

    static QString PathCommonGFX();

    static QString PathWorldTiles();
    static QString PathWorldScenery();
    static QString PathWorldPaths();
    static QString PathWorldLevels();
    static QString PathWorldPlayable();

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
    static QString playerLvlPath;
    static QString playerWldPath;

    static QString tilePath;
    static QString scenePath;
    static QString pathPath;
    static QString wlvlPath;

    static QString commonGPath;

    static void refreshPaths();
    static bool loadEngineSettings(); //!< Load engine.ini file
};


#endif // CONFIG_MANAGER_H
