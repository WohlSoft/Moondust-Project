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
    float   box_padding;
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
    int borderWidth;
    float   box_padding;
    QString title_font_name;
    int     title_font_id;
    QString title_font_color;
    GlColor title_font_rgba;
};

struct MenuSetup
{
    MenuSetup() :
        selector(":/images/selector.png"),
        scrollerUp(":/images/scroll_up.png"),
        scrollerDown(":/images/scroll_down.png"),
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

template<class obj_T>
void loadCustomConfig(PGE_DataArray<obj_T> &container,
                      int ID,
                      CustomDirManager& dir,
                      QString fileName = "file",
                      QString section = "item",
                      bool (*loaderFunk)(obj_T&, QString, obj_T*, QString, QSettings*) = nullptr
                      )
{
    bool isDefault=false;
    QString file = dir.getCustomFile(QString(fileName+"-%1.ini").arg(ID), &isDefault);
    if(file.isEmpty())
        file = dir.getCustomFile(QString(fileName+"-%1.txt").arg(ID), &isDefault);
    if( !file.isEmpty() && !isDefault )
    {
        obj_T &sceneSetup = container[ID];
        if(loaderFunk)
            loaderFunk(sceneSetup, section, &sceneSetup, file, nullptr);
    }
}

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
    //MenuBox setup
    static MenuBoxSetup    setup_menu_box;
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

    //Level data
    friend struct LevelSetup;
    static LevelSetup setup_Level;

    //Credits Screen
    friend struct CreditsScreenSetup;
    static CreditsScreenSetup setup_CreditsScreen;


    /********Music and sounds*******/
    static bool loadMusic(QString rootPath, QString iniFile, bool isCustom=false);
    static bool loadDefaultMusics();
    static QString getWldMusic(unsigned long musicID, QString customMusic="");
    static QString getLvlMusic(unsigned long musicID, QString customMusic="");
    static QString getSpecialMusic(unsigned long musicID);

    static unsigned long music_custom_id;
    static unsigned long music_w_custom_id;
    static PGE_DataArray<obj_music> main_music_lvl;
    static PGE_DataArray<obj_music> main_music_wld;
    static PGE_DataArray<obj_music> main_music_spc;

    static bool loadDefaultSounds();
    static bool loadSound(QString rootPath, QString iniFile, bool isCustom=false);
    static QString getSound(unsigned long sndID);
    static long getSoundByRole(obj_sound_role::roles role);
    static bool loadSoundRolesTable();

    static PGE_DataArray<obj_sound > main_sound;
    static PGE_DataArray<long > main_sound_table;
    static void buildSoundIndex();
    static void clearSoundIndex();
    static QVector<obj_sound_index > main_sfx_index;

    static bool musicIniChanged();
    static bool soundIniChanged();
    static QString music_lastIniFile;
    static QString sound_lastIniFile;
    static bool music_lastIniFile_changed;
    static bool sound_lastIniFile_changed;
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
    static bool loadLevelBlock(obj_block &sblock, QString section, obj_block *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getBlockTexture(long blockID);
    /*****************************/
    static PGE_DataArray<obj_block>   lvl_block_indexes;
    static CustomDirManager Dir_Blocks;
    static QList<SimpleAnimator > Animator_Blocks;
    /*****Level blocks************/

    /*****Level BGO************/
    static bool loadLevelBGO();
    static bool loadLevelBGO(obj_bgo &sbgo, QString section, obj_bgo *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getBgoTexture(long bgoID);
    /*****************************/
    static PGE_DataArray<obj_bgo>   lvl_bgo_indexes;
    static CustomDirManager Dir_BGO;
    static QList<SimpleAnimator > Animator_BGO;
    /*****Level BGO************/

    /*****Level NPC************/
    static bool loadLevelNPC();
    static bool loadLevelNPC(obj_npc &snpc, QString section, obj_npc *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static void loadNpcTxtConfig(long npcID);
    static long getNpcTexture(long npcID);
    /*****************************/
    static PGE_DataArray<obj_npc>   lvl_npc_indexes;
    static NPC_GlobalSetup          g_setup_npc;
    static CustomDirManager Dir_NPC;
    static CustomDirManager Dir_NPCScript;
    static QList<AdvNpcAnimator > Animator_NPC;//!< Global NPC Animators (just for a coins, vines, not for activing NPC's!)
    /*****Level NPC************/




    /*****Level Backgrounds************/
    static bool loadLevelBackG();
    static bool loadLevelBackground(obj_BG &sbg, QString section, obj_BG *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getBGTexture(long bgID, bool isSecond=false);
    /*****************************/
    static PGE_DataArray<obj_BG>   lvl_bg_indexes;
    static CustomDirManager Dir_BG;
    static QList<SimpleAnimator > Animator_BG;
    /*****Level Backgrounds************/
    /*================================Level config Data===end=====================*/

    /*================================World config Data===========================*/
    /*****World Tiles************/
    static bool loadWorldTiles();
    static bool loadWorldTile(obj_w_tile &tile, QString section, obj_w_tile *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getTileTexture(long tileID);
    /*****************************/
    static PGE_DataArray<obj_w_tile>   wld_tiles;
    static CustomDirManager         Dir_Tiles;
    static QList<SimpleAnimator >   Animator_Tiles;
    /*****World Tiles************/

    /*****World Scenery************/
    static bool loadWorldScenery();
    static bool loadWorldScenery(obj_w_scenery &scene, QString section, obj_w_scenery *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getSceneryTexture(long sceneryID);
    /*****************************/
    static PGE_DataArray<obj_w_scenery>   wld_scenery;
    static CustomDirManager         Dir_Scenery;
    static QList<SimpleAnimator >   Animator_Scenery;
    /*****World Scenery************/

    /*****World Paths************/
    static bool loadWorldPaths();
    static bool loadWorldPath(obj_w_path &path, QString section, obj_w_path *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getWldPathTexture(long pathID);
    /*****************************/
    static PGE_DataArray<obj_w_path>   wld_paths;
    static CustomDirManager         Dir_WldPaths;
    static QList<SimpleAnimator >   Animator_WldPaths;
    /*****World Paths************/

    /*****World Levels************/
    static bool loadWorldLevels();
    static bool loadWorldLevel(obj_w_level &level, QString section, obj_w_level *merge_with=0, QString iniFile="", QSettings *setup=nullptr);
    static long getWldLevelTexture(long levelID);
    /*****************************/
    static PGE_DataArray<obj_w_level>   wld_levels;
    static CustomDirManager         Dir_WldLevel;
    static QList<SimpleAnimator >   Animator_WldLevel;
    static wld_levels_Markers        marker_wlvl;
    /*****World Levels************/



    /*================================World config Data===end=====================*/

    /*================================Common config Data===========================*/

    /*****Level Effects************/
    static bool loadLevelEffects();
    static long getEffectTexture(long effectID);
    /*****************************/
    static PGE_DataArray<obj_effect>   lvl_effects_indexes;
    static Effects_GlobalSetup         g_setup_effects;
    static CustomDirManager Dir_EFFECT;
    /*****Level Effects************/


    /********Playable characters*******/
    static long getLvlPlayerTexture(long playerID, int stateID);
    static long getWldPlayerTexture(long playerID, int stateID);
    static void resetPlayableTexuresState();        //!< Sets all 'isInit' state to false for all textures for level textutes
    static void resetPlayableTexuresStateWld();     //!< Same but for world map player images
    static bool loadPlayableCharacters();           //!< Load lvl_characters.ini file
    /*****************************/
    static PGE_DataArray<obj_player > playable_characters;
    static CustomDirManager Dir_PlayerWld;
    static CustomDirManager Dir_PlayerLvl;
    static CustomDirManager Dir_PlayerScript;
    /********Playable characters*******/

    /*================================Common config Data===end=====================*/

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
    static QString PathLevelNPCScript();
    static QString PathLevelEffect();
    static QString PathLevelPlayable();
    static QString PathLevelPlayerScript();

    static QString PathCommonGFX();

    static QString PathScript();

    static QString PathWorldTiles();
    static QString PathWorldScenery();
    static QString PathWorldPaths();
    static QString PathWorldLevels();
    static QString PathWorldPlayable();

    static QString PathMusic();
    static QString PathSound();

    static QString clearMusTrack(QString path);
private:
    static void checkForImage(QString &imgPath, QString root);

    //special paths
    static QString imgFile, imgFileM;
    static QString tmpstr;
    static QStringList tmp;

    static QString bgoPath;
    static QString BGPath;
    static QString blockPath;
    static QString npcPath;
    static QString npcScriptPath;
    static QString effectPath;
    static QString playerLvlPath;
    static QString playerWldPath;
    static QString playerScriptPath;

    static QString tilePath;
    static QString scenePath;
    static QString pathPath;
    static QString wlvlPath;

    static QString commonGPath;

    static QString scriptPath;

    static void refreshPaths();
    static bool loadEngineSettings(); //!< Load engine.ini file
};


#endif // CONFIG_MANAGER_H
