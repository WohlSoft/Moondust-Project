#include "obj_sound.h"
#include "config_manager.h"

#include <QFileInfo>
#include <QSettings>
#include <gui/pge_msgbox.h>
#include <SDL2/SDL_mixer_ext.h>
#include <QVector>
#include <common_features/logger.h>
#include <common_features/file_mapper.h>
#include <QtDebug>

#ifdef DEBUG_BUILD
#include <QElapsedTimer>
#endif

PGE_DataArray<obj_sound > ConfigManager::main_sound;
PGE_DataArray<long > ConfigManager::main_sound_table;

QVector<obj_sound_index > ConfigManager::main_sfx_index;


bool ConfigManager::soundIniChanged()
{
    bool s=sound_lastIniFile_changed;
    sound_lastIniFile_changed=false;
    #ifdef DEBUG_BUILD
    WriteToLog(QtDebugMsg, QString("Last Sounds.INI was changed: %1").arg(s));
    #endif
    return s;
}

QString ConfigManager::sound_lastIniFile;
bool ConfigManager::sound_lastIniFile_changed=false;


bool ConfigManager::loadDefaultSounds()
{
    return loadSound(dirs.sounds, config_dir+"sounds.ini", false);
}

obj_sound::obj_sound()
{
    id=0;
    hidden=false;
    channel=-1;
}

obj_sound_index::obj_sound_index()
{
    need_reload=false;
    chunk=NULL;
    channel=-1;
}

void obj_sound_index::setPath(QString _path)
{
    need_reload=(path!=_path);
    path=_path;
}


void obj_sound_index::play()
{
    if(chunk)
        Mix_PlayChannel(channel, chunk, 0);
    else
        qDebug() << "obj_sound_index::play() Null chunk!, file path:" << path;
}

void ConfigManager::buildSoundIndex()
{
    int need_to_reserve=0;
    int total_channels=32;

    bool newBuild=main_sfx_index.isEmpty();

    #ifdef DEBUG_BUILD
    QElapsedTimer loadingTime;
    loadingTime.start();
    #endif

    if(newBuild)
    {
        //build array table
        main_sfx_index.resize(main_sound.size()-1);
        for(int i=1; i<main_sound.size();i++)
        {
            obj_sound_index sound;
            if(main_sound.contains(i))
            {
                obj_sound &snd = main_sound[i];
                #if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
                PGE_FileMapper fileMap;
                if( fileMap.open_file(snd.absPath.toUtf8().data()) )
                {
                    sound.chunk = Mix_LoadWAV_RW(SDL_RWFromMem(fileMap.data, fileMap.size), fileMap.size);
                    fileMap.close_file();
                }
                #else
                sound.chunk = Mix_LoadWAV(snd.absPath.toUtf8().data());
                #endif
                sound.path = snd.absPath;
                if(!sound.chunk)
                    qDebug() <<"Fail to load sound-"<<i<<":"<<Mix_GetError();
                else
                    need_to_reserve += (snd.channel>=0?1:0);
                sound.channel = snd.channel;
            }
            main_sfx_index[i-1]=sound;
        }
    } else {
        for(int i=1; (i<main_sound.size())&&(i<=main_sfx_index.size()); i++)
        {
            if(main_sound.contains(i))
            {
                obj_sound_index &sound = main_sfx_index[i-1];
                obj_sound &snd = main_sound[i];
                sound.setPath(snd.absPath);
                if(sound.need_reload)
                {
                    #if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
                    PGE_FileMapper fileMap;
                    if( fileMap.open_file(snd.absPath.toUtf8().data()) )
                    {
                        sound.chunk = Mix_LoadWAV_RW(SDL_RWFromMem(fileMap.data, fileMap.size), fileMap.size);
                        fileMap.close_file();
                    }
                    #else
                    sound.chunk = Mix_LoadWAV(snd.absPath.toUtf8().data());
                    #endif
                }
                if(!sound.chunk)
                    qDebug() <<"Fail to load sound-"<<i<<":"<<Mix_GetError();
                else
                    need_to_reserve += (snd.channel>=0?1:0);
                sound.channel = snd.channel;
            }
        }
    }

    if(need_to_reserve>0)
    {
        total_channels=(total_channels+need_to_reserve+32);
        Mix_AllocateChannels(total_channels);
    }

    //Final channel definition (use reserved channels at end of channels set)
    int set_channel = (total_channels-1);
    for(int i=0; i<main_sfx_index.size();i++)
    {
        obj_sound_index &sound = main_sfx_index[i];
        if(sound.channel>=0)
        {
            sound.channel = set_channel--;
        }
    }

    if(need_to_reserve==total_channels)
        need_to_reserve=0;
    else
        need_to_reserve=(total_channels-need_to_reserve);

    #define RESERVE_CHANS_COMMAND Mix_ReserveChannels(need_to_reserve)

    #ifdef DEBUG_BUILD
    WriteToLog(QtDebugMsg, QString("Loading of sounds passed in %1 milliseconds").arg(loadingTime.elapsed()));
    qDebug() << "Reserved audio channels: "<< RESERVE_CHANS_COMMAND;
    qDebug() << "SFX Index entries: " << main_sfx_index.size();
    #else
    RESERVE_CHANS_COMMAND;
    #endif
    SDL_ClearError();
}

void ConfigManager::clearSoundIndex()
{
    Mix_ReserveChannels(0);

    if(main_sfx_index.isEmpty()) return;

    int size = main_sfx_index.size();
    obj_sound_index * d = main_sfx_index.data();
    for(int i=0; i<size; i++)
    {
        if(d[i].chunk)
            Mix_FreeChunk(d[i].chunk);
    }
    main_sfx_index.clear();
}


bool ConfigManager::loadSound(QString rootPath, QString iniFile, bool isCustom)
{
    unsigned int i;

    obj_sound sound;

    unsigned long sound_total=0;
    int cur_channel=0;
    bool reserveChannel=false;

    QString sound_ini = iniFile;
    if(!QFile::exists(sound_ini))
    {
        if(isCustom) return false;
        addError(QString("ERROR LOADING sounds.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::error(QString("ERROR LOADING sounds.ini: file does not exist"));
        return false;
    }

    if(isCustom)
    {
        if(sound_lastIniFile==iniFile) return false;
        sound_lastIniFile=iniFile;
        sound_lastIniFile_changed=true;
    }

    QSettings soundset(sound_ini, QSettings::IniFormat);
    soundset.setIniCodec("UTF-8");


    if(!isCustom) //Show errors if error caused with the internal stuff folder
    {
        main_sound.clear();   //Clear old
    }

    if(!isCustom)
    {
        soundset.beginGroup("sound-main");
            sound_total = soundset.value("total", "0").toInt();
        soundset.endGroup();
    } else {
        sound_total = main_sound.total();
    }

    if(sound_total==0)
    {
        if(isCustom) return false;
        addError(QString("ERROR LOADING sounds.ini: number of items not define, or empty config"), QtCriticalMsg);
        PGE_MsgBox::error(QString("ERROR LOADING sounds.ini: number of items not define, or empty config"));
        return false;
    }

    if(!isCustom)
    {
        main_sound.allocateSlots(sound_total);
    }
    //////////////////////////////

    //Sound
    for(i=1; i<=sound_total; i++)
    {
        soundset.beginGroup( QString("sound-"+QString::number(i)) );
            sound.name = soundset.value("name", "").toString();
            if(sound.name.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("Sound-%1 Item name isn't defined").arg(i));
                    goto skipSoundFile;
                } else {
                    sound.name = "sound-"+QString::number(i);
                }
            }
            sound.file = soundset.value("file", "").toString();
            if(sound.file.isEmpty())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("Sound-%1 Item file isn't defined").arg(i));
                }
                goto skipSoundFile;
            }
            sound.hidden = soundset.value("hidden", "0").toBool();

            sound.absPath = rootPath + sound.file;
            if(!QFileInfo(sound.absPath).exists())
            {
                if(!isCustom) //Show errors if error caused with the internal stuff folder
                {
                    addError(QString("Sound-%1: file %2 not exist").arg(i).arg(sound.absPath));
                }
                goto skipSoundFile;
            }
            reserveChannel = soundset.value("single-channel", false).toBool();
            if(reserveChannel)
                sound.channel = cur_channel++;
            else
                if(isCustom && main_sound.contains(i))
                    sound.channel = main_sound[i].channel;
                else sound.channel=-1;

            sound.id = i;
            main_sound.storeElement(i, sound);
        skipSoundFile:

        soundset.endGroup();

        if( soundset.status() != QSettings::NoError )
        {
            if(!isCustom) //Show errors if error caused with the internal stuff folder
            {
                addError(QString("ERROR LOADING sounds.ini N:%1 (sound %2)").arg(soundset.status()).arg(i), QtCriticalMsg);
                PGE_MsgBox::error(QString("ERROR LOADING sounds.ini N:%1 (sound %2)").arg(soundset.status()).arg(i));
                return false;
            }
            break;
        }
    }
    return true;
}

bool ConfigManager::loadSoundRolesTable()
{
    QString sound_ini = config_dir+"sound_roles.ini";
    if(!QFile::exists(sound_ini))
    {
        addError(QString("ERROR LOADING sound_roles.ini: file does not exist"), QtCriticalMsg);
        PGE_MsgBox::error(QString("ERROR LOADING sound_roles.ini: file does not exist"));
        return false;
    }

    main_sound_table.clear();
    main_sound_table.allocateSlots(57);

    QSettings soundset(sound_ini, QSettings::IniFormat);
    soundset.setIniCodec("UTF-8");
    soundset.beginGroup("sound-roles");
    //main_sound_table[obj_sound_role::]
        main_sound_table[obj_sound_role::Greeting]=soundset.value("greeting", 0).toInt();
        main_sound_table[obj_sound_role::MenuDo]=soundset.value("menudo", 0).toInt();
        main_sound_table[obj_sound_role::MenuScroll]=soundset.value("menuscroll", 0).toInt();
        main_sound_table[obj_sound_role::MenuMessageBox]=soundset.value("menumessagebox", 0).toInt();
        main_sound_table[obj_sound_role::MenuPause]=soundset.value("menupause", 0).toInt();
        main_sound_table[obj_sound_role::CameraSwitch]=soundset.value("cameraswitch", 0).toInt();
        main_sound_table[obj_sound_role::PlayerJump]=soundset.value("playerjump", 0).toInt();
        main_sound_table[obj_sound_role::PlayerStomp]=soundset.value("playerstomp", 0).toInt();
        main_sound_table[obj_sound_role::PlayerKick]=soundset.value("playerkick", 0).toInt();
        main_sound_table[obj_sound_role::PlayerShrink]=soundset.value("playershrink", 0).toInt();
        main_sound_table[obj_sound_role::PlayerHarm]=soundset.value("playerharm", 0).toInt();
        main_sound_table[obj_sound_role::PlayerGrow]=soundset.value("playergrow", 0).toInt();
        main_sound_table[obj_sound_role::PlayerDied]=soundset.value("playerdied", 0).toInt();
        main_sound_table[obj_sound_role::PlayerDropItem]=soundset.value("playerdropitem", 0).toInt();
        main_sound_table[obj_sound_role::PlayerTakeItem]=soundset.value("playertakeitem", 0).toInt();
        main_sound_table[obj_sound_role::PlayerSlide]=soundset.value("playerslide", 0).toInt();
        main_sound_table[obj_sound_role::PlayerGrab1]=soundset.value("playergrab1", 0).toInt();
        main_sound_table[obj_sound_role::PlayerGrab2]=soundset.value("playergrab2", 0).toInt();
        main_sound_table[obj_sound_role::PlayerSpring]=soundset.value("playerspring", 0).toInt();
        main_sound_table[obj_sound_role::PlayerClimb]=soundset.value("playerclimb", 0).toInt();
        main_sound_table[obj_sound_role::PlayerTail]=soundset.value("playertail", 0).toInt();
        main_sound_table[obj_sound_role::PlayerMagic]=soundset.value("playermagic", 0).toInt();
        main_sound_table[obj_sound_role::PlayerWaterSwim]=soundset.value("playerwaterswim", 0).toInt();
        main_sound_table[obj_sound_role::BonusCoin]=soundset.value("bonuscoin", 0).toInt();
        main_sound_table[obj_sound_role::Bonus1up]=soundset.value("bonus1up", 0).toInt();
        main_sound_table[obj_sound_role::WeaponHammer]=soundset.value("weaponhammer", 0).toInt();
        main_sound_table[obj_sound_role::WeaponFire]=soundset.value("weaponfire", 0).toInt();
        main_sound_table[obj_sound_role::WeaponCannon]=soundset.value("weaponcannon", 0).toInt();
        main_sound_table[obj_sound_role::WeaponExplosion]=soundset.value("weaponexplosion", 0).toInt();
        main_sound_table[obj_sound_role::WeaponBigFire]=soundset.value("weaponbigfire", 0).toInt();
        main_sound_table[obj_sound_role::NpcLavaBurn]=soundset.value("npclavaburn", 0).toInt();
        main_sound_table[obj_sound_role::NpcStoneFall]=soundset.value("npcstonefall", 0).toInt();
        main_sound_table[obj_sound_role::NpcHit]=soundset.value("npchit", 0).toInt();
        main_sound_table[obj_sound_role::NpcDeath]=soundset.value("npcdeath", 0).toInt();
        main_sound_table[obj_sound_role::WarpPipe]=soundset.value("warppipe", 0).toInt();
        main_sound_table[obj_sound_role::WarpDoor]=soundset.value("warpdoor", 0).toInt();
        main_sound_table[obj_sound_role::WarpTeleport]=soundset.value("warpteleport", 0).toInt();
        main_sound_table[obj_sound_role::LevelFailed]=soundset.value("levelfailed", 0).toInt();
        main_sound_table[obj_sound_role::LevelCheckPoint]=soundset.value("levelcheckpoint", 0).toInt();
        main_sound_table[obj_sound_role::WorldMove]=soundset.value("worldmove", 0).toInt();
        main_sound_table[obj_sound_role::WorldDeny]=soundset.value("worlddeny", 0).toInt();
        main_sound_table[obj_sound_role::WorldOpenPath]=soundset.value("worldopenpath", 0).toInt();
        main_sound_table[obj_sound_role::WorldEnterLevel]=soundset.value("worldenterlevel", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit01]=soundset.value("levelexit01", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit02]=soundset.value("levelexit02", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit03]=soundset.value("levelexit03", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit04]=soundset.value("levelexit04", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit05]=soundset.value("levelexit05", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit06]=soundset.value("levelexit06", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit07]=soundset.value("levelexit07", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit08]=soundset.value("levelexit08", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit09]=soundset.value("levelexit09", 0).toInt();
        main_sound_table[obj_sound_role::LevelExit10]=soundset.value("levelexit10", 0).toInt();
        main_sound_table[obj_sound_role::GameCompleted]=soundset.value("gamecompleted", 0).toInt();
        main_sound_table[obj_sound_role::BlockHit]=soundset.value("blockhit", 0).toInt();
        main_sound_table[obj_sound_role::BlockOpen]=soundset.value("blockopen", 0).toInt();
        main_sound_table[obj_sound_role::BlockSmashed]=soundset.value("blocksmashed", 0).toInt();
        main_sound_table[obj_sound_role::BlockSwitch]=soundset.value("blockswitch", 0).toInt();
    soundset.endGroup();

    if( soundset.status() != QSettings::NoError )
    {
        addError(QString("ERROR LOADING sound_roles.ini N:%1").arg(soundset.status()), QtCriticalMsg);
        PGE_MsgBox::error(QString("ERROR LOADING sound_roles.ini N:%1").arg(soundset.status()));
        return false;
    }
    return true;
}

QString ConfigManager::getSound(unsigned long sndID)
{
    if(main_sound.contains(sndID))
        return main_sound[sndID].absPath;
    else
        return "";
}

long ConfigManager::getSoundByRole(obj_sound_role::roles role)
{
    if(main_sound_table.contains(role))
        return main_sound_table[role];
    else
        return 0;
}

