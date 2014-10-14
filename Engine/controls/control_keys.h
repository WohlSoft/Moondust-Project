#ifndef CONTROL_KEYS_H
#define CONTROL_KEYS_H

struct controller_keys
{
    bool start;

    bool left;
    bool right;
    bool up;
    bool down;

    bool run;
    bool jump;
    bool alt_run;
    bool alt_jump;

    bool drop;
};

controller_keys ResetControlKeys();

#endif

