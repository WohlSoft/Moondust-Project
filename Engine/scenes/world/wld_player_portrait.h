#ifndef WLD_PLAYER_PORTRAIT_H
#define WLD_PLAYER_PORTRAIT_H

#include <data_configs/obj_player.h>
#include <common_features/matrix_animator.h>

class WorldScene_Portrait
{
public:
    WorldScene_Portrait();
    WorldScene_Portrait(unsigned long CharacterID,
                        unsigned long stateID,
                        int posX,
                        int posY,
                        std::string ani,
                        int framedelay,
                        int dir);
    WorldScene_Portrait(const WorldScene_Portrait &pt) = default;
    ~WorldScene_Portrait() = default;
    void init(unsigned long CharacterID,
              unsigned long stateID,
              int posX,
              int posY,
              std::string ani,
              int framedelay,
              int dir);
    void render();
    void update(double ticks);

    bool isValid() const;

private:
    bool m_isValid = false;
    int m_posX = 0;
    int m_posY = 0;
    int m_posX_render = 0;
    int m_posY_render = 0;
    obj_player       m_setup;
    obj_player_state m_state_cur;
    MatrixAnimator m_animator;
    PGE_Texture m_texture;
    int m_frameW = 0;
    int m_frameH = 0;
};

#endif // WLD_PLAYER_PORTRAIT_H
