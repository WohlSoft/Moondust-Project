#ifndef LVL_PLAYER_H
#define LVL_PLAYER_H

#include "../../graphics/lvl_camera.h"

#include "../../physics/base_object.h"
#include "../../controls/controllable_object.h"
#include <file_formats.h>

class LVL_Player :
        public PGE_Phys_Object,
        public ControllableObject
{
    public:
        LVL_Player();
        ~LVL_Player();
        void init();
        void update();

        int playerID;

        float32 force; //!< Move force
        float32 hMaxSpeed; //!< Max walk speed
        float32 hRunningMaxSpeed; //!< Max run speed
        float32 fallMaxSpeed; //!< Max fall speed

        float32 curHMaxSpeed; //!< Current moving max speed
        bool isRunning;

        bool JumpPressed;
        bool allowJump;
        int jumpForce;


        PlayerPoint * data;

        PGE_LevelCamera * camera;
};

#endif // LVL_PLAYER_H
