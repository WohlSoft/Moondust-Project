#ifndef LVL_BLOCK_H
#define LVL_BLOCK_H

#include "../../physics/base_object.h"

#include <file_formats.h>

class LVL_Block : public PGE_Phys_Object
{
public:
    LVL_Block();
    ~LVL_Block();
    void init();

    LevelBlock* data;
    bool slippery;

    //float posX();
    //float posY();
};

#endif // LVL_BLOCK_H
