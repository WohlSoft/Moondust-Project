#ifndef LVL_BGO_H
#define LVL_BGO_H

#include "../../physics/base_object.h"
#include "../../data_configs/obj_bgo.h"

#include <file_formats.h>

class LVL_Bgo : public PGE_Phys_Object
{
public:
    LVL_Bgo();
    ~LVL_Bgo();
    void init();

    LevelBGO* data; //Local settings

    bool animated;
    long animator_ID;

    obj_bgo * setup;//Global config

    //float posX();
    //float posY();
    void render(float camX, float camY);
};


#endif // LVL_BGO_H
