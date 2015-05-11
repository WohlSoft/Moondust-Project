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

#endif // CONTROLLER_KEY_MAP_H

