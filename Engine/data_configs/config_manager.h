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
#include <Utils/vptrlist.h>

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

#include <Utils/strings.h>
#include <fmt/fmt_format.h>

#include "custom_data.h"


struct DataFolders
{
    std::string worlds;

    std::string music;
    std::string sounds;

    std::string glevel;
    std::string gworld;
    std::string gplayble;

    std::string gcommon;

    std::string gcustom;
};

/*****************Fonts********************/
struct FontsSetup
{
    FontsSetup() :
        double_pixled(false)
    {}
    bool double_pixled;
    std::string fontname;
    std::string rasterFontsFile;
};
/******************************************/


/**************Cursors*********************/
struct MainCursors
{
    std::string normal;
    std::string rubber;
};
/******************************************/

struct MessageBoxSetup
{
    MessageBoxSetup():
        borderWidth(2),
        box_padding(10),
        font_id(-1)
    {}
    std::string sprite;
    int         borderWidth;
    double      box_padding;
    std::string font_name;
    int         font_id;
    std::string font_color;
    GlColor     font_rgba;
};

struct MenuBoxSetup
{
    MenuBoxSetup():
        borderWidth(2),
        box_padding(10),
        title_font_id(-1)
    {}
    std::string sprite;
    int         borderWidth;
    double      box_padding;
    std::string title_font_name;
    int         title_font_id;
    std::string title_font_color;
    GlColor     title_font_rgba;
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
    std::string selector;
    std::string scrollerUp;
    std::string scrollerDown;
    int         item_height;
    int         font_offset;
    std::string font_name;
    int         font_id;
};

struct ScriptsSetup
{
    //Script in the level custom folder
    std::string lvl_local;
    //Script in the same folder as level
    std::string lvl_common;
    //Script in the world map custom folder
    std::string wld_local;
    //Script in the same folder as world map
    std::string wld_common;
};

template<class obj_T>
void loadCustomConfig(PGE_DataArray<obj_T> &container,
                      unsigned long ID,
                      CustomDirManager &dir,
                      std::string fileName = "file",
                      std::string section = "item",
                      bool (*loaderFunk)(obj_T &, std::string, obj_T *, std::string, IniProcessing *) = nullptr
                     )
{
    bool isDefault = false;
    std::string file = dir.getCustomFile(fmt::format("{1}-{0}.ini", ID, fileName), &isDefault);

    if(file.empty())
        file = dir.getCustomFile(fmt::format("{1}-{0}.txt", ID, fileName), &isDefault);

    if(!file.empty() && !isDefault)
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
    extern std::string  config_idSTD;
    extern std::string  config_name;
    extern std::string  config_dirSTD;
    extern std::string  data_dirSTD;
    extern unsigned int default_grid;

    typedef VPtrList<PGE_Texture> TexturesBank;

    //Scripts setup
    extern ScriptsSetup setup_Scripts;

    //Common Data
    extern TexturesBank common_textures;

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
    bool loadMusic(std::string rootPath, std::string iniFile, bool isCustom = false);
    bool loadDefaultMusics();
    std::string getWldMusic(unsigned long musicID, std::string customMusic = "");
    std::string getLvlMusic(unsigned long musicID, std::string customMusic = "");
    std::string getSpecialMusic(unsigned long musicID);

    extern unsigned long music_custom_id;
    extern unsigned long music_w_custom_id;
    extern PGE_DataArray<obj_music> main_music_lvl;
    extern PGE_DataArray<obj_music> main_music_wld;
    extern PGE_DataArray<obj_music> main_music_spc;

    bool loadDefaultSounds();
    bool loadSound(std::string rootPath, std::string iniFile, bool isCustom = false);
    std::string getSound(unsigned long sndID);
    long getSoundByRole(obj_sound_role::roles role);
    bool loadSoundRolesTable();

    extern PGE_DataArray<obj_sound > main_sound;
    extern PGE_DataArray<long > main_sound_table;
    void buildSoundIndex();
    void clearSoundIndex();
    extern std::vector<obj_sound_index > main_sfx_index;

    bool musicIniChanged();
    bool soundIniChanged();
    extern std::string music_lastIniFile;
    extern std::string sound_lastIniFile;
    extern bool music_lastIniFile_changed;
    extern bool sound_lastIniFile_changed;
    /********Music and sounds*******/


    void setConfigPath(std::string p);
    //Load settings
    bool loadBasics();
    bool unloadLevelConfigs();
    bool unloadWorldConfigs();
    void unluadAll();

    /*================================Level config Data===========================*/

    typedef VPtrList<SimpleAnimator> AnimatorsArray;

    /*****Level blocks************/
    bool loadLevelBlocks();
    bool loadLevelBlock(obj_block &sblock, std::string section, obj_block *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getBlockTexture(unsigned long blockID);
    /*****************************/
    extern PGE_DataArray<obj_block>   lvl_block_indexes;
    extern CustomDirManager Dir_Blocks;
    extern AnimatorsArray Animator_Blocks;
    /*****Level blocks************/

    /*****Level BGO************/
    bool loadLevelBGO();
    bool loadLevelBGO(obj_bgo &sbgo, std::string section, obj_bgo *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getBgoTexture(unsigned long bgoID);
    /*****************************/
    extern PGE_DataArray<obj_bgo>   lvl_bgo_indexes;
    extern CustomDirManager Dir_BGO;
    extern AnimatorsArray Animator_BGO;
    /*****Level BGO************/

    /*****Level NPC************/
    bool loadLevelNPC();
    bool loadLevelNPC(obj_npc &snpc,
                      std::string section,
                      obj_npc *merge_with = 0,
                      std::string iniFile = "",
                      IniProcessing *setup = nullptr);
    void loadNpcTxtConfig(unsigned long npcID);
    int  getNpcTexture(unsigned long npcID);
    /*****************************/
    extern PGE_DataArray<obj_npc>   lvl_npc_indexes;
    extern NPC_GlobalSetup          g_setup_npc;
    extern CustomDirManager Dir_NPC;
    extern CustomDirManager Dir_NPCScript;
    extern VPtrList<AdvNpcAnimator > Animator_NPC;//!< Global NPC Animators (just for a coins, vines, not for activing NPC's!)
    /*****Level NPC************/




    /*****Level Backgrounds************/
    bool loadLevelBackG();
    bool loadLevelBackground(obj_BG &sbg, std::string section, obj_BG *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getBGTexture(unsigned long bgID, bool isSecond = false);
    /*****************************/
    extern PGE_DataArray<obj_BG>   lvl_bg_indexes;
    extern CustomDirManager Dir_BG;
    extern AnimatorsArray Animator_BG;
    /*****Level Backgrounds************/
    /*================================Level config Data===end=====================*/

    /*================================World config Data===========================*/
    /*****World Tiles************/
    bool loadWorldTiles();
    bool loadWorldTile(obj_w_tile &tile, std::string section, obj_w_tile *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getTileTexture(unsigned long tileID);
    /*****************************/
    extern PGE_DataArray<obj_w_tile>    wld_tiles;
    extern CustomDirManager         Dir_Tiles;
    extern AnimatorsArray   Animator_Tiles;
    /*****World Tiles************/

    /*****World Scenery************/
    bool loadWorldScenery();
    bool loadWorldScenery(obj_w_scenery &scene, std::string section, obj_w_scenery *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getSceneryTexture(unsigned long sceneryID);
    /*****************************/
    extern PGE_DataArray<obj_w_scenery>     wld_scenery;
    extern CustomDirManager         Dir_Scenery;
    extern AnimatorsArray   Animator_Scenery;
    /*****World Scenery************/

    /*****World Paths************/
    bool loadWorldPaths();
    bool loadWorldPath(obj_w_path &path, std::string section, obj_w_path *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getWldPathTexture(unsigned long pathID);
    /*****************************/
    extern PGE_DataArray<obj_w_path>   wld_paths;
    extern CustomDirManager         Dir_WldPaths;
    extern AnimatorsArray   Animator_WldPaths;
    /*****World Paths************/

    /*****World Levels************/
    bool loadWorldLevels();
    bool loadWorldLevel(obj_w_level &level, std::string section, obj_w_level *merge_with = 0, std::string iniFile = "", IniProcessing *setup = nullptr);
    int  getWldLevelTexture(unsigned long levelID);
    /*****************************/
    extern PGE_DataArray<obj_w_level>   wld_levels;
    extern CustomDirManager         Dir_WldLevel;
    extern AnimatorsArray   Animator_WldLevel;
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
    extern TexturesBank level_textures;
    extern TexturesBank world_textures;
    /***********Texture banks*************/

    void addError(std::string bug);

    extern Strings::List errorsList;

    std::string PathLevelBGO();
    std::string PathLevelBG();
    std::string PathLevelBlock();
    std::string PathLevelNPC();
    std::string PathLevelNPCScript();
    std::string PathLevelEffect();
    std::string PathLevelPlayable();
    std::string PathLevelPlayerScript();

    std::string PathCommonGFX();

    std::string PathScript();

    std::string PathWorldTiles();
    std::string PathWorldScenery();
    std::string PathWorldPaths();
    std::string PathWorldLevels();
    std::string PathWorldPlayable();

    std::string PathMusic();
    std::string PathSound();

    std::string clearMusTrack(std::string path);
};


#endif // CONFIG_MANAGER_H
