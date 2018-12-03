/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include <gui/pge_msgbox.h>

#include "obj_music.h"
#include "config_manager.h"
#include "config_manager_private.h"

#include <Utils/files.h>
#include <IniProcessor/ini_processing.h>
#include <common_features/fmt_format_ne.h>

uint64_t        ConfigManager::music_custom_id;
uint64_t        ConfigManager::music_w_custom_id;
PGE_DataArray<obj_music> ConfigManager::main_music_lvl;
PGE_DataArray<obj_music> ConfigManager::main_music_wld;
PGE_DataArray<obj_music> ConfigManager::main_music_spc;

bool ConfigManager::musicIniChanged()
{
    bool s = music_lastIniFile_changed;
    music_lastIniFile_changed = false;
    return s;
}

std::string ConfigManager::music_lastIniFile;
bool ConfigManager::music_lastIniFile_changed = false;

obj_music::obj_music()
{
    id = 0;
}

bool ConfigManager::loadDefaultMusics()
{
    return loadMusic(dirs.music, config_dirSTD + "music.ini", false);
}

std::string ConfigManager::clearMusTrack(std::string path)
{
    std::string::size_type argDim = path.rfind('|');
    if(argDim != std::string::npos)
        return path.substr(0, argDim);
    else
        return path;
}

bool ConfigManager::loadMusic(std::string rootPath, std::string iniFile, bool isCustom)
{
    unsigned int i;
    obj_music smusic_lvl;
    obj_music smusic_wld;
    obj_music smusic_spc;
    unsigned long music_lvl_total = 0;
    unsigned long music_wld_total = 0;
    unsigned long music_spc_total = 0;
    std::string music_ini = iniFile;

    if(!Files::fileExists(music_ini))
    {
        if(isCustom)
            return false;

        addError("ERROR LOADING music.ini: file does not exist");
        return false;
    }

    if(isCustom)
    {
        if(music_lastIniFile == iniFile) return false;

        music_lastIniFile = iniFile;
        music_lastIniFile_changed = true;
    }

    IniProcessing musicset(music_ini);

    if(!isCustom)
    {
        main_music_lvl.clear();   //Clear old
        main_music_wld.clear();   //Clear old
        main_music_spc.clear();   //Clear old
        musicset.beginGroup("music-main");
        music_lvl_total = musicset.value("total-level", 0).toULongLong();
        music_wld_total = musicset.value("total-world", 0).toULongLong();
        music_spc_total = musicset.value("total-special", 0).toULongLong();
        music_custom_id = musicset.value("level-custom-music-id", 24).toULongLong();
        music_w_custom_id = musicset.value("world-custom-music-id", 17).toULongLong();
        musicset.endGroup();
    }
    else
    {
        music_lvl_total = main_music_lvl.total();
        music_wld_total = main_music_wld.total();
        music_spc_total = main_music_spc.total();
    }

    //////////////////////////////

    if(!isCustom && music_lvl_total == 0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of Level Music items not define, or empty config");
        return false;
    }

    if(!isCustom && music_wld_total == 0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of World Music items not define, or empty config");
        return false;
    }

    if(!isCustom && music_spc_total == 0)
    {
        PGE_MsgBox::fatal("ERROR LOADING music.ini: number of Special Music items not define, or empty config");
        return false;
    }

    if(!isCustom)
    {
        main_music_lvl.allocateSlots(music_lvl_total);
        main_music_wld.allocateSlots(music_wld_total);
        main_music_spc.allocateSlots(music_spc_total);
    }

    //World music
    for(i = 1; i <= music_wld_total; i++)
    {
        musicset.beginGroup(fmt::format_ne("world-music-{0}", i));
        smusic_wld.id = i;
        smusic_wld.name = musicset.value("name", "").toString();

        if(smusic_wld.name.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                addError(fmt::format_ne("WLD-Music-{0} Item name isn't defined", i));
                goto skipWldMusic;
            }
            else
                smusic_wld.name = fmt::format_ne("world-music-{0}", i);
        }

        smusic_wld.file = musicset.value("file", "").toString();

        if(smusic_wld.file.empty() && (i != music_w_custom_id))
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("WLD-Music-{0} Item file isn't defined", i));

            goto skipWldMusic;
        }

        smusic_wld.absPath = rootPath + smusic_wld.file;

        if(
            (smusic_wld.id != music_w_custom_id) &&
            (!Files::fileExists(clearMusTrack(smusic_wld.absPath)))
        )
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("WLD-Music-{0}: file {1} not exist", i, smusic_wld.absPath));

            goto skipWldMusic;
        }

        main_music_wld[i] = smusic_wld;
skipWldMusic:
        musicset.endGroup();

        if(musicset.lastError() != IniProcessing::ERR_OK)
        {
            if(!isCustom)
                addError(fmt::format_ne("ERROR LOADING music.ini N:{0} (world music {1})", musicset.lineWithError(), i));
            break;
        }
    }

    //Special music
    for(i = 1; i <= music_spc_total; i++)
    {
        musicset.beginGroup(fmt::format_ne("special-music-{0}", i));
        smusic_spc.id = i;
        smusic_spc.name = musicset.value("name", "").toString();

        if(smusic_spc.name.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                addError(fmt::format_ne("SPC-Music-{0} Item name isn't defined", i));
                goto skipSpcMusic;
            }
            else
                smusic_spc.name = fmt::format_ne("special-music-{0}", i);
        }

        smusic_spc.file = musicset.value("file", "").toString();

        if(smusic_spc.file.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("SPC-Music-{0} Item file isn't defined", i));

            goto skipSpcMusic;
        }

        smusic_spc.absPath = rootPath + smusic_spc.file;

        if(!Files::fileExists(clearMusTrack(smusic_spc.absPath)))
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("Special-Music-{0}: file {1} not exist", i, smusic_spc.absPath));

            goto skipSpcMusic;
        }

        main_music_spc[i] = smusic_spc;
skipSpcMusic:
        musicset.endGroup();

        if(musicset.lastError() != IniProcessing::ERR_OK)
        {
            std::string msg = fmt::format_ne("ERROR LOADING music.ini N:{0} (special music {1})", musicset.lineWithError(), i);
            addError(msg);
            PGE_MsgBox::error(msg);
            break;
        }
    }

    //Level music
    for(i = 1; i <= music_lvl_total; i++)
    {
        musicset.beginGroup(fmt::format_ne("level-music-{0}", i));
        smusic_lvl.id = i;
        smusic_lvl.name = musicset.value("name", "").toString();

        if(smusic_lvl.name.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                addError(fmt::format_ne("LVL-Music-{0} Item name isn't defined", i));
                goto skipLvlMusic;
            }
            else
                smusic_lvl.name = fmt::format_ne("level-music-{0}", i);
        }

        smusic_lvl.file = musicset.value("file", "").toString();

        if(smusic_lvl.file.empty() && (i != music_custom_id))
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("LVL-Music-{0} Item file isn't defined", i));

            goto skipLvlMusic;
        }

        smusic_lvl.absPath = rootPath + smusic_lvl.file;

        if(
            (smusic_lvl.id != music_custom_id) &&
            (!Files::fileExists(clearMusTrack(smusic_lvl.absPath)))
        )
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("LVL-Music-{0}: file {1} not exist", i, smusic_lvl.absPath));

            goto skipLvlMusic;
        }

        main_music_lvl[i] = smusic_lvl;
skipLvlMusic:
        musicset.endGroup();

        if(musicset.lastError() != IniProcessing::ERR_OK)
        {
            if(!isCustom)
            {
                addError(fmt::format_ne("ERROR LOADING music.ini N:{0} (level-music {1})", musicset.lineWithError(), i));
                PGE_MsgBox::error(fmt::format_ne("ERROR LOADING music.ini N:{0} (level-music {1})", musicset.lineWithError(), i));
            }
            break;
        }
    }

    return true;
}

std::string ConfigManager::getWldMusic(uint64_t musicID, std::string customMusic)
{
    if(musicID == 0)
        return "";
    else if(musicID == music_w_custom_id)
        return customMusic;
    else if(main_music_wld.contains(musicID))
        return main_music_wld[musicID].absPath;
    else
        return "";
}

std::string ConfigManager::getLvlMusic(uint64_t musicID, std::string customMusic)
{
    if(musicID == 0)
        return "";
    else if(musicID == music_custom_id)
        return customMusic;
    else if(main_music_lvl.contains(musicID))
        return main_music_lvl[musicID].absPath;
    else
        return "";
}

std::string ConfigManager::getSpecialMusic(uint64_t musicID)
{
    if(musicID == 0)
        return "";
    else if(main_music_spc.contains(musicID))
        return main_music_spc[musicID].absPath;
    else
        return "";
}

