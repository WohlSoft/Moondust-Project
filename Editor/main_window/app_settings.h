#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <QString>

class AppSettings
{
public:


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

    QString configName;

};

#endif // APP_SETTINGS_H
