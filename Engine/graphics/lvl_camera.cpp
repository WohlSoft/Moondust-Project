#include "lvl_camera.h"
#include "window.h"

PGE_LevelCamera::PGE_LevelCamera()
{
    worldPtr = NULL;
    sensor = NULL;
    section = 0;
    isWarp = false;
}

PGE_LevelCamera::~PGE_LevelCamera()
{
    if(sensor && worldPtr)
        worldPtr->DestroyBody(sensor);
}


void PGE_LevelCamera::setWorld(b2World *wld)
{
    worldPtr = wld;
}


void PGE_LevelCamera::init(float x, float y, float w, float h)
{
    if(!worldPtr) return;

    if(!sensor)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(PhysUtil::pix2met(x + (w/2)),
                             PhysUtil::pix2met(y + (h/2) ) );
        bodyDef.fixedRotation = true;
        sensor = worldPtr->CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.SetAsBox(PhysUtil::pix2met(w)/2-0.1, PhysUtil::pix2met(h)/2-0.1);
        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.isSensor = true;
        fixtureDef.density = 0.0f; fixtureDef.friction = 0.0f;
        sensor->CreateFixture(&fixtureDef);
    }

}


int PGE_LevelCamera::w()
{
    return width;
}

int PGE_LevelCamera::h()
{
    return height;
}

int PGE_LevelCamera::posX()
{
    return PhysUtil::met2pix(sensor->GetPosition().x) - width/2;
}

int PGE_LevelCamera::posY()
{
    return PhysUtil::met2pix(sensor->GetPosition().y) - height/2;
}

void PGE_LevelCamera::setPos(int x, int y)
{
    pos_x = -x;
    pos_y = -y;

    if(-pos_x < s_left)
        pos_x = -s_left;
    if(-(pos_x-PGE_Window::Width) > s_right)
        pos_x = -s_right+PGE_Window::Width;

    if(-pos_y < s_top)
        pos_y = -s_top;
    if(-(pos_y-PGE_Window::Height) > s_bottom)
        pos_y = -s_bottom+PGE_Window::Height;

    sensor->SetTransform(b2Vec2( PhysUtil::pix2met(-pos_x), PhysUtil::pix2met(-pos_y)), 0);
}

void PGE_LevelCamera::setSize(int w, int h)
{
    width = w;
    height = h;
    b2PolygonShape* shape = (b2PolygonShape*)(sensor->GetFixtureList()->GetShape());
    shape->m_vertices[0].Set(-PhysUtil::pix2met(w)/2, -PhysUtil::pix2met(h)/2);
    shape->m_vertices[1].Set( PhysUtil::pix2met(w)/2, -PhysUtil::pix2met(h)/2);
    shape->m_vertices[2].Set( PhysUtil::pix2met(w)/2,  PhysUtil::pix2met(h)/2);
    shape->m_vertices[3].Set(-PhysUtil::pix2met(w)/2,  PhysUtil::pix2met(h)/2);
}


void PGE_LevelCamera::update()
{
    objects_to_render.clear();

    if(!sensor) return;

    for(b2ContactEdge* ce = sensor->GetContactList(); ce; ce = ce->next)
    {
        b2Contact* c = ce->contact;
        PGE_Phys_Object * visibleBody = static_cast<PGE_Phys_Object *>(c->GetFixtureA()->GetBody()->GetUserData());
        switch(visibleBody->type)
        {
        case PGE_Phys_Object::LVL_Block:
        case PGE_Phys_Object::LVL_BGO:
        case PGE_Phys_Object::LVL_NPC:
        case PGE_Phys_Object::LVL_Effect:
            objects_to_render.push_back(visibleBody);
        }
    }
}


void PGE_LevelCamera::changeSectionBorders(long left, long top, long right, long bottom)
{
    s_left = left;
    s_top = top;
    s_right = right;
    s_bottom = bottom;
}

PGE_RenderList PGE_LevelCamera::renderObjects()
{
    return objects_to_render;
}


void PGE_LevelCamera::drawBackground()
{

}
