#ifndef CONTROLLER_KEY_MAP_H
#define CONTROLLER_KEY_MAP_H

struct KeyMap
{
    inline KeyMap()
    {
        start=0; drop=0;
        left=0; right=0; up=0; down=0;
        jump=0; jump_alt=0; run=0; run_alt=0;
    }
    int start;
    int drop;

    int left;
    int right;
    int up;
    int down;

    int jump;
    int jump_alt;

    int run;
    int run_alt;
};

struct KeyMapJoyCtrls
{
    enum CtrlTypes
    {
        JoyAxisX=0,
        JoyAxisY,
        JoyBallX,
        JoyBallY,
        JoyHat,
        JoyButton
    };
    inline KeyMapJoyCtrls()
    {
        start=0; drop=0;
        left=0; right=0; up=0; down=0;
        jump=0; jump_alt=0; run=0; run_alt=0;
    }
    int start;

    int left;
    int right;
    int up;
    int down;

    int run;
    int jump;
    int run_alt;
    int jump_alt;

    int drop;
};

#endif // CONTROLLER_KEY_MAP_H

