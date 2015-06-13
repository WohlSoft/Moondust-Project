#ifndef CONTROLLER_KEY_MAP_H
#define CONTROLLER_KEY_MAP_H

struct KM_Key
{
    inline KM_Key()
    {
        val=-1;
        id=-1;
        type=-1;
    }
    int val;
    int id;
    int type;
};

struct KeyMap
{
    KM_Key start;
    KM_Key drop;

    KM_Key left;
    KM_Key right;
    KM_Key up;
    KM_Key down;

    KM_Key jump;
    KM_Key jump_alt;

    KM_Key run;
    KM_Key run_alt;
};

struct KeyMapJoyCtrls
{
    enum CtrlTypes
    {
        NoControl=-1,
        JoyAxis=0,
        JoyBallX,
        JoyBallY,
        JoyHat,
        JoyButton
    };
    inline KeyMapJoyCtrls()
    {
        start=-1; drop=-1;
        left=-1; right=-1; up=-1; down=-1;
        jump=-1; jump_alt=-1; run=-1; run_alt=-1;
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

