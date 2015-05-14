#ifndef WLD_PLAYER_PORTRAIT_H
#define WLD_PLAYER_PORTRAIT_H

#include <data_configs/obj_player.h>
#include <common_features/matrix_animator.h>

class WorldScene_Portrait
{
public:
    WorldScene_Portrait();
    WorldScene_Portrait(int CharacterID, int stateID, int _posX, int _posY, QString ani, int framedelay, int dir);
    WorldScene_Portrait(const WorldScene_Portrait &pt);
    ~WorldScene_Portrait();
    void render();
    void update(int ticks);
private:
    int posX;
    int posY;
    int posX_render;
    int posY_render;
    obj_player       setup;
    obj_player_state state_cur;
    MatrixAnimator animator;
    PGE_Texture texture;
    int frameW;
    int frameH;
};

#endif // WLD_PLAYER_PORTRAIT_H

