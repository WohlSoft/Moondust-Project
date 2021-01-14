/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "obj_sound.h"
#include "config_manager.h"
#include "config_manager_private.h"

#include <gui/pge_msgbox.h>
#include <SDL2/SDL_mixer_ext.h>
#include <vector>
#include <common_features/logger.h>
#include <FileMapper/file_mapper.h>

#include <common_features/fmt_format_ne.h>
#include <IniProcessor/ini_processing.h>
#include <Utils/files.h>

#ifdef DEBUG_BUILD
#include <Utils/elapsed_timer.h>
#endif

PGE_DataArray<obj_sound > ConfigManager::main_sound;
PGE_DataArray<long > ConfigManager::main_sound_table;

std::vector<obj_sound_index > ConfigManager::main_sfx_index;


bool ConfigManager::soundIniChanged()
{
    bool s = sound_lastIniFile_changed;
    sound_lastIniFile_changed = false;
#ifdef DEBUG_BUILD
    pLogDebug("Last Sounds.INI was changed: %d", s);
#endif
    return s;
}

std::string ConfigManager::sound_lastIniFile;
bool ConfigManager::sound_lastIniFile_changed = false;


bool ConfigManager::loadDefaultSounds()
{
    return loadSound(dirs.sounds, config_dirSTD + "sounds.ini", false);
}

void obj_sound_index::setPath(std::string _path)
{
    need_reload = (path != _path);
    path = _path;
}


void obj_sound_index::play()
{
    if(chunk)
        Mix_PlayChannel(channel, chunk, 0);
    else
        pLogDebug("obj_sound_index::play() Null chunk!, file path: %s", path.c_str());
}

void ConfigManager::buildSoundIndex()
{
    int need_to_reserve = 0;
    int total_channels = 32;
    bool newBuild = main_sfx_index.empty();
#ifdef DEBUG_BUILD
    ElapsedTimer loadingTime;
    loadingTime.start();
#endif

    if(newBuild)
    {
        //build array table
        main_sfx_index.resize(static_cast<size_t>(main_sound.size()) - 1);

        for(unsigned long i = 1; i < main_sound.size(); i++)
        {
            obj_sound_index sound;

            if(main_sound.contains(i))
            {
                obj_sound &snd = main_sound[i];
#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32) || defined(__HAIKU__)
                FileMapper fileMap;

                if(fileMap.open_file(snd.absPath.c_str()))
                {
                    sound.chunk = Mix_LoadWAV_RW(SDL_RWFromMem(fileMap.data(),
                                                 static_cast<int>(fileMap.size())), 1);
                    fileMap.close_file();
                }

#else
                sound.chunk = Mix_LoadWAV(snd.absPath.c_str());
#endif
                sound.path = snd.absPath;

                if(!sound.chunk)
                    pLogWarning("Fail to load sound-%d: %s", i, Mix_GetError());
                else
                    need_to_reserve += (snd.channel >= 0 ? 1 : 0);

                sound.channel = snd.channel;
            }

            main_sfx_index[static_cast<size_t>(i) - 1] = sound;
        }
    }
    else
    {
        for(unsigned long i = 1; (i < main_sound.size()) && (i <= static_cast<unsigned long>(main_sfx_index.size())); i++)
        {
            if(main_sound.contains(i))
            {
                obj_sound_index &sound = main_sfx_index[static_cast<size_t>(i) - 1];
                obj_sound &snd = main_sound[i];
                sound.setPath(snd.absPath);

                if(sound.need_reload)
                {
#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32) || defined(__HAIKU__)
                    FileMapper fileMap;

                    if(fileMap.open_file(snd.absPath.c_str()))
                    {
                        sound.chunk = Mix_LoadWAV_RW(SDL_RWFromMem(fileMap.data(),
                                                     static_cast<int>(fileMap.size())),
                                                     static_cast<int>(fileMap.size()));
                        fileMap.close_file();
                    }

#else
                    sound.chunk = Mix_LoadWAV(snd.absPath.c_str());
#endif
                }

                if(!sound.chunk)
                    pLogWarning("Fail to load sound-%d: %s", i, Mix_GetError());
                else
                    need_to_reserve += (snd.channel >= 0 ? 1 : 0);

                sound.channel = snd.channel;
            }
        }
    }

    if(need_to_reserve > 0)
    {
        total_channels = (total_channels + need_to_reserve + 32);
        total_channels = Mix_AllocateChannels(total_channels);
    }

    //Final channel definition (use reserved channels at end of channels set)
    //int set_channel = (total_channels-1);
    //for(int i=0; i < main_sfx_index.size(); i++)
    //{
    //    obj_sound_index &sound = main_sfx_index[i];
    //    if(sound.channel>=0)
    //    {
    //        sound.channel = set_channel--;
    //    }
    //}

    if(need_to_reserve == total_channels)
        need_to_reserve = 0;

    //else
    //    need_to_reserve=set_channel;
#ifndef DEBUG_BUILD
    Mix_ReserveChannels(need_to_reserve)
#endif
#define RESERVE_CHANS_COMMAND Mix_ReserveChannels(need_to_reserve)
    D_pLogDebug("Loading of sounds passed in %d milliseconds", static_cast<int>(loadingTime.elapsed()));
    D_pLogDebug("Reserved audio channels: %d", RESERVE_CHANS_COMMAND);
    D_pLogDebug("SFX Index entries: %d", main_sfx_index.size());
#undef RESERVE_CHANS_COMMAND
    SDL_ClearError();
}

void ConfigManager::clearSoundIndex()
{
    Mix_ReserveChannels(0);

    if(main_sfx_index.empty())
        return;

    size_t size = main_sfx_index.size();
    obj_sound_index *d = main_sfx_index.data();

    for(size_t i = 0; i < size; i++)
    {
        if(d[i].chunk)
            Mix_FreeChunk(d[i].chunk);
    }

    main_sfx_index.clear();
}


bool ConfigManager::loadSound(std::string rootPath, std::string iniFile, bool isCustom)
{
    unsigned int i;
    obj_sound sound;
    unsigned long sound_total = 0;
    int cur_channel = 0;
    bool reserveChannel = false;
    std::string sound_ini = iniFile;

    if(!Files::fileExists(sound_ini))
    {
        if(isCustom) return false;

        addError("ERROR LOADING sounds.ini: file does not exist");
        PGE_MsgBox::error("ERROR LOADING sounds.ini: file does not exist");
        return false;
    }

    if(isCustom)
    {
        if(sound_lastIniFile == iniFile)
            return false;

        sound_lastIniFile = iniFile;
        sound_lastIniFile_changed = true;
    }

    IniProcessing soundset(sound_ini);

    if(!isCustom) //Show errors if error caused with the internal stuff folder
    {
        main_sound.clear();   //Clear old
    }

    if(!isCustom)
    {
        soundset.beginGroup("sound-main");
        sound_total = soundset.value("total", 0).toULongLong();
        soundset.endGroup();
    }
    else
        sound_total = main_sound.total();

    if(sound_total == 0)
    {
        if(isCustom) return false;

        addError("ERROR LOADING sounds.ini: number of items not define, or empty config");
        PGE_MsgBox::error("ERROR LOADING sounds.ini: number of items not define, or empty config");
        return false;
    }

    if(!isCustom)
        main_sound.allocateSlots(sound_total);

    //////////////////////////////

    //Sound
    for(i = 1; i <= sound_total; i++)
    {
        soundset.beginGroup(fmt::format_ne("sound-{0}", i));
        sound.name = soundset.value("name", "").toString();

        if(sound.name.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                addError(fmt::format_ne("Sound-{0} Item name isn't defined", i));
                goto skipSoundFile;
            }
            else
                sound.name = fmt::format_ne("sound-{0}", i);
        }

        sound.file = soundset.value("file", "").toString();

        if(sound.file.empty())
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("Sound-{0} Item file isn't defined", i));

            goto skipSoundFile;
        }

        sound.hidden = soundset.value("hidden", "0").toBool();
        sound.absPath = rootPath + sound.file;

        if(!Files::fileExists(sound.absPath))
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
                addError(fmt::format_ne("Sound-{0}: file {1} not exist", i, sound.absPath));

            goto skipSoundFile;
        }

        reserveChannel = soundset.value("single-channel", false).toBool();

        if(reserveChannel)
            sound.channel = cur_channel++;
        else if(isCustom && main_sound.contains(i))
            sound.channel = main_sound[i].channel;
        else
            sound.channel = -1;

        sound.id = i;
        main_sound.storeElement(i, sound);
skipSoundFile:
        soundset.endGroup();

        if(soundset.lastError() != IniProcessing::ERR_OK)
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                std::string msg = fmt::format_ne("ERROR LOADING sounds.ini N:{0} (sound {1})", soundset.lastError(),i);
                addError(msg);
                PGE_MsgBox::error(msg);
                return false;
            }

            break;
        }
    }

    return true;
}

bool ConfigManager::loadSoundRolesTable()
{
    std::string sound_ini = config_dirSTD + "sound_roles.ini";

    if(!Files::fileExists(sound_ini))
    {
        const char* msg = "ERROR LOADING sound_roles.ini: file does not exist";
        addError(msg);
        PGE_MsgBox::error(msg);
        return false;
    }

    main_sound_table.clear();
    main_sound_table.allocateSlots(57);
    IniProcessing soundset(sound_ini);
    soundset.beginGroup("sound-roles");
    {
        //main_sound_table[obj_sound_role::]
        main_sound_table[obj_sound_role::Greeting]          = soundset.value("greeting",        0).toInt();
        main_sound_table[obj_sound_role::MenuDo]            = soundset.value("menudo",          0).toInt();
        main_sound_table[obj_sound_role::MenuScroll]        = soundset.value("menuscroll",      0).toInt();
        main_sound_table[obj_sound_role::MenuMessageBox]    = soundset.value("menumessagebox",  0).toInt();
        main_sound_table[obj_sound_role::MenuPause]         = soundset.value("menupause",       0).toInt();
        main_sound_table[obj_sound_role::CameraSwitch]      = soundset.value("cameraswitch",    0).toInt();
        main_sound_table[obj_sound_role::PlayerJump]        = soundset.value("playerjump",      0).toInt();
        main_sound_table[obj_sound_role::PlayerStomp]       = soundset.value("playerstomp",     0).toInt();
        main_sound_table[obj_sound_role::PlayerKick]        = soundset.value("playerkick",      0).toInt();
        main_sound_table[obj_sound_role::PlayerShrink]      = soundset.value("playershrink",    0).toInt();
        main_sound_table[obj_sound_role::PlayerHarm]        = soundset.value("playerharm",      0).toInt();
        main_sound_table[obj_sound_role::PlayerGrow]        = soundset.value("playergrow",      0).toInt();
        main_sound_table[obj_sound_role::PlayerDied]        = soundset.value("playerdied",      0).toInt();
        main_sound_table[obj_sound_role::PlayerDropItem]    = soundset.value("playerdropitem",  0).toInt();
        main_sound_table[obj_sound_role::PlayerTakeItem]    = soundset.value("playertakeitem",  0).toInt();
        main_sound_table[obj_sound_role::PlayerSlide]       = soundset.value("playerslide",     0).toInt();
        main_sound_table[obj_sound_role::PlayerGrab1]       = soundset.value("playergrab1",     0).toInt();
        main_sound_table[obj_sound_role::PlayerGrab2]       = soundset.value("playergrab2",     0).toInt();
        main_sound_table[obj_sound_role::PlayerSpring]      = soundset.value("playerspring",    0).toInt();
        main_sound_table[obj_sound_role::PlayerClimb]       = soundset.value("playerclimb",     0).toInt();
        main_sound_table[obj_sound_role::PlayerTail]        = soundset.value("playertail",      0).toInt();
        main_sound_table[obj_sound_role::PlayerMagic]       = soundset.value("playermagic",     0).toInt();
        main_sound_table[obj_sound_role::PlayerWaterSwim]   = soundset.value("playerwaterswim", 0).toInt();
        main_sound_table[obj_sound_role::BonusCoin]         = soundset.value("bonuscoin",       0).toInt();
        main_sound_table[obj_sound_role::Bonus1up]          = soundset.value("bonus1up",        0).toInt();
        main_sound_table[obj_sound_role::WeaponHammer]      = soundset.value("weaponhammer",    0).toInt();
        main_sound_table[obj_sound_role::WeaponFire]        = soundset.value("weaponfire",      0).toInt();
        main_sound_table[obj_sound_role::WeaponCannon]      = soundset.value("weaponcannon",    0).toInt();
        main_sound_table[obj_sound_role::WeaponExplosion]   = soundset.value("weaponexplosion", 0).toInt();
        main_sound_table[obj_sound_role::WeaponBigFire]     = soundset.value("weaponbigfire",   0).toInt();
        main_sound_table[obj_sound_role::NpcLavaBurn]       = soundset.value("npclavaburn",     0).toInt();
        main_sound_table[obj_sound_role::NpcStoneFall]      = soundset.value("npcstonefall",    0).toInt();
        main_sound_table[obj_sound_role::NpcHit]            = soundset.value("npchit",          0).toInt();
        main_sound_table[obj_sound_role::NpcDeath]          = soundset.value("npcdeath",        0).toInt();
        main_sound_table[obj_sound_role::WarpPipe]          = soundset.value("warppipe",        0).toInt();
        main_sound_table[obj_sound_role::WarpDoor]          = soundset.value("warpdoor",        0).toInt();
        main_sound_table[obj_sound_role::WarpTeleport]      = soundset.value("warpteleport",    0).toInt();
        main_sound_table[obj_sound_role::LevelFailed]       = soundset.value("levelfailed",     0).toInt();
        main_sound_table[obj_sound_role::LevelCheckPoint]   = soundset.value("levelcheckpoint", 0).toInt();
        main_sound_table[obj_sound_role::WorldMove]         = soundset.value("worldmove",       0).toInt();
        main_sound_table[obj_sound_role::WorldDeny]         = soundset.value("worlddeny",       0).toInt();
        main_sound_table[obj_sound_role::WorldOpenPath]     = soundset.value("worldopenpath",   0).toInt();
        main_sound_table[obj_sound_role::WorldEnterLevel]   = soundset.value("worldenterlevel", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit01]       = soundset.value("levelexit01",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit02]       = soundset.value("levelexit02",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit03]       = soundset.value("levelexit03",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit04]       = soundset.value("levelexit04",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit05]       = soundset.value("levelexit05",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit06]       = soundset.value("levelexit06",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit07]       = soundset.value("levelexit07",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit08]       = soundset.value("levelexit08",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit09]       = soundset.value("levelexit09",     0).toInt();
        main_sound_table[obj_sound_role::LevelExit10]       = soundset.value("levelexit10",     0).toInt();
        main_sound_table[obj_sound_role::GameCompleted]     = soundset.value("gamecompleted",   0).toInt();
        main_sound_table[obj_sound_role::BlockHit]          = soundset.value("blockhit",        0).toInt();
        main_sound_table[obj_sound_role::BlockOpen]         = soundset.value("blockopen",       0).toInt();
        main_sound_table[obj_sound_role::BlockSmashed]      = soundset.value("blocksmashed",    0).toInt();
        main_sound_table[obj_sound_role::BlockSwitch]       = soundset.value("blockswitch",     0).toInt();
    }
    soundset.endGroup();

    if(soundset.lastError() != IniProcessing::ERR_OK)
    {
        addError(fmt::format_ne("ERROR LOADING sound_roles.ini N:{0}", soundset.lineWithError()));
        PGE_MsgBox::error(fmt::format_ne("ERROR LOADING sound_roles.ini N:{0}", soundset.lineWithError()));
        return false;
    }

    return true;
}

std::string ConfigManager::getSound(size_t sndID)
{
    if(main_sound.contains(sndID))
        return main_sound[sndID].absPath;
    else
        return "";
}

size_t ConfigManager::getSoundByRole(obj_sound_role::roles role)
{
    if(main_sound_table.contains(role))
        return main_sound_table[role];
    else
        return 0;
}
