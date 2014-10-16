#include "lvl_camera.h"
#include "window.h"

#include <QtDebug>

PGE_LevelCamera::PGE_LevelCamera()
{
    worldPtr = NULL;
    sensor = NULL;
    section = 0;
    isWarp = false;
    width=800;
    height=600;
}

PGE_LevelCamera::~PGE_LevelCamera()
{

    qDebug() << "Destroy camera";

    if(sensor && worldPtr)
    {
        worldPtr->DestroyBody(sensor);
        sensor->SetUserData(NULL);
        sensor = NULL;
    }

}


void PGE_LevelCamera::setWorld(b2World *wld)
{
    worldPtr = wld;
}


void PGE_LevelCamera::init(float x, float y, float w, float h)
{
    if(!worldPtr) return;

    pos_x = x;
    pos_y = y;
    width = w;
    height = h;

    if(!sensor)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(PhysUtil::pix2met(x + (w/2)),
                             PhysUtil::pix2met(y + (h/2) ) );
        bodyDef.fixedRotation = true;
        bodyDef.gravityScale = 0;
        sensor = worldPtr->CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.SetAsBox(PhysUtil::pix2met(w)/2, PhysUtil::pix2met(h)/2);
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

qreal PGE_LevelCamera::posX()
{
    return PhysUtil::met2pix(sensor->GetPosition().x) - width/2;
}

qreal PGE_LevelCamera::posY()
{
    return PhysUtil::met2pix(sensor->GetPosition().y) - height/2;
}

void PGE_LevelCamera::setPos(int x, int y)
{
    pos_x = x;
    pos_y = y;

    if(pos_x < s_left)
        pos_x = s_left;
    if((pos_x+width) > s_right)
        pos_x = s_right-width;

    if(pos_y < s_top)
        pos_y = s_top;
    if((pos_y+height) > s_bottom)
        pos_y = s_bottom-height;

    sensor->SetTransform(b2Vec2( PhysUtil::pix2met(pos_x+width/2),
                                 PhysUtil::pix2met(pos_y+height/2)), 0);
    sensor->SetLinearVelocity(b2Vec2(0,0));
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
    //int contacts = 0;

    for(b2ContactEdge* ce = sensor->GetContactList(); ce; ce = ce->next)
    {
        b2Contact* c = ce->contact;
        //contacts++;

        PGE_Phys_Object * visibleBody;
        visibleBody = static_cast<PGE_Phys_Object *>(c->GetFixtureA()->GetBody()->GetUserData());

        if(visibleBody==NULL)
            visibleBody = static_cast<PGE_Phys_Object *>(c->GetFixtureB()->GetBody()->GetUserData());

        if(visibleBody==NULL)
            continue;

        switch(visibleBody->type)
        {
        case PGE_Phys_Object::LVLBlock:
        case PGE_Phys_Object::LVLBGO:
        case PGE_Phys_Object::LVLNPC:
        case PGE_Phys_Object::LVLPlayer:
        case PGE_Phys_Object::LVLEffect:
            objects_to_render.push_back(visibleBody);
        }
    }
    //qDebug() << "Contacts" << contacts;
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
