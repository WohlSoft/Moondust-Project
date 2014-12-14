#ifndef PGE_EDITOR_DEFINES_H
#define PGE_EDITOR_DEFINES_H

class ItemTypes
{
public:
    enum itemTypes
    {
        LVL_Block       = 0,
        LVL_BGO         = 1,
        LVL_NPC         = 2,
        LVL_PhysEnv     = 3,
        LVL_Door        = 4,
        LVL_Player      = 5,
        WLD_Tile        = 6,
        WLD_Scenery     = 7,
        WLD_Path        = 8,
        WLD_Level       = 9,
        WLD_MusicBox    = 10
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

class Script{
public:
    enum CompilerType{
        COMPILER_AUTOCODE = 0,
        COMPILER_LUNALUA,
        COMPILER_PGELUA
    };

};


#endif // PGE_EDITOR_DEFINES_H
