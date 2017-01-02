/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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
#include "../common_features/npc_animator.h"
#include "../common_features/matrix_animator.h"
#include "../common_features/data_array.h"

#include "setup_load_screen.h"
#include "setup_wld_scene.h"
#include "setup_lvl_scene.h"
#include "setup_title_screen.h"
#include "setup_credits_screen.h"

#include "obj_block.h"
#include "obj_bgo.h"
#include "obj_npc.h"
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
    FontsSetup() :
        double_pixled(false)
    {}
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
    MessageBoxSetup():
        borderWidth(2),
        box_padding(10),
        font_id(-1)
    {}
    QString sprite;
    int borderWidth;
    double  box_padding;
    QString font_name;
    int     font_id;
    QString font_color;
    GlColor font_rgba;
};

struct MenuBoxSetup
{
    MenuBoxSetup():
        borderWidth(2),
        box_padding(10),
        title_font_id(-1)
    {}
    QString sprite;
    int     borderWidth;
    double  box_padding;
    QString title_font_name;
    int     title_font_id;
    QString title_font_color;
    GlColor title_font_rgba;
};

struct MenuSetup
{
    MenuSetup() :
        selector(":selector.png"),
        scrollerUp(":scroll_up.png"),
        scrollerDown(":scroll_down.png"),
        item_height(32),
        font_offset(4),
        font_id(-1)
    {}
    QString selector;
    QString scrollerUp;
    QString scrollerDown;
    int     item_height;
    int     font_offset;
    QString font_name;
    int     font_id;
};

struct ScriptsSetup
{
    //Script in the level custom folder
    QString lvl_local;
    //Script in the same folder as level
    QString lvl_common;
    //Script in the world map custom folder
    QString wld_local;
    //Script in the same folder as world map
    QString wld_common;
};

template<class obj_T>
void loadCustomConfig(PGE_DataArray<obj_T> &container,
                      unsigned long ID,
                      CustomDirManager &dir,
                      QString fileName = "file",
                      QString section = "item",
                      bool (*loaderFunk)(obj_T &, QString, obj_T *, QString, QSettings *) = nullptr
                     )
{
    bool isDefault = false;
    QString file = dir.getCustomFile(QString(fileName + "-%1.ini").arg(ID), &isDefault);

    if(file.isEmpty())
        file = dir.getCustomFile(QString(fileName + "-%1.txt").arg(ID), &isDefault);

    if(!file.isEmpty() && !isDefault)
    {
        obj_T &sceneSetup = container[ID];

        if(loaderFunk)
            loaderFunk(sceneSetup, section, &sceneSetup, file, nullptr);
    }
}

////////////////////Common items///////////////////////////

namespace ConfigManager
{
    extern DataFolders dirs;
    extern QString      config_id;
    extern std::string  config_name;
    extern QString      config_dir;
    extern QString      data_dir;
    extern unsigned int default_grid;

    //Scripts setup
    extern ScriptsSetup setup_Scripts;

    //Common Data
    extern QList<PGE_Texture > common_textures;

    extern unsigned int screen_width;
    extern unsigned int screen_height;

    enum screenType
    {
        SCR_Static = 0, //Static / Scalable screen
        SCR_Dynamic   //Dynamic screen
    };
    extern screenType screen_type;

    //Fonts
    extern FontsSetup setup_fonts;
    //curors
    extern MainCursors setup_cursors;
    //MessageBox setup
    extern MessageBoxSetup setup_message_box;
    //MenuBox setup
    extern MenuBoxSetup    setup_menu_box;
    //Menu setup
    extern MenuSetup setup_menus;


    //LoadingScreen
    //friend struct LoadingScreenSetup;
    extern LoadingScreenSetup setup_LoadingScreen;

    //Title Screen
    //friend struct TitleScreenSetup;
    extern TitleScreenSetup setup_TitleScreen;

    //World map data
    //friend struct WorldMapSetup;
    extern WorldMapSetup setup_WorldMap;

    //Level data
    //friend struct LevelSetup;
    extern LevelSetup setup_Level;

    //Credits Screen
    //friend struct CreditsScreenSetup;
    extern CreditsScreenSetup setup_CreditsScreen;


    /********Music and sounds*******/
    bool loadMusic(QString rootPath, QString iniFile, bool isCustom = false);
    bool loadDefaultMusics();
    QString getWldMusic(unsigned long musicID, QString customMusic = "");
    QString getLvlMusic(unsigned long musicID, QString customMusic = "");
    QString getSpecialMusic(unsigned long musicID);

    extern unsigned long music_custom_id;
    extern unsigned long music_w_custom_id;
    extern PGE_DataArray<obj_music> main_music_lvl;
    extern PGE_DataArray<obj_music> main_music_wld;
    extern PGE_DataArray<obj_music> main_music_spc;

    bool loadDefaultSounds();
    bool loadSound(QString rootPath, QString iniFile, bool isCustom = false);
    QString getSound(unsigned long sndID);
    long getSoundByRole(obj_sound_role::roles role);
    bool loadSoundRolesTable();

    extern PGE_DataArray<obj_sound > main_sound;
    extern PGE_DataArray<long > main_sound_table;
    void buildSoundIndex();
    void clearSoundIndex();
    extern QVector<obj_sound_index > main_sfx_index;

    bool musicIniChanged();
    bool soundIniChanged();
    extern QString music_lastIniFile;
    extern QString sound_lastIniFile;
    extern bool music_lastIniFile_changed;
    extern bool sound_lastIniFile_changed;
    /********Music and sounds*******/


    void setConfigPath(QString p);
    //Load settings
    bool loadBasics();
    bool unloadLevelConfigs();
    bool unloadWorldConfigs();
    void unluadAll();

    /*================================Level config Data===========================*/

    /*****Level blocks************/
    bool loadLevelBlocks();
    bool loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getBlockTexture(unsigned long blockID);
    /*****************************/
    extern PGE_DataArray<obj_block>   lvl_block_indexes;
    extern CustomDirManager Dir_Blocks;
    extern QList<SimpleAnimator > Animator_Blocks;
    /*****Level blocks************/

    /*****Level BGO************/
    bool loadLevelBGO();
    bool loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getBgoTexture(unsigned long bgoID);
    /*****************************/
    extern PGE_DataArray<obj_bgo>   lvl_bgo_indexes;
    extern CustomDirManager Dir_BGO;
    extern QList<SimpleAnimator > Animator_BGO;
    /*****Level BGO************/

    /*****Level NPC************/
    bool loadLevelNPC();
    bool loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    void loadNpcTxtConfig(unsigned long npcID);
    int  getNpcTexture(unsigned long npcID);
    /*****************************/
    extern PGE_DataArray<obj_npc>   lvl_npc_indexes;
    extern NPC_GlobalSetup          g_setup_npc;
    extern CustomDirManager Dir_NPC;
    extern CustomDirManager Dir_NPCScript;
    extern QList<AdvNpcAnimator > Animator_NPC;//!< Global NPC Animators (just for a coins, vines, not for activing NPC's!)
    /*****Level NPC************/




    /*****Level Backgrounds************/
    bool loadLevelBackG();
    bool loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getBGTexture(unsigned long bgID, bool isSecond = false);
    /*****************************/
    extern PGE_DataArray<obj_BG>   lvl_bg_indexes;
    extern CustomDirManager Dir_BG;
    extern QList<SimpleAnimator > Animator_BG;
    /*****Level Backgrounds************/
    /*================================Level config Data===end=====================*/

    /*================================World config Data===========================*/
    /*****World Tiles************/
    bool loadWorldTiles();
    bool loadWorldTile(obj_w_tile &tile, QString section, obj_w_tile *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getTileTexture(unsigned long tileID);
    /*****************************/
    extern PGE_DataArray<obj_w_tile>    wld_tiles;
    extern CustomDirManager         Dir_Tiles;
    extern QList<SimpleAnimator >   Animator_Tiles;
    /*****World Tiles************/

    /*****World Scenery************/
    bool loadWorldScenery();
    bool loadWorldScenery(obj_w_scenery &scene, QString section, obj_w_scenery *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getSceneryTexture(unsigned long sceneryID);
    /*****************************/
    extern PGE_DataArray<obj_w_scenery>     wld_scenery;
    extern CustomDirManager         Dir_Scenery;
    extern QList<SimpleAnimator >   Animator_Scenery;
    /*****World Scenery************/

    /*****World Paths************/
    bool loadWorldPaths();
    bool loadWorldPath(obj_w_path &path, QString section, obj_w_path *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getWldPathTexture(unsigned long pathID);
    /*****************************/
    extern PGE_DataArray<obj_w_path>   wld_paths;
    extern CustomDirManager         Dir_WldPaths;
    extern QList<SimpleAnimator >   Animator_WldPaths;
    /*****World Paths************/

    /*****World Levels************/
    bool loadWorldLevels();
    bool loadWorldLevel(obj_w_level &level, QString section, obj_w_level *merge_with = 0, QString iniFile = "", QSettings *setup = nullptr);
    int  getWldLevelTexture(unsigned long levelID);
    /*****************************/
    extern PGE_DataArray<obj_w_level>   wld_levels;
    extern CustomDirManager         Dir_WldLevel;
    extern QList<SimpleAnimator >   Animator_WldLevel;
    extern wld_levels_Markers        marker_wlvl;
    /*****World Levels************/



    /*================================World config Data===end=====================*/

    /*================================Common config Data===========================*/

    /*****Level Effects************/
    bool loadLevelEffects();
    int  getEffectTexture(unsigned long effectID);
    /*****************************/
    extern PGE_DataArray<obj_effect>   lvl_effects_indexes;
    extern Effects_GlobalSetup         g_setup_effects;
    extern CustomDirManager Dir_EFFECT;
    /*****Level Effects************/


    /********Playable characters*******/
    int  getLvlPlayerTexture(unsigned long playerID, unsigned long stateID);
    int  getWldPlayerTexture(unsigned long playerID, unsigned long stateID);
    void resetPlayableTexuresState();        //!< Sets all 'isInit' state to false for all textures for level textutes
    void resetPlayableTexuresStateWld();     //!< Same but for world map player images
    bool loadPlayableCharacters();           //!< Load lvl_characters.ini file
    /*****************************/
    extern PGE_DataArray<obj_player > playable_characters;
    extern CustomDirManager Dir_PlayerWld;
    extern CustomDirManager Dir_PlayerLvl;
    extern CustomDirManager Dir_PlayerScript;
    /********Playable characters*******/

    /*================================Common config Data===end=====================*/

    /***********Texture banks*************/
    extern QList<PGE_Texture > level_textures;
    extern QList<PGE_Texture > world_textures;
    /***********Texture banks*************/

    void addError(QString bug, QtMsgType level = QtWarningMsg);

    extern QStringList errorsList;

    QString PathLevelBGO();
    QString PathLevelBG();
    QString PathLevelBlock();
    QString PathLevelNPC();
    QString PathLevelNPCScript();
    QString PathLevelEffect();
    QString PathLevelPlayable();
    QString PathLevelPlayerScript();

    QString PathCommonGFX();

    QString PathScript();

    QString PathWorldTiles();
    QString PathWorldScenery();
    QString PathWorldPaths();
    QString PathWorldLevels();
    QString PathWorldPlayable();

    QString PathMusic();
    QString PathSound();

    QString clearMusTrack(QString path);
};


#endif // CONFIG_MANAGER_H
