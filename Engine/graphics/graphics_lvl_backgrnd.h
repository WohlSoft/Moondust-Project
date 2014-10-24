#ifndef GRAPHICS_LVL_BACKGRND_H
#define GRAPHICS_LVL_BACKGRND_H

#include "../data_configs/obj_bg.h"
#include "../common_features/pge_texture.h"
#include "lvl_camera.h"


class LVL_Background
{
public:
    LVL_Background();
    LVL_Background(PGE_LevelCamera *parentCamera);

    ~LVL_Background();
    enum type
    {
        single_row      = 0,
        double_row      = 1,
        tiled           = 2,
        multi_layered   = 3
    };
    obj_BG * setup;

    PGE_LevelCamera *pCamera;

    type bgType;

    PGE_Texture txData1;
    PGE_Texture txData2;

    bool isAnimated;
    long animator_ID;

    PGEColor color;

    void setBg(obj_BG &bg);
    void setNone();
    void draw(float x, float y); //draw by camera position

private:
    void construct();
};


#endif // GRAPHICS_LVL_BACKGRND_H
