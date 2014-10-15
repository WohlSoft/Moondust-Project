#ifndef LVL_CAMERA_H
#define LVL_CAMERA_H

#include <Box2D/Box2D.h>
#include "../physics/base_object.h"
#include "graphics.h"
#include <vector>

#include <file_formats.h>


typedef QVector<PGE_Phys_Object *>  PGE_RenderList;

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
    void drawBackground();

    void changeSectionBorders(long left, long top, long right, long bottom);
    PGE_RenderList renderObjects();

    LevelSection *section;

    LVL_Background background;

    bool isWarp;

    /// Limits of section motion
    int s_top;
    int s_bottom;
    int s_left;
    int s_right;

    int pos_x;
    int pos_y;

private:
    PGE_RenderList objects_to_render;

    int width;
    int height;

    b2World * worldPtr;
    b2Body* sensor;
};

#endif // LVL_CAMERA_H
