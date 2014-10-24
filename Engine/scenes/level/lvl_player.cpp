#include "lvl_player.h"
#include "../../graphics/window.h"

#include <QtDebug>

LVL_Player::LVL_Player()
{
    camera = NULL;
    worldPtr = NULL;
    playerID = 0;

    type = LVLPlayer;

    force=1000.0f;
    hMaxSpeed=24.0f;
    hRunningMaxSpeed=48.0f;
    fallMaxSpeed=720.0f;

    JumpPressed=false;
    onGround=true;
    jumpForce=0;

    curHMaxSpeed = hMaxSpeed;
    isRunning = false;
}

LVL_Player::~LVL_Player()
{
    qDebug() << "Destroy player";

    if(physBody && worldPtr)
    {
        worldPtr->DestroyBody(physBody);
        physBody->SetUserData(NULL);
        physBody = NULL;
    }
}

void LVL_Player::init()
{
    if(!worldPtr) return;

    setSize(data->w, data->h);

    playerID = data->id;

    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set
            (
                PhysUtil::pix2met((float)data->x + posX_coefficient),
                PhysUtil::pix2met((float)data->y + posY_coefficient)
            );

//    bodyDef.position.Set(PhysUtil::pix2met((float)data->x + ((float)data->w/2)),
//            PhysUtil::pix2met((float)data->y + ((float)data->w/2) ) );

    bodyDef.fixedRotation = true;
    bodyDef.bullet = true;
    bodyDef.userData = (void*)dynamic_cast<PGE_Phys_Object *>(this);

    physBody = worldPtr->CreateBody(&bodyDef);

    b2PolygonShape shape;
    shape.SetAsBox(PhysUtil::pix2met(posX_coefficient),
                   PhysUtil::pix2met(posY_coefficient));

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 1.0f; fixtureDef.friction = 0.3f;
    physBody->CreateFixture(&fixtureDef);

    //qDebug() <<"Start position is " << posX() << posY();
}




void LVL_Player::update()
{
    if(!physBody) return;
    if(!camera) return;

    if(_player_moveup)
    {
        setPos(posX(), posY()-2 );
        _player_moveup = false;
    }

    if(physBody->GetLinearVelocity().y > 72)
        physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, 72));


    //Running key
    if(keys.run)
    {
        if(!isRunning)
        {
            curHMaxSpeed = hRunningMaxSpeed;
            isRunning=true;
        }
    }
    else
    {
        if(isRunning)
        {
            curHMaxSpeed = hMaxSpeed;
            isRunning=false;
        }
    }


    //If left key is pressed
    if(keys.right)
        if(physBody->GetLinearVelocity().x <= curHMaxSpeed)
            physBody->ApplyForceToCenter(b2Vec2(force, 0.0f), true);

    //If right key is pressed
    if(keys.left)
        if(physBody->GetLinearVelocity().x >= -curHMaxSpeed)
            physBody->ApplyForceToCenter(b2Vec2(-force, 0.0f), true);

    if( keys.jump)
    {
        if(!JumpPressed)
        {
            JumpPressed=true;
            physBody->SetLinearVelocity(b2Vec2(physBody->GetLinearVelocity().x, -65.0f-fabs(physBody->GetLinearVelocity().x/5)));
        }
    }
    else
    {
        if(JumpPressed)
        {
            JumpPressed=false;
        }
    }


    //Return player to start position on fall down
    if( posY() > camera->s_bottom+height )
        physBody->SetTransform(b2Vec2(
                PhysUtil::pix2met(data->x + (posX_coefficient) ),
                PhysUtil::pix2met(data->y + (posY_coefficient) )), 0.0f);


    //Connection of section opposite sides
    if(camera->isWarp)
    {
        if(posX() < camera->s_left-width-1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->s_right+posX_coefficient),
                 physBody->GetPosition().y), 0.0f);
        else
        if(posX() > camera->s_right + 1 )
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->s_left-posX_coefficient ),
                 physBody->GetPosition().y), 0.0f
                                   );
    }
    else
    {
        //Prevent moving of player away from screen
        if( posX() < camera->s_left)
        {
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->s_left + posX_coefficient),
                    physBody->GetPosition().y), 0.0f);
            physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
        }
        else
        if( posX()+width > camera->s_right)
        {
            physBody->SetTransform(b2Vec2(
                 PhysUtil::pix2met(camera->s_right-posX_coefficient ),
                    physBody->GetPosition().y), 0.0f
                                   );
            physBody->SetLinearVelocity(b2Vec2(0, physBody->GetLinearVelocity().y));
        }

    }

    camera->setPos( posX() - PGE_Window::Width/2 + posX_coefficient,
                    posY() - PGE_Window::Height/2 + posY_coefficient );



}



void LVL_Player::render(float camX, float camY)
{
    QRectF player = QRectF( posX()
                            -camX,

                            posY()
                            -camY,

                            width,
                            height
                         );
//        qDebug() << "PlPos" << pl.left() << pl.top() << player.right() << player.bottom();

    glDisable(GL_TEXTURE_2D);
    glColor4f( 0.f, 0.f, 1.f, 1.f);
    glBegin( GL_QUADS );
        glVertex2f( player.left(), player.top());
        glVertex2f( player.right(), player.top());
        glVertex2f( player.right(),  player.bottom());
        glVertex2f( player.left(),  player.bottom());
    glEnd();

}

