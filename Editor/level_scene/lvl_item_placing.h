#ifndef ITEM_PLACING_H
#define ITEM_PLACING_H

#include "../file_formats/lvl_filedata.h"
class LvlPlacingItems
{
public:
    static LevelNPC npcSet;
    static LevelBlock blockSet;
    static LevelBGO bgoSet;
    static LevelWater waterSet;

    enum doorType{
        DOOR_Entrance=0,
        DOOR_Exit
    };

    static int doorType;
    static int waterType;
    static int playerID;

    static int gridSz;
    static QPoint gridOffset;

};

#endif // ITEM_PLACING_H
