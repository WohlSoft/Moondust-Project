#ifndef DEFINES_H
#define DEFINES_H


class ItemTypes
{
public:
    //Multiselectable
    enum itemTypes
    {
        LVL_Block       = 1<<0,
        LVL_BGO         = 1<<1,
        LVL_NPC         = 1<<2,
        LVL_PhysEnv     = 1<<3,
        LVL_Door        = 1<<4,
        LVL_Player      = 1<<5,
        WLD_Tile        = 1<<6,
        WLD_Scenery     = 1<<7,
        WLD_Path        = 1<<8,
        WLD_Level       = 1<<9,
        WLD_MusicBox    = 1<<10
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
