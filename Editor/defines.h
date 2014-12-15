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

class Script{
public:
    enum CompilerType{
        COMPILER_AUTOCODE = 0,
        COMPILER_LUNALUA,
        COMPILER_PGELUA
    };

};


#endif // PGE_EDITOR_DEFINES_H
