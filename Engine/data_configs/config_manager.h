#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "../common_features/app_path.h"
#include "../common_features/pge_texture.h"
#include "../common_features/simple_animator.h"

#include "obj_block.h"
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

struct LoadingScreenAdditionalImage
{
    QString imgFile;
    bool animated;
    int frames;
    int x;
    int y;
};

struct LoadingScreenData
{
    int bg_color_r;
    int bg_color_g;
    int bg_color_b;
    QString backgroundImg;
    int updateDelay;
    QVector<LoadingScreenAdditionalImage > AdditionalImages;
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
    static QString config_dir;
    static QString data_dir;

    //Common Data
    static QVector<PGE_Texture > common_textures;

    static unsigned int screen_width;
    static unsigned int screen_height;

    enum screenType{
        SCR_Static=0, //Static / Scalable screen
        SCR_Dynamic   //Dynamic screen
    };
    static screenType screen_type;

    //LoadingScreen
    static LoadingScreenData LoadingScreen;

    //World map data
    static WorldMapData WorldMap;


    static unsigned long music_custom_id;
    static unsigned long music_w_custom_id;
    static QVector<obj_music > main_music_lvl;
    static QVector<obj_music > main_music_wld;
    static QVector<obj_music > main_music_spc;

    static QVector<obj_sound > main_sound;


    //Level config Data
    //Level blocks
    static QVector<obj_block >     lvl_blocks;
    static QMap<long, obj_block>   lvl_block_indexes;
    static long getBlockTexture(long blockID);
    static CustomDirManager Dir_Blocks;
    static QVector<SimpleAnimator *> Animator_Blocks;

    //Texture bank
    static QVector<PGE_Texture > level_textures; //Texture bank



    static QVector<PGE_Texture > world_textures;


    static void setConfigPath(QString p);
    //Load settings
    static bool loadBasics();

    static bool loadLevelBlocks();

    static bool unloadLevelConfigs();



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
