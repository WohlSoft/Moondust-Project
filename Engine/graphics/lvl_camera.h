#ifndef LVL_CAMERA_H
#define LVL_CAMERA_H

#include <Box2D/Box2D.h>
#include "../physics/base_object.h"
#include <vector>


typedef std::vector<PGE_Phys_Object *>  PGE_RenderList;

class PGE_LevelCamera
{
public:
    PGE_LevelCamera();
    ~PGE_LevelCamera();
    int w(); //!< Width
    int h(); //!< Height
    int posX(); //!< Position X
    int posY(); //!< Position Y

    void setWorld(b2World * wld);
    void init(float x, float y, float w, float h);

    void setPos(int x, int y);
    void setSize(int w, int h);
    void update();
    PGE_RenderList renderObjects();

private:
    PGE_RenderList objects_to_render;
    int width;
    int height;
    b2World * worldPtr;
    b2Body* sensor;
};

#endif // LVL_CAMERA_H
