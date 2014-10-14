#include "../common_features/pge_texture.h"
#include "config_manager.h"

#include "../common_features/graphics_funcs.h"

ConfigManager::ConfigManager()
{
}

DataFolders      ConfigManager::dirs;
QString          ConfigManager::config_dir;
QString          ConfigManager::data_dir;

QStringList ConfigManager::errorsList;

//Common Data
QVector<PGE_Texture > ConfigManager::common_textures;

unsigned long ConfigManager::music_custom_id;
unsigned long ConfigManager::music_w_custom_id;
QVector<obj_music > ConfigManager::main_music_lvl;
QVector<obj_music > ConfigManager::main_music_wld;
QVector<obj_music > ConfigManager::main_music_spc;

QVector<obj_sound > ConfigManager::main_sound;


//Level config Data
QVector<obj_block >     ConfigManager::lvl_blocks;
QMap<long, obj_block>   ConfigManager::lvl_block_indexes;
QVector<PGE_Texture >   ConfigManager::level_textures; //Texture bank

QVector<PGE_Texture > ConfigManager::world_textures;


QString ConfigManager::imgFile, ConfigManager::imgFileM;
QString ConfigManager::tmpstr;
QStringList ConfigManager::tmp;

unsigned long ConfigManager::total_data;
QString ConfigManager::bgoPath;
QString ConfigManager::BGPath;
QString ConfigManager::blockPath;
QString ConfigManager::npcPath;

QString ConfigManager::tilePath;
QString ConfigManager::scenePath;
QString ConfigManager::pathPath;
QString ConfigManager::wlvlPath;




PGE_Texture*           ConfigManager::getBlockTexture(long blockID)
{
    PGE_Texture* target=NULL;
    if(!lvl_block_indexes.contains(blockID))
        return target;

    if(lvl_block_indexes[blockID].isInit)
        return lvl_block_indexes[blockID].image;
    else
    {
        PGE_Texture texture = GraphicsHelps::loadTexture(
                    blockPath + lvl_block_indexes[blockID].image_n,
                    blockPath + lvl_block_indexes[blockID].mask_n
                    );
        level_textures.push_back(texture);
        lvl_block_indexes[blockID].image = &(level_textures.last());
        return lvl_block_indexes[blockID].image;
    }

}





void ConfigManager::setConfigPath(QString p)
{
    config_dir = ApplicationPath + "/" +  "configs/" + p + "/";
}

void ConfigManager::loadBasics()
{
    QString gui_ini = config_dir + "main.ini";
    QSettings guiset(gui_ini, QSettings::IniFormat);
    guiset.setIniCodec("UTF-8");

    guiset.beginGroup("main");
        data_dir = (guiset.value("application-dir", "0").toBool() ?
                        ApplicationPath + "/" : config_dir + "data/" );
        guiset.endGroup();
}

void ConfigManager::addError(QString bug, QtMsgType level)
{
    Q_UNUSED(level);
    errorsList<<bug;
}

