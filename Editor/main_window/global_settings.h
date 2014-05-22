#ifndef GLOBAL_SETTINGS_H
#define GLOBAL_SETTINGS_H

#include <QString>

class GlobalSettings
{
public:
    static QString locale;

};

class ConfStatus
{
//Configuration status
public:
    static long total_blocks;
    static long total_bgo;
    static long total_bg;
    static long total_npc;

    static long total_music_lvl;
    static long total_music_wld;
    static long total_music_spc;

    static QString configName;

};

#endif // GLOBAL_SETTINGS_H
