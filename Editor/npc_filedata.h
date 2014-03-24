#ifndef NPC_FILEDATA_H
#define NPC_FILEDATA_H

struct NPCConfigFile
{
    bool ReadFileValid;

    /* Using options flags BEGIN */
    bool en_gfxoffsetx;
    bool en_gfxoffsety;
    bool en_width;
    bool en_height;
    bool en_gfxwidth;
    bool en_gfxheight;
    bool en_score;
    bool en_playerblock;
    bool en_playerblocktop;
    bool en_npcblock;
    bool en_npcblocktop;
    bool en_grabside;
    bool en_grabtop;
    bool en_jumphurt;
    bool en_nohurt;
    bool en_noblockcollision;
    bool en_cliffturn;
    bool en_noyoshi;
    bool en_foreground;
    bool en_speed;
    bool en_nofireball;
    bool en_nogravity;
    bool en_frames;
    bool en_framespeed;
    bool en_framestyle;
    bool en_noiceball;
    /* Using options flags END */

    int gfxoffsetx;
    int gfxoffsety;
    unsigned int width;
    unsigned int height;
    unsigned int gfxwidth;
    unsigned int gfxheight;
    unsigned int score;
    bool playerblock;
    bool playerblocktop;
    bool npcblock;
    bool npcblocktop;
    bool grabside;
    bool grabtop;
    bool jumphurt;
    bool nohurt;
    bool noblockcollision;
    bool cliffturn;
    bool noyoshi;
    bool foreground;
    float speed;
    bool nofireball;
    bool nogravity;
    unsigned int frames;
    unsigned int framespeed;
    unsigned int framestyle;
    bool noiceball;
};

#endif // NPC_FILEDATA_H
