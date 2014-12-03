#ifndef DEFINES_H
#define DEFINES_H


class ItemTypes
{
public:
    enum itemTypes
    {
        LVL_Block   = 0,
        LVL_BGO,
        LVL_NPC,
        WLD_Tile,
        WLD_Scenery,
        WLD_Path,
        WLD_Level,
        WLD_MusicBox
    };
};

class Script{
public:
    enum CompilerType{
        COMPILER_AUTOCODE = 0,
        COMPILER_LUNALUA,
        COMPILER_PGELUA
    };

};


#endif // DEFINES_H
