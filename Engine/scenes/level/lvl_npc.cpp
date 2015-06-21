/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2015 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "lvl_npc.h"
#include "../../data_configs/config_manager.h"
#include <graphics/gl_renderer.h>
#include <common_features/number_limiter.h>
#include <common_features/maths.h>

#include "lvl_section.h"

#include "lvl_scene_ptr.h"

LVL_Npc::LVL_Npc() : PGE_Phys_Object()
{
    type = LVLNPC;
    data = FileFormats::dummyLvlNpc();
    animated=false;
    animator_ID=0;
    killed=false;
    isActivated=false;
    _isInited=false;
    isLuaNPC=false;
    isWarping=false;

    warpDirectO=0;
    warpSpriteOffset=1.0f;
    warpFrameW=0.0f;
    warpFrameH=0.0f;

    _onGround=false;

    _stucked=false;

    bumpDown=false;
    bumpUp=false;

    forceCollideCenter=true;
    _heightDelta=0.0f;
}

LVL_Npc::~LVL_Npc()
{}

void LVL_Npc::init()
{
    if(_isInited) return;

    phys_setup.gravityAccel=ConfigManager::marker_npc.phs_gravity_accel;
    phys_setup.max_vel_y=   ConfigManager::marker_npc.phs_max_fall_speed;

    transformTo_x(data.id);
    setPos(data.x, data.y);
    _syncSection();
    if(isLuaNPC){
        try{
            lua_onInit();
        } catch (luabind::error& e) {
            LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
        }
    }

    _isInited=true;
}

void LVL_Npc::setDirection(int dir)
{
    if(dir==0) dir=(rand()%2) ? -1 : 1;
    _direction=Maths::sgn(dir);
    int imgOffsetX = -((int)round( - ( ( (double)setup->gfx_w - (double)setup->width ) / 2 ) )
                        +(-((double)setup->gfx_offset_x)*_direction));
    int imgOffsetY = -(int)round( - (double)setup->gfx_h + (double)setup->height + (double)setup->gfx_offset_y);
    offset.setPoint(imgOffsetX, imgOffsetY);
}

int LVL_Npc::direction()
{
    return _direction;
}

void LVL_Npc::kill()
{
    killed=true;
    sct()->unregisterElement(this);
    LvlSceneP::s->dead_npcs.push_back(this);
    LvlSceneP::s->launchStaticEffectC(setup->effect_1, posCenterX(), posCenterY(), 1, 0, 0, 0, 0, _direction);
}

void LVL_Npc::transformTo(long id, int type)
{
    if(id<=0) return;

    if(type==2)//block
    {
    //        transformTask_block t;
    //        t.block = this;
    //        t.id=id;
    //        t.type=type;

        //LvlSceneP::s->block_transforms.push_back(t);
    }
    if(type==1)//Other NPC
    {
        // :-P
    }
}

void LVL_Npc::transformTo_x(long id)
{
    if(_isInited)
    {
        if(data.id==(unsigned)abs(id)) return;
        if(!ConfigManager::lvl_npc_indexes.contains(id))
            return;
        setup = &ConfigManager::lvl_npc_indexes[id];
    }

    data.id=id;

    double targetZ = 0;
    if(setup->foreground)
        targetZ = LevelScene::Z_npcFore;
    else
    if(setup->background)
        targetZ = LevelScene::Z_npcBack;
    else
        targetZ = LevelScene::Z_npcStd;

    z_index += targetZ;

    LevelScene::zCounter += 0.00000001;
    z_index += LevelScene::zCounter;

    long tID = ConfigManager::getNpcTexture(data.id);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = ((setup->frames>1) || (setup->framestyle>0));
        animator_ID = setup->animator_ID;
    }

    warpFrameW = texture.w;
    warpFrameH = texture.h;

    setSize(setup->width, setup->height);

    setDirection(_direction);
    frameSize.setSize(setup->gfx_w, setup->gfx_h);
    animator.construct(texture, *setup);

    setDefaults();
    setGravityScale(setup->gravity ? 1.0f : 0.f);

    if(setup->block_player)
        collide_player = COLLISION_ANY;
    else
    if(setup->block_player_top)
        collide_player = COLLISION_TOP;
    else
        collide_player = COLLISION_NONE;

    if(setup->block_npc)
        collide_npc = COLLISION_ANY;
    else
    if(setup->block_npc_top)
        collide_npc = COLLISION_TOP;
    else
        collide_npc = COLLISION_NONE;
}


void LVL_Npc::update(float tickTime)
{
    float accelCof=tickTime/1000.0f;
    if(killed) return;

    PGE_Phys_Object::update(tickTime);
    timeout-=tickTime;
    animator.manualTick(tickTime);

    if(motionSpeed!=0)
    {
        if(!collided_left.isEmpty())
            setDirection(1);
        else
        if(!collided_right.isEmpty())
            setDirection(-1);
        setSpeedX((motionSpeed*accelCof)*_direction);
    }

    LVL_Section *section=sct();
    PGE_RectF sBox = section->sectionRect();

    if(section->isWarp())
    {
        if(posX()<sBox.left()-_width-1 )
            setPosX(sBox.right()+_width-1);
        else
        if(posX()>sBox.right() + 1 )
            setPosX(sBox.left()-_width+1);
    }

    try{
        lua_onLoop(tickTime);
    } catch (luabind::error& e) {
        LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
    }
}

void LVL_Npc::render(double camX, double camY)
{
    if(killed) return;
    if(!isActivated) return;

    AniPos x(0,1);
    if(animated)
    {
        if(is_scenery)
            x=ConfigManager::Animator_NPC[animator_ID].image(_direction);
        else
            x=animator.image(_direction);
    }

    /*
    GlRenderer::renderTexture(&texture, posX()-camX+(offset.x()+(-((double)setup->gfx_offset_x)*direction)),
                              posY()-camY+offset.y(),
                              frameSize.w(),
                              frameSize.h(),
                              x.first, x.second);
    */

    PGE_RectF tPos; tPos.setLeft(0.0); tPos.setRight(1.0);
    tPos.setTop(x.first); tPos.setBottom(x.second);

    PGE_RectF npc;
    npc.setRect(round(posX()-camX)-offset.x(),
                   round(posY()-offset.y())-camY,
                   frameSize.w(),
                   frameSize.h()
                   );
    if(isWarping)
    {
        if(warpSpriteOffset>=1.0)
            return;
        //     Exit direction: [1] down  [3] up  [4] left  [2] right
        // Entrance direction: [3] down, [1] up, [2] left, [4] right
        switch(warpDirectO)
        {
            case WARP_LEFT://Left entrance, right Exit
                {
                    float wOfs = offset.x()/warpFrameW;//Relative X offset
                    float wOfsF = frameSize.w()/warpFrameW; //Relative hitbox width
                    tPos.setLeft(tPos.left()+wOfs+(warpSpriteOffset*wOfsF));
                    npc.setLeft( npc.left()+offset.x() );
                    npc.setRight( npc.right()-(warpSpriteOffset*_width) );
                }
                break;
            case WARP_TOP://Up entrance, down exit
                {
                    float hOfs = offset.y()/warpFrameH;//Relative Y offset
                    float hOfsF = frameSize.h()/warpFrameH; //Relative hitbox Height
                    tPos.setTop(tPos.top()+hOfs+(warpSpriteOffset*hOfsF));
                    npc.setTop( npc.top()+offset.y() );
                    npc.setBottom( npc.bottom()-(warpSpriteOffset*frameSize.h()) );
                }
                break;
            case WARP_RIGHT://right emtramce. left exit
                {
                    float wOfs =  offset.x()/warpFrameW;               //Relative X offset
                    float fWw =   1.0;   //Relative width of frame
                    float wOfHB = frameSize.w()/warpFrameW;                 //Relative width of hitbox
                    float wWAbs = warpFrameW*fWw;                   //Absolute width of frame
                    if(!warpResizedBody)
                    {
                        tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpSpriteOffset*wOfHB));
                        npc.setLeft( npc.left()+(warpSpriteOffset*frameSize.w()) );
                        npc.setRight( npc.right()-(wWAbs-offset.x()-frameSize.w()) );
                    }
                    else
                    {
                        tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpSpriteOffset*wOfHB));
                        npc.setLeft( npc.left()+offset.x() );
                        npc.setRight( npc.right()-(wWAbs-offset.x()-frameSize.w()*(1.0-warpSpriteOffset) ) );
                    }
                }
                break;
            case WARP_BOTTOM://down entrance, up exit
                {
                    float hOfs =  offset.y()/warpFrameH;               //Relative Y offset
                    float fHh =   animator.sizeOfFrame().h();  //Relative height of frame
                    float hOfHB = frameSize.h()/warpFrameH;                //Relative height of hitbox
                    float hHAbs = warpFrameH*fHh;                   //Absolute height of frame
                    if(!warpResizedBody)
                    {
                        tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpSpriteOffset*hOfHB));
                        npc.setTop( npc.top()+(warpSpriteOffset*frameSize.h()) );
                        npc.setBottom( npc.bottom()-(hHAbs-offset.y()-frameSize.h()) );
                    }
                    else
                    {
                        tPos.setBottom(tPos.bottom()-(fHh-hOfHB-hOfs)-(warpSpriteOffset*hOfHB));
                        npc.setTop( npc.top()+offset.y() );
                        npc.setBottom( npc.bottom()-(hHAbs-offset.y()-frameSize.h()*(1.0-warpSpriteOffset) ) );
                    }
                }
                break;
            default:
                break;
        }
    }

    GlRenderer::renderTexture(&texture,
                              npc.x(),
                              npc.y(),
                              npc.width(),
                              npc.height(),
                              tPos.top(),
                              tPos.bottom(),
                              tPos.left(),
                              tPos.right());
}

void LVL_Npc::setDefaults()
{
    if(!setup) return;
    setDirection(data.direct);
    motionSpeed = ((!data.nomove)&&(setup->movement)) ? ((float)setup->speed) : 0.0f;
    is_scenery  = setup->scenery;
}

void LVL_Npc::Activate()
{
    if(!is_scenery)
        timeout=4000;
    else
        timeout=150;
    if(isActivated) return;

    animator.start();
    if(isLuaNPC){
        try{
            lua_onActivated();
        } catch (luabind::error& e) {
            LvlSceneP::s->getLuaEngine()->postLateShutdownError(e);
        }
    }
    isActivated=true;
}

void LVL_Npc::deActivate()
{
    isActivated=false;
    if(!is_scenery)
    {
        setDefaults();
        setPos(data.x, data.y);
    }
    animator.stop();
}

void LVL_Npc::setSpriteWarp(float depth, LVL_Npc::WarpingSide direction, bool resizedBody)
{
    NumberLimiter::apply(depth, 0.0f, 1.0f);
    isWarping=(depth>0.0f);
    warpSpriteOffset=depth;
    warpResizedBody=resizedBody;
    warpDirectO=direction;
}

void LVL_Npc::resetSpriteWarp()
{
    warpSpriteOffset=0.0f;
    warpResizedBody=false;
    isWarping=false;
}

void LVL_Npc::lua_setSequenceLeft(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceLeft exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceL(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequenceRight(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequenceRight exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequenceR(luabind_utils::convArrayTo<int>(frames));
}

void LVL_Npc::lua_setSequence(luabind::object frames)
{
    int ltype = luabind::type(frames);
    if(luabind::type(frames) != LUA_TTABLE){
        luaL_error(frames.interpreter(), "setSequence exptected int-array, got %s", lua_typename(frames.interpreter(), ltype));
        return;
    }
    animator.setSequence(luabind_utils::convArrayTo<int>(frames));
}

bool LVL_Npc::isInited()
{
    return _isInited;
}









void LVL_Npc::updateCollisions()
{
    foot_contacts_map.clear();
    _onGround=false;
    foot_sl_contacts_map.clear();
    //contactedWarp = NULL;
    //contactedWithWarp=false;
    //climbable_map.clear();
    environments_map.clear();
    contacted_bgos.clear();

    collided_top.clear();
    collided_left.clear();
    collided_right.clear();
    collided_bottom.clear();
    collided_center.clear();

    #ifdef COLLIDE_DEBUG
    qDebug() << "=====Collision check and resolve begin======";
    #endif

    PGE_Phys_Object::updateCollisions();

    bool resolveBottom=false;
    bool resolveTop=false;
    bool resolveLeft=false;
    bool resolveRight=false;

    double backupX=posRect.x();
    double backupY=posRect.y();
    double _wallX=posRect.x();
    double _floorY=posRect.y();

    QVector<LVL_Block*> floor_blocks;
    QVector<LVL_Block*> wall_blocks;
    //QVector<LVL_Block*> blocks_to_hit;
    if(!collided_bottom.isEmpty())
    {
        for(PlayerColliders::iterator it=collided_bottom.begin(); it!=collided_bottom.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            switch(collided->type)
            {
                case PGE_Phys_Object::LVLBlock:
                {
                    LVL_Block *blk= static_cast<LVL_Block*>(collided);
                    if(!blk) continue;
                    foot_contacts_map[(intptr_t)collided]=(intptr_t)collided;
                    if(blk->slippery_surface) foot_sl_contacts_map[(intptr_t)collided]=(intptr_t)collided;
                    //if(blk->setup->bounce) blocks_to_hit.push_back(blk);
                    floor_blocks.push_back(blk);
                }
                break;
                default:break;
            }
        }
        if(isFloor(floor_blocks))
        {
            LVL_Block*nearest = nearestBlockY(floor_blocks);
            if(nearest)
            {
                _floorY = nearest->posRect.top()-posRect.height();
                resolveBottom=true;
            }
        }
        else
        {
            foot_contacts_map.clear();
            foot_sl_contacts_map.clear();
        }
    }

    if(!collided_top.isEmpty())
    {
        //blocks_to_hit.clear();
        for(PlayerColliders::iterator it=collided_top.begin(); it!=collided_top.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            //if(blk) blocks_to_hit.push_back(blk);
            if(blk) floor_blocks.push_back(blk);
        }
        if(isFloor(floor_blocks))
        {
            LVL_Block*nearest = nearestBlockY(floor_blocks);
            if(nearest)
            {
                if(!resolveBottom) _floorY = nearest->posRect.bottom()+1;
                resolveTop=true;
            }
        }
    }

    bool wall=false;
    if(!collided_left.isEmpty())
    {
        for(PlayerColliders::iterator it=collided_left.begin(); it!=collided_left.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            if(blk) wall_blocks.push_back(blk);
        }
        if(isWall(wall_blocks))
        {
            LVL_Block*nearest = nearestBlock(wall_blocks);
            if(nearest)
            {
                _wallX = nearest->posRect.right();
                resolveLeft=true;
                wall=true;
            }
        }
    }

    if(!collided_right.isEmpty())
    {
        wall_blocks.clear();
        for(PlayerColliders::iterator it=collided_right.begin(); it!=collided_right.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            if(blk) wall_blocks.push_back(blk);
        }
        if(isWall(wall_blocks))
        {
            LVL_Block*nearest = nearestBlock(wall_blocks);
            if(nearest)
            {
                _wallX = nearest->posRect.left()-posRect.width();
                resolveRight=true;
                wall=true;
            }
        }
    }

    if((resolveLeft||resolveRight) && (resolveTop||resolveBottom))
    {
        //check if on floor or in air
        bool _iswall=false;
        bool _isfloor=false;
        posRect.setX(_wallX);
        _isfloor=isFloor(floor_blocks);
        posRect.setPos(backupX, _floorY);
        _iswall=isWall(wall_blocks);
        posRect.setX(backupX);
        if(!_iswall && _isfloor)
        {
            resolveLeft=false;
            resolveRight=false;
        }
        if(!_isfloor && _iswall)
        {
            resolveTop=false;
            resolveBottom=false;
        }
    }

    if(resolveLeft || resolveRight)
    {
        posRect.setX(_wallX);
        setSpeedX(0);
    }
    if(resolveBottom || resolveTop)
    {
        posRect.setY(_floorY);
        //float bumpSpeed=speedY();
        setSpeedY(0);
        //if(!blocks_to_hit.isEmpty())
        //{
        //    LVL_Block*nearest = nearestBlock(blocks_to_hit);
        //    if(nearest)
        //    {
        //        //long npcid=nearest->data.npc_id;
        //        if(resolveBottom) nearest->hit(LVL_Block::down); else nearest->hit();
        //        //if( nearest->setup->hitable || (npcid!=0) || (nearest->destroyed) || nearest->setup->bounce )
        //        //    bump(resolveBottom,
        //        //         (resolveBottom ? physics_cur.velocity_jump_bounce : bumpSpeed),
        //        //         physics_cur.jump_time_bounce);
        //    }
        //}
        //if(resolveTop && !bumpUp && !bumpDown )
        //    jumpTime=0;
    }
    else
    {
        posRect.setY(backupY);
    }
    _stucked = ( (!collided_center.isEmpty()) && (!collided_bottom.isEmpty()) && (!wall) );

    #ifdef COLLIDE_DEBUG
    qDebug() << "=====Collision check and resolve end======";
    #endif
}


void LVL_Npc::solveCollision(PGE_Phys_Object *collided)
{
    if(!collided) return;

    switch(collided->type)
    {
        case PGE_Phys_Object::LVLBlock:
        {
            //Don't collide with blocks if disabled!
            if(!setup->collision_with_blocks) break;

            #ifdef COLLIDE_DEBUG
            qDebug() << "Player:"<<posRect.x()<<posRect.y()<<posRect.width()<<posRect.height();
            #endif
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            if(blk)
            {
                if(blk->destroyed)
                {
                    #ifdef COLLIDE_DEBUG
                    qDebug() << "Destroyed!";
                    #endif
                    break;
                }
            }
            else
            {
                #ifdef COLLIDE_DEBUG
                qDebug() << "Wrong cast";
                #endif
                break;
            }

            if( ((!forceCollideCenter)&&(!collided->posRect.collideRect(posRect)))||
                ((forceCollideCenter)&&(!collided->posRect.collideRectDeep(posRect, 1.0, -3.0))) )
            {
                #ifdef COLLIDE_DEBUG
                qDebug() << "No, is not collidng";
                #endif
                break;
            }
            #ifdef COLLIDE_DEBUG
            qDebug() << "Collided item! "<<collided->type<<" " <<collided->posRect.center().x()<<collided->posRect.center().y();
            #endif

            if((bumpUp||bumpDown)&&(!forceCollideCenter))
            {
                #ifdef COLLIDE_DEBUG
                qDebug() << "Bump? U'r dumb!";
                #endif
                break;
            }

            PGE_PointF c1 = posRect.center();
            PGE_RectF &r1 = posRect;
            PGE_PointF cc = collided->posRect.center();
            PGE_RectF  rc = collided->posRect;

            switch(collided->collide_npc)
            {
                case COLLISION_TOP:
                {
                    PGE_RectF &r1=posRect;
                    PGE_RectF  rc = collided->posRect;
                    if(
                            (
                                (speedY() >= 0.0)
                                &&
                                (r1.bottom() < rc.top()+_velocityY_prev)
                                &&
                                (
                                     (r1.left()<rc.right()-1 ) &&
                                     (r1.right()>rc.left()+1 )
                                 )
                             )
                            ||
                            (r1.bottom() <= rc.top())
                            )
                    {
                        if(blk->isHidden) break;
                        collided_bottom[(intptr_t)collided]=collided;//bottom of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Top of block";
                        #endif
                    }
                }
                break;
                case COLLISION_ANY:
                {
                    #ifdef COLLIDE_DEBUG
                    bool found=false;
                    #endif
                    double xSpeed = Maths::max(fabs(speedX()), fabs(_velocityX_prev)) * Maths::sgn(speedX());
                    double ySpeed = Maths::max(fabs(speedY()), fabs(_velocityY_prev)) * Maths::sgn(speedY());
                    //*****************************Feet of NPC****************************/
                    if(
                            (
                                (speedY() >= 0.0)
                                &&
                                (floor(r1.bottom()) < rc.top()+ySpeed)
                                &&( !( (r1.left()>=rc.right()-0.2) || (r1.right() <= rc.left()+0.2) ) )
                             )
                            ||
                            (r1.bottom() <= rc.top())
                            )
                    {
                            if(blk->isHidden) break;
                            collided_bottom[(intptr_t)collided]=collided;//bottom of player
                            #ifdef COLLIDE_DEBUG
                            qDebug() << "Top of block";
                            found=true;
                            #endif
                    }
                    //*****************************Head of NPC****************************/
                    else if( (
                                 (  ((!forceCollideCenter)&&(speedY()<0.0))||(forceCollideCenter&&(speedY()<=0.0))   )
                                 &&
                                 (r1.top() > rc.bottom()+ySpeed-1.0+_heightDelta)
                                 &&( !( (r1.left()>=rc.right()-0.5 ) || (r1.right() <= rc.left()+0.5 ) ) )
                              )
                             )
                    {
                        collided_top[(intptr_t)collided]=collided;//top of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Bottom of block";
                        found=true;
                        #endif
                    }
                    //*****************************Left****************************/
                    else if( (speedX()<0.0) && (c1.x() > cc.x()) && (r1.left() >= rc.right()+xSpeed-1.0)
                             && ( (r1.top()<rc.bottom())&&(r1.bottom()>rc.top()) ) )
                    {
                        if(blk->isHidden) break;
                        collided_left[(intptr_t)collided]=collided;//right of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Right of block";
                        #endif
                    }
                    //*****************************Right****************************/
                    else if( (speedX()>0.0) && (c1.x() < cc.x()) && ( r1.right() <= rc.left()+xSpeed+1.0)
                             && ( (r1.top()<rc.bottom())&&(r1.bottom()>rc.top()) ) )
                    {
                        if(blk->isHidden) break;
                        collided_right[(intptr_t)collided]=collided;//left of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Left of block";
                        found=true;
                        #endif
                    }


                    float c=forceCollideCenter? 0.0f : 1.0f;
                    //*****************************Center****************************/
                    #ifdef COLLIDE_DEBUG
                    qDebug() << "block" <<posRect.top()<<":"<<blk->posRect.bottom()
                             << "block" <<posRect.bottom()<<":"<<blk->posRect.top()<<" collide?"<<
                                blk->posRect.collideRectDeep(posRect,
                                                                                     fabs(_velocityX_prev)*c+c*2.0,
                                                                                     fabs(_velocityY_prev)*c+c*2.0) <<
                                "depths: "<< fabs(_velocityX_prev)*c+c*2.0 <<
                            fabs(_velocityY_prev)*c+c;
                    #endif
                    if( blk->posRect.collideRectDeep(posRect,
                                                     fabs(_velocityX_prev)*c+c*2.0,
                                                     fabs(_velocityY_prev)*c+c*2.0)
                            )
                    {
                        if(blk->isHidden && !forceCollideCenter) break;
                        collided_center[(intptr_t)collided]=collided;
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Center of block";
                        found=true;
                        #endif
                    }

                    #ifdef COLLIDE_DEBUG
                    qDebug() << "---checked---" << (found?"and found!": "but nothing..." )<<
                                r1.left()<< "<="<< rc.right()<<"+"<<xSpeed ;
                    #endif
                    break;
                }
            default: break;
            }
            break;
        }
        case PGE_Phys_Object::LVLBGO:
        {
            LVL_Bgo *bgo= static_cast<LVL_Bgo*>(collided);
            if(bgo)
            {
                contacted_bgos[(intptr_t)collided]=collided;
            }
            break;
        }
        case PGE_Phys_Object::LVLNPC:
        {
            LVL_Npc *npc= static_cast<LVL_Npc*>(collided);
            if(npc)
            {
                if(npc->killed)        break;
                if(npc->data.friendly) break;

                if(!LvlSceneP::s->isExit())
                {
                    if(npc->data.id==11){
                        LvlSceneP::s->setExiting(4500, 1);
                    }
                    else
                    if((npc->data.id==15)||(npc->data.id==16)){
                        LvlSceneP::s->setExiting(7000, 2);
                    }
                    else
                    if((npc->data.id==39)||(npc->data.id==41)){
                        LvlSceneP::s->setExiting(3200, 5);
                    }
                    else
                    if(npc->data.id==97){
                        LvlSceneP::s->setExiting(4500, 7);
                    }
                    else
                    if(npc->data.id==197){
                        LvlSceneP::s->setExiting(15000, 8);
                    }
                    else
                    if(npc->data.id==86 )
                    {
                        LvlSceneP::s->setExiting(5000, 7);
                    }
                }
//                if((!npc->data.friendly)&&(npc->setup->takable))
//                {
//                    kill_npc(npc, LVL_Player::NPC_Taked_Coin);
//                    npc->kill();
//                }
            }
            break;
        }
        case PGE_Phys_Object::LVLPhysEnv:
        {
            LVL_PhysEnv *env= static_cast<LVL_PhysEnv*>(collided);
            if(env)
            {
                if(env) environments_map[(intptr_t)env]=env->env_type;
            }
            break;
        }
    default: break;
    }
}

bool LVL_Npc::onGround()
{
    return _onGround;
}
