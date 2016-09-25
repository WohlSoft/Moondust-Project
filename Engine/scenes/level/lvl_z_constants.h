#ifndef LVL_Z_CONSTANTS_H
#define LVL_Z_CONSTANTS_H

struct LevelZOrderTable
{
    LevelZOrderTable();

    const long double backImage; //Background

    //Background-2
    const long double bgoBack2; // backround BGO

    const long double blockBack2; // sizable blocks

    //Background-1
    const long double bgoBack1; // backround BGO

    const long double npcBack; // background NPC
    const long double blockBack1; // standart block
    const long double npcStd; // standart NPC
    const long double player; //player Point

    //Foreground-1
    const long double bgoFront1; // foreground BGO
    const long double npcFront; // foreground NPC
    const long double blockFront1; // foreground blocks
    //Foreground-2
    const long double bgoFront2; // foreground BGO

    //System layer
    const long double debugFront;
};

#endif // LVL_Z_CONSTANTS_H
