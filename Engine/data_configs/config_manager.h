#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include "../common_features/app_path.h"
#include "../common_features/pge_texture.h"
#include "obj_block.h"
#include <QMap>
#include <QSettings>
#include <QFile>



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

    static unsigned long music_custom_id;
    static unsigned long music_w_custom_id;
    static QVector<obj_music > main_music_lvl;
    static QVector<obj_music > main_music_wld;
    static QVector<obj_music > main_music_spc;

    static QVector<obj_sound > main_sound;


    //Level config Data
    static QVector<obj_block >     lvl_blocks;
    static QMap<long, obj_block>   lvl_block_indexes;
    static QVector<PGE_Texture >   level_textures; //Texture bank
    static PGE_Texture* getBlockTexture(long blockID);


    static QVector<PGE_Texture > world_textures;


    static void setConfigPath(QString p);
    static bool loadBasics();

    static bool loadLevelBlocks();

    static void addError(QString bug, QtMsgType level=QtWarningMsg);

    static QStringList errorsList;


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

    static QString tilePath;
    static QString scenePath;
    static QString pathPath;
    static QString wlvlPath;

    static QString commonGPath;


};


#endif // CONFIG_MANAGER_H
