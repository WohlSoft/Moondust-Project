#ifndef PGE_EDITOR_DEFINES_H
#define PGE_EDITOR_DEFINES_H

#include <QString>
#include <QObject>

#ifdef Q_OS_ANDROID
    #define PGEDefaultFontSize 16
#elif __APPLE__
    #define PGEDefaultFontSize 14
#else
    #define PGEDefaultFontSize 8
#endif

class ItemTypes
{
public:
    enum itemTypes
    {
        LVL_Block       = 0,
        LVL_BGO         = 1,
        LVL_NPC         = 2,
        LVL_PhysEnv     = 8,
        LVL_Door        = 9,
        LVL_Player      = 10,
        WLD_Tile        = 3,
        WLD_Scenery     = 4,
        WLD_Path        = 5,
        WLD_Level       = 6,
        WLD_MusicBox    = 7
    };

    //Multiselectable
    enum itemTypesMultiSelectable
    {
        LVL_S_Block       = 1<<0,
        LVL_S_BGO         = 1<<1,
        LVL_S_NPC         = 1<<2,
        LVL_S_PhysEnv     = 1<<3,
        LVL_S_Door        = 1<<4,
        LVL_S_Player      = 1<<5,
        WLD_S_Tile        = 1<<6,
        WLD_S_Scenery     = 1<<7,
        WLD_S_Path        = 1<<8,
        WLD_S_Level       = 1<<9,
        WLD_S_MusicBox    = 1<<10
    };



};

class HistorySettings{
public:
    enum WorldSettingSubType{
        SETTING_HUB = 0,
        SETTING_RESTARTAFTERFAIL,
        SETTING_TOTALSTARS,
        SETTING_INTROLEVEL,
        SETTING_PATHBACKGROUND,
        SETTING_BIGPATHBACKGROUND,
        SETTING_ALWAYSVISIBLE,
        SETTING_GAMESTARTPOINT,
        SETTING_LEVELFILE,
        SETTING_LEVELTITLE,
        SETTING_DOORID,
        SETTING_PATHBYTOP,
        SETTING_PATHBYRIGHT,
        SETTING_PATHBYBOTTOM,
        SETTING_PATHBYLEFT,
        SETTING_GOTOX,
        SETTING_GOTOY,
        SETTING_CHARACTER,
        SETTING_WORLDTITLE
    };

    enum LevelSettingSubType{
        SETTING_INVISIBLE = 0,      //extraData: bool [Activated?]
        SETTING_SLIPPERY,           //extraData: bool [Activated?]
        SETTING_FRIENDLY,           //extraData: bool [Activated?]
        SETTING_BOSS,               //extraData: bool [Activated?]
        SETTING_NOMOVEABLE,         //extraData: bool [Activated?]
        SETTING_MESSAGE,            //extraData: String [New Text]
        SETTING_DIRECTION,          //extraData: int [New Dir]
        SETTING_CHANGENPC,          //extraData: int [New NPC ID]
        SETTING_WATERTYPE,          //extraData: bool [IsWater = true, IsQuicksand = false]
        SETTING_LAYER,            //extraData: String [Layer name]
        SETTING_NOYOSHI,            //extraData: bool [Activated?]
        SETTING_ALLOWNPC,           //extraData: bool [Activated?]
        SETTING_LOCKED,             //extraData: bool [Activated?]
        SETTING_WARPTYPE,           //extraData: QList<QVariant[int]> [Old warptype, New warptype]
        SETTING_NEEDASTAR,          //extraData: QList<QVariant[int]> [Old stars, New stars]
        SETTING_ENTRDIR,            //extraData: QList<QVariant[int]> [Old entrance dir, New entrance dir]
        SETTING_EXITDIR,            //extraData: QList<QVariant[int]> [Old exit dir, New exit dir]
        SETTING_LEVELEXIT,          //extraData: QList<QVariant[???]> [bool Checked, int ox, int oy]
        SETTING_LEVELENTR,          //extraData: QList<QVariant[???]> [bool Checked, int ix, int iy]
        SETTING_LEVELWARPTO,        //extraData: QList<QVariant[int]> [Old id, New id]
        SETTING_GENACTIVATE,        //extraData: bool [Activated?]
        SETTING_GENTYPE,            //extraData: int [new type]
        SETTING_GENDIR,             //extraData: int [new dir]
        SETTING_GENTIME,            //extraData: int [new time]
        SETTING_ATTACHLAYER,        //extraData: String [new layer]
        SETTING_EV_DESTROYED,       //extraData: String [new event]
        SETTING_EV_HITED,           //extraData: String [new event]
        SETTING_EV_LAYER_EMP,       //extraData: String [new event]
        SETTING_EV_ACTIVATE,        //extraData: String [new event]
        SETTING_EV_DEATH,           //extraData: String [new event]
        SETTING_EV_TALK,            //extraData: String [new event]
        SETTING_SPECIAL_DATA,       //extraData: int [new type]
        SETTING_EV_AUTOSTART,       //extraData: bool [Activated?]
        SETTING_EV_SMOKE,           //extraData: bool [Activated?]
        SETTING_EV_LHIDEADD,        //extraData: String [new Layer]
        SETTING_EV_LSHOWADD,        //extraData: String [new Layer]
        SETTING_EV_LTOGADD,         //extraData: String [new Layer]
        SETTING_EV_LHIDEDEL,        //extraData: String [old Layer]
        SETTING_EV_LSHOWDEL,        //extraData: String [old Layer]
        SETTING_EV_LTOGDEL,         //extraData: String [old Layer]
        SETTING_EV_MOVELAYER,       //extraData: QList<QVariant[String]> [Old layer, New layer]
        SETTING_EV_SPEEDLAYERX,     //extraData: QList<QVariant[double]> [Old x, New x]
        SETTING_EV_SPEEDLAYERY,     //extraData: QList<QVariant[double]> [Old y, New y]
        SETTING_EV_AUTOSCRSEC,      //extraData: QList<QVariant[String]> [Old section, New section]
        SETTING_EV_AUTOSCRX,        //extraData: QList<QVariant[double]> [Old y, New y]
        SETTING_EV_AUTOSCRY,        //extraData: QList<QVariant[double]> [Old y, New y]
        SETTING_EV_SECSIZE,         //extraData: QList<QVariant[long]> [Section, old top, old right, old bottom, old left, top, right, bottom, left]
        SETTING_EV_SECMUS,          //extraData: QList<QVariant[long]> [Section, old music id, new music id]
        SETTING_EV_SECBG,           //extraData: QList<QVariant[long]> [Section, old background id, new background id]
        SETTING_EV_MSG,             //extraDara: QList<QVariant[String]> [Old msg, New msg]
        SETTING_EV_SOUND,           //extraDara: QList<QVariant[long]> [Old sound id, New sound id]
        SETTING_EV_ENDGAME,         //extraData: QList<QVariant[long]> [Old endgame id, New endgame id]
        SETTING_EV_KUP,             //extraData: bool [Activated?]
        SETTING_EV_KDOWN,           //extraData: bool [Activated?]
        SETTING_EV_KLEFT,           //extraData: bool [Activated?]
        SETTING_EV_KRIGHT,          //extraData: bool [Activated?]
        SETTING_EV_KRUN,            //extraData: bool [Activated?]
        SETTING_EV_KALTRUN,         //extraData: bool [Activated?]
        SETTING_EV_KJUMP,           //extraData: bool [Activated?]
        SETTING_EV_KALTJUMP,        //extraData: bool [Activated?]
        SETTING_EV_KDROP,           //extraData: bool [Activated?]
        SETTING_EV_KSTART,          //extraData: bool [Activated?]
        SETTING_EV_TRIACTIVATE,     //extraData: QList<QVariant[String]> [Old trigger, New trigger]
        SETTING_EV_TRIDELAY,        //extraData: QList<QVariant[long]> [Old delay, New delay]
        SETTING_SECISWARP,          //extraData: bool [Activated?]
        SETTING_SECNOBACK,          //extraData: bool [Activated?]
        SETTING_SECOFFSCREENEXIT,   //extraData: bool [Activated?]
        SETTING_SECUNDERWATER,      //extraData: bool [Activated?]
        SETTING_SECBACKGROUNDIMG,   //extraData: QList<QVariant[int]> [old background id, new background id]
        SETTING_SECMUSIC,           //extraData: QList<QVariant[int]> [old music id, new music id]
        SETTING_SECCUSTOMMUSIC,     //extraData: QList<QVariant[String]> [old custom music name, new custom music name]
        SETTING_LEVELNAME,          //extraData: QList<QVariant[String]> [old level name, new level name]
        SETTING_BGOSORTING
    };

    static QString settingToString(const HistorySettings::LevelSettingSubType &modLevelSetting)
    {
        switch (modLevelSetting) {
        case SETTING_SLIPPERY: return QObject::tr("Slippery");
        case SETTING_FRIENDLY: return QObject::tr("Friendly");
        case SETTING_BOSS: return QObject::tr("Boss");
        case SETTING_NOMOVEABLE: return QObject::tr("Not Moveable");
        case SETTING_MESSAGE: return QObject::tr("Message");
        case SETTING_DIRECTION: return QObject::tr("Direction");
        case SETTING_CHANGENPC: return QObject::tr("Included NPC");
        case SETTING_WATERTYPE: return QObject::tr("Water Type");
        case SETTING_LAYER: return QObject::tr("Layer");
        case SETTING_NOYOSHI: return QObject::tr("No Yoshi");
        case SETTING_ALLOWNPC: return QObject::tr("Allow NPC");
        case SETTING_LOCKED: return QObject::tr("Locked");
        case SETTING_WARPTYPE: return QObject::tr("Warp Type");
        case SETTING_NEEDASTAR: return QObject::tr("Need Stars");
        case SETTING_ENTRDIR: return QObject::tr("Entrance Direction");
        case SETTING_EXITDIR: return QObject::tr("Exit Direction");
        case SETTING_LEVELEXIT: return QObject::tr("Set Level Exit");
        case SETTING_LEVELENTR: return QObject::tr("Set Level Entrance");
        case SETTING_LEVELWARPTO: return QObject::tr("Level Warp To");
        case SETTING_GENACTIVATE: return QObject::tr("Activate Generator");
        case SETTING_GENTYPE: return QObject::tr("Generator Type");
        case SETTING_GENDIR: return QObject::tr("Generator Direction");
        case SETTING_GENTIME: return QObject::tr("Generator Time");
        case SETTING_ATTACHLAYER: return QObject::tr("Attach Layer");
        case SETTING_EV_DESTROYED: return QObject::tr("Event Block Destroyed");
        case SETTING_EV_HITED: return QObject::tr("Event Block Hited");
        case SETTING_EV_LAYER_EMP: return QObject::tr("Event Layer Empty");
        case SETTING_EV_ACTIVATE: return QObject::tr("Event NPC Activate");
        case SETTING_EV_DEATH: return QObject::tr("Event NPC Die");
        case SETTING_EV_TALK: return QObject::tr("Event NPC Talk");
        case SETTING_SPECIAL_DATA: return QObject::tr("NPC Special Data");
        case SETTING_EV_AUTOSTART: return QObject::tr("Autostart");
        case SETTING_EV_SMOKE: return QObject::tr("Layer Smoke Effect");
        case SETTING_EV_LHIDEADD: return QObject::tr("Add Hide Layer");
        case SETTING_EV_LHIDEDEL: return QObject::tr("Remove Hide Layer");
        case SETTING_EV_LSHOWADD: return QObject::tr("Add Show Layer");
        case SETTING_EV_LSHOWDEL: return QObject::tr("Remove Show Layer");
        case SETTING_EV_LTOGADD: return QObject::tr("Add Toggle Layer");
        case SETTING_EV_LTOGDEL: return QObject::tr("Remove Toggle Layer");
        case SETTING_EV_MOVELAYER: return QObject::tr("Moving Layer");
        case SETTING_EV_SPEEDLAYERX: return QObject::tr("Layer Speed Horizontal");
        case SETTING_EV_SPEEDLAYERY: return QObject::tr("Layer Speed Vertical");
        case SETTING_EV_AUTOSCRSEC: return QObject::tr("Autoscroll Layer");
        case SETTING_EV_AUTOSCRX: return QObject::tr("Autoscroll Layer Speed Horizontal");
        case SETTING_EV_AUTOSCRY: return QObject::tr("Autoscroll Layer Speed Vertical");
        case SETTING_EV_SECSIZE: return QObject::tr("Section Size");
        case SETTING_EV_SECMUS: return QObject::tr("Section Music");
        case SETTING_EV_SECBG: return QObject::tr("Section Background");
        case SETTING_EV_MSG: return QObject::tr("Message");
        case SETTING_EV_SOUND: return QObject::tr("Sound");
        case SETTING_EV_ENDGAME: return QObject::tr("End Game");
        case SETTING_EV_KUP: return QObject::tr("Up Key Activate");
        case SETTING_EV_KDOWN: return QObject::tr("Down Key Activate");
        case SETTING_EV_KLEFT: return QObject::tr("Left Key Activate");
        case SETTING_EV_KRIGHT: return QObject::tr("Right Key Activate");
        case SETTING_EV_KRUN: return QObject::tr("Run Key Activate");
        case SETTING_EV_KALTRUN: return QObject::tr("Alt Run Key Activate");
        case SETTING_EV_KJUMP: return QObject::tr("Jump Key Activate");
        case SETTING_EV_KALTJUMP: return QObject::tr("Alt Jump Key Activate");
        case SETTING_EV_KDROP: return QObject::tr("Drop Key Activate");
        case SETTING_EV_KSTART: return QObject::tr("Start Key Activate");
        case SETTING_EV_TRIACTIVATE: return QObject::tr("Trigger Activate");
        case SETTING_EV_TRIDELAY: return QObject::tr("Trigger Delay");
        case SETTING_SECISWARP: return QObject::tr("Is Warp");
        case SETTING_SECNOBACK: return QObject::tr("No Back");
        case SETTING_SECOFFSCREENEXIT: return QObject::tr("Off Screen Exit");
        case SETTING_SECUNDERWATER: return QObject::tr("Underwater");
        case SETTING_SECBACKGROUNDIMG: return QObject::tr("Background Image");
        case SETTING_SECMUSIC: return QObject::tr("Music");
        case SETTING_SECCUSTOMMUSIC: return QObject::tr("Custom Music");
        case SETTING_LEVELNAME: return QObject::tr("Level Name");
        case SETTING_BGOSORTING: return QObject::tr("BGO Sorting Priority");
        default:
            return QObject::tr("Unknown");
        }
    }

    static QString settingToString(const HistorySettings::WorldSettingSubType &modWorldSetting)
    {
        switch (modWorldSetting) {
        case SETTING_ALWAYSVISIBLE: return QObject::tr("Always Visible");
        case SETTING_BIGPATHBACKGROUND: return QObject::tr("Big Path Background");
        case SETTING_CHARACTER: return QObject::tr("Character");
        case SETTING_DOORID: return QObject::tr("Door ID");
        case SETTING_GAMESTARTPOINT: return QObject::tr("Game start point");
        case SETTING_GOTOX: return QObject::tr("Goto X");
        case SETTING_GOTOY: return QObject::tr("Goto Y");
        case SETTING_HUB: return QObject::tr("Hub styled world");
        case SETTING_INTROLEVEL: return QObject::tr("Intro Level");
        case SETTING_LEVELFILE: return QObject::tr("Level file");
        case SETTING_LEVELTITLE: return QObject::tr("Level title");
        case SETTING_PATHBACKGROUND: return QObject::tr("Path Background");
        case SETTING_PATHBYBOTTOM: return QObject::tr("Exit at bottom");
        case SETTING_PATHBYLEFT: return QObject::tr("Exit at left");
        case SETTING_PATHBYRIGHT: return QObject::tr("Exit at right");
        case SETTING_PATHBYTOP: return QObject::tr("Exit at top");
        case SETTING_RESTARTAFTERFAIL: return QObject::tr("Restart after fail");
        case SETTING_TOTALSTARS: return QObject::tr("Total stars");
        default:
            return QObject::tr("Unknown");
        }
    }
};

class Script{
public:
    enum CompilerType{
        COMPILER_AUTOCODE = 0,
        COMPILER_LUNALUA,
        COMPILER_PGELUA
    };

};


#endif // PGE_EDITOR_DEFINES_H
