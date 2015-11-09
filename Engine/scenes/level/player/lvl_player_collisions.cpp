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

#include "../lvl_player.h"
#include "../lvl_bgo.h"
#include "../lvl_block.h"
#include "../lvl_npc.h"
#include "../lvl_physenv.h"
#include "../lvl_warp.h"

#include "../lvl_scene_ptr.h"

#include <audio/pge_audio.h>
#include <common_features/maths.h>

#include "../collision_checks.h"

//#define COLLIDE_DEBUG

void LVL_Player::updateCollisions()
{
    foot_contacts_map.clear();
    _onGround=false;
    foot_sl_contacts_map.clear();
    contactedWarp = NULL;
    contactedWithWarp=false;
    climbable_map.clear();
    environments_map.clear();

    collided_top.clear();
    collided_left.clear();
    collided_right.clear();
    collided_bottom.clear();
    collided_center.clear();

    collided_talkable_npc = NULL;

    collided_slope=false;
    collided_slope_angle_ratio=0.0f;
    collided_slope_celling=false;
    collided_slope_angle_ratio_celling=0.0f;

    _velocityX_add=0;
    _velocityY_add=0;

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

    double _floorX_vel=0.0;//velocities sum
    double _floorX_num=0.0;//num of velocities
    double _floorY_vel=0.0;//velocities sum
    double _floorY_num=0.0;//num of velocities

    QVector<PGE_Phys_Object*> floor_blocks;
    QVector<PGE_Phys_Object*> wall_blocks;
    QVector<PGE_Phys_Object*> blocks_to_hit;
    //QVector<PGE_Phys_Object*> add_speed_to;

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
                    foot_contacts_map[(intptr_t)collided]=collided;
                    if(blk->slippery_surface) foot_sl_contacts_map[(intptr_t)collided]=collided;
                    if(blk->setup->bounce) blocks_to_hit.push_back(blk);
                    floor_blocks.push_back(blk);
                    _floorY_vel+=blk->speedYsum();
                    _floorY_num+=1.0;
                    _floorX_vel+=blk->speedXsum();
                    _floorX_num+=1.0;
                } break;
                case PGE_Phys_Object::LVLNPC:
                {
                    LVL_Npc *npc= static_cast<LVL_Npc*>(collided);
                    if(!npc) continue;
                    foot_contacts_map[(intptr_t)collided]=collided;
                    if(npc->slippery_surface) foot_sl_contacts_map[(intptr_t)collided]=collided;
                    floor_blocks.push_back(npc);
                    _floorY_vel+=npc->speedYsum();
                    _floorY_num+=1.0;
                    _floorX_vel+=npc->speedXsum();
                    _floorX_num+=1.0;
                }
                break;
                default:break;
            }
        }
        if(_floorX_num!=0.0) _floorX_vel=_floorX_vel/_floorX_num;
        if(_floorY_num!=0.0) _floorY_vel=_floorY_vel/_floorY_num;
        if(!foot_contacts_map.isEmpty())
        {
            _velocityX_add=_floorX_vel;
            _velocityY_add=_floorY_vel;
        }

        if(isFloor(floor_blocks))
        {
            PGE_Phys_Object*nearest = nearestBlockY(floor_blocks);
            if(nearest)
            {
                LVL_Block *blk= static_cast<LVL_Block*>(nearest);
                if(blk && (blk->shape!=LVL_Block::shape_rect))
                {
                    if(blk->shape==LVL_Block::shape_tr_top_right)
                    {
                        collided_slope=true; collided_slope_angle_ratio=blk->shape_slope_angle_ratio;
                        _floorY = nearest->posRect.bottom()-SL_HeightTopRight(*this, nearest);
                        if(_floorY<nearest->top()) _floorY=nearest->posRect.top();
                        else if(_floorY>nearest->bottom()) _floorY=nearest->posRect.bottom();
                    }
                    else
                    if(blk->shape==LVL_Block::shape_tr_top_left)
                    {
                        collided_slope=true; collided_slope_angle_ratio=blk->shape_slope_angle_ratio;
                        _floorY = nearest->posRect.bottom()-SL_HeightTopLeft(*this, nearest);
                        if(_floorY<nearest->top()) _floorY=nearest->posRect.top();
                        else if(_floorY>nearest->bottom()) _floorY=nearest->posRect.bottom();
                    }
                    else
                        _floorY = nearest->posRect.top();
                    _floorY-=posRect.height();
                } else {
                    _floorY = nearest->posRect.top()-posRect.height();
                }
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
        blocks_to_hit.clear();
        for(PlayerColliders::iterator it=collided_top.begin(); it!=collided_top.end() ; it++)
        {
            PGE_Phys_Object *body= *it;
            if(body) blocks_to_hit.push_back(body);
            if(body) floor_blocks.push_back(body);
        }
        if(isFloor(floor_blocks))
        {
            PGE_Phys_Object*nearest = nearestBlockY(blocks_to_hit);
            if(nearest)
            {
                if(!resolveBottom)
                {
                    LVL_Block *blk= static_cast<LVL_Block*>(nearest);
                    if(blk && (blk->shape!=LVL_Block::shape_rect))
                    {
                        if(blk->shape==LVL_Block::shape_tr_bottom_right)
                        {
                            _floorY = nearest->posRect.top()+SL_HeightTopLeft(*this, nearest);
                            if(_floorY<nearest->top()) _floorY=nearest->posRect.top();
                            else if(_floorY>nearest->bottom()) _floorY=nearest->posRect.bottom();
                        }
                        else
                        if(blk->shape==LVL_Block::shape_tr_bottom_left)
                        {
                            _floorY = nearest->posRect.top()+SL_HeightTopRight(*this, nearest);
                            if(_floorY<nearest->top()) _floorY=nearest->posRect.top();
                            else if(_floorY>nearest->bottom()) _floorY=nearest->posRect.bottom();
                        }
                    }
                    else
                        _floorY = nearest->posRect.bottom();
                    _floorY+=1;
                }
                resolveTop=true;
            }
        }
    }

    bool wall=false;
    if(!collided_left.isEmpty())
    {
        for(PlayerColliders::iterator it=collided_left.begin(); it!=collided_left.end() ; it++)
        {
            PGE_Phys_Object *body= *it;
            if(body) wall_blocks.push_back(body);
        }
        if(isWall(wall_blocks))
        {
            PGE_Phys_Object*nearest = nearestBlock(wall_blocks);
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
            PGE_Phys_Object *body= *it;
            if(body) wall_blocks.push_back(body);
        }
        if(isWall(wall_blocks))
        {
            PGE_Phys_Object*nearest = nearestBlock(wall_blocks);
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
        _velocityX_add=0;
    }
    if(resolveBottom || resolveTop)
    {
        posRect.setY(_floorY);
        float bumpSpeed=speedY();
        setSpeedY(_floorY_vel);
        _velocityY_add=0;
        if(!blocks_to_hit.isEmpty())
        {
            PGE_Phys_Object* nearestObj=nearestBlock(blocks_to_hit);
            if(nearestObj && (nearestObj->type==PGE_Phys_Object::LVLBlock))
            {
                LVL_Block*nearest = static_cast<LVL_Block*>(nearestObj);
                long npcid=nearest->data.npc_id;
                if(resolveBottom) nearest->hit(LVL_Block::down); else nearest->hit();
                if( nearest->setup->hitable || (npcid!=0) || (nearest->destroyed) || nearest->setup->bounce )
                    bump(resolveBottom,
                         (resolveBottom ? physics_cur.velocity_jump_bounce : bumpSpeed),
                         physics_cur.jump_time_bounce);
            }
            else
            {
                PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
            }
        }
        if(resolveTop && !bumpUp && !bumpDown )
            jumpTime=0;
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



void LVL_Player::_collideUnduck()
{
    _syncPosition();
    if(isWarping) return;

    #ifdef COLLIDE_DEBUG
    qDebug() << "do unduck!";
    #endif

    //Detect collidable blocks!
    QVector<PGE_Phys_Object*> bodies;
    PGE_RectF posRectC = posRect;
    posRectC.setTop(posRectC.top()-4.0f);
    posRectC.setLeft(posRectC.left()+1.0f);
    posRectC.setRight(posRectC.right()-1.0f);
    posRectC.setBottom(posRectC.bottom()+ (ducking ? 0.0:(-posRect.height()/2.0))+4.0 );
    LvlSceneP::s->queryItems(posRectC, &bodies);

    forceCollideCenter=true;
    for(PGE_RenderList::iterator it=bodies.begin();it!=bodies.end(); it++ )
    {
        PGE_Phys_Object*body=*it;
        if(body==this) continue;
        if(body->isPaused()) continue;
        if(!body->isVisible()) continue;
        solveCollision(body);
    }
    forceCollideCenter=false;

    #ifdef COLLIDE_DEBUG
    int hited=0;
    PGE_RectF nearestRect;
    #endif

    bool resolveTop=false;
    double _floorY=0;
    QVector<PGE_Phys_Object*> blocks_to_hit;

    if((!collided_center.isEmpty())&&(collided_bottom.isEmpty()))
    {
        for(PlayerColliders::iterator it=collided_center.begin(); it!=collided_center.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            if(blk) blocks_to_hit.push_back(blk);
        }
        for(PlayerColliders::iterator it=collided_top.begin(); it!=collided_top.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            LVL_Block *blk= static_cast<LVL_Block*>(collided);
            if(blk) blocks_to_hit.push_back(blk);
        }
        if(isFloor(blocks_to_hit))
        {
            PGE_Phys_Object*nearest = nearestBlockY(blocks_to_hit);
            if(nearest)
            {
                _floorY=nearest->posRect.bottom()+1.0;
                resolveTop=true;
            }
        }
    }

    if(resolveTop)
    {
        posRect.setY(_floorY);
        if(!blocks_to_hit.isEmpty())
        {
            PGE_Phys_Object*nearest_obj=nearestBlock(blocks_to_hit);
            if(nearest_obj && (nearest_obj->type==PGE_Phys_Object::LVLBlock))
            {
                LVL_Block*nearest = static_cast<LVL_Block*>(nearest_obj);
                resolveTop=true;
                long npcid=nearest->data.npc_id;
                nearest->hit();
                if( nearest->setup->hitable || (npcid!=0) || (nearest->destroyed) || (nearest->setup->bounce) )
                {
                    bump(false, speedY());
                }
            }
            else
            {
                PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
            }
        }
        setSpeedY(0.0);
        _syncPosition();
    }

    _heightDelta = 0.0;
    #ifdef COLLIDE_DEBUG
    qDebug() << "unduck: blocks to hit"<< hited << " Nearest: "<<nearestRect.top() << nearestRect.bottom() << "are bottom empty: " << collided_bottom.isEmpty() << "bodies found: "<<bodies.size() << "bodies at center: "<<collided_center.size();
    #endif
}



void LVL_Player::solveCollision(PGE_Phys_Object *collided)
{
    if(!collided) return;

    switch(collided->type)
    {
        case PGE_Phys_Object::LVLBlock:
        {
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

//            PGE_PointF c1 = posRect.center();
//            PGE_RectF &r1 = posRect;
//            PGE_PointF cc = collided->posRect.center();
//            PGE_RectF  rc = collided->posRect;

            switch(collided->collide_player)
            {
                case COLLISION_TOP:
                {
                    if(isCollideFloorToponly(*this, collided))
                    {
                        if(blk->isHidden) break;
                        collided_bottom[(intptr_t)collided]=collided;//bottom of player
                        if(blk->setup->lava) kill(DEAD_burn);
                        else if(blk->setup->danger==2||blk->setup->danger==-3||blk->setup->danger==4) harm(1);
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
                    //*****************************Feet of player****************************/
                    if(
                        (( (blk->shape==LVL_Block::shape_rect)||
                           (blk->shape==LVL_Block::shape_tr_bottom_left)||
                           (blk->shape==LVL_Block::shape_tr_bottom_right) ) && isCollideFloor(*this, collided))||
                        ((blk->shape==LVL_Block::shape_tr_top_right)&&isCollideSlopeFloor(*this, collided, SLOPE_RIGHT)) ||
                        ((blk->shape==LVL_Block::shape_tr_top_left)&&isCollideSlopeFloor(*this, collided, SLOPE_LEFT))

                      ){
                        if(blk->isHidden) break;
                        collided_bottom[(intptr_t)collided]=collided;//bottom of player
                        if(blk->setup->lava) kill(DEAD_burn);
                        else if(blk->setup->danger==2||blk->setup->danger==-3||blk->setup->danger==4) harm(1);
                    }
                    //*****************************Head of player****************************/
                    else if(
                            (( (blk->shape==LVL_Block::shape_rect)||
                                (blk->shape==LVL_Block::shape_tr_top_left)||
                                (blk->shape==LVL_Block::shape_tr_top_right)) &&
                                isCollideCelling(*this, collided, _heightDelta, forceCollideCenter))||
                            ((blk->shape==LVL_Block::shape_tr_bottom_right)&&isCollideSlopeCelling(*this, collided, SLOPE_RIGHT)) ||
                            ((blk->shape==LVL_Block::shape_tr_bottom_left)&&isCollideSlopeCelling(*this, collided, SLOPE_LEFT))
                           )
                    {
                        collided_top[(intptr_t)collided]=collided;//top of player
                        if(blk->setup->lava) kill(DEAD_burn);
                        else if(blk->setup->danger==-2||blk->setup->danger==-3||blk->setup->danger==4) harm(1);
                    }
                    //*****************************Left****************************/
                    else if( (isCollideLeft(*this, collided)&&(blk->shape==LVL_Block::shape_rect))||
                             (isCollideLeft(*this, collided)&&(blk->shape==LVL_Block::shape_tr_top_left)
                              &&(posRect.bottom()>=(collided->posRect.top()+SL_HeightTopRight(*this, collided)+1.0)))||
                             (isCollideLeft(*this, collided)&&(blk->shape==LVL_Block::shape_tr_bottom_left)
                              &&(posRect.top()<=(collided->posRect.bottom()-SL_HeightTopRight(*this, collided)-1.0))) )
                    {
                        if(blk->isHidden) break;
                        collided_left[(intptr_t)collided]=collided;//right of player
                        if(blk->setup->lava) kill(DEAD_burn);
                        else if(blk->setup->danger==-1||blk->setup->danger==3||blk->setup->danger==4) harm(1);
                    }
                    //*****************************Right****************************/
                    else if( (isCollideRight(*this, collided)&&(blk->shape==LVL_Block::shape_rect))||
                             (isCollideRight(*this, collided)&&(blk->shape==LVL_Block::shape_tr_top_right)
                             &&(posRect.bottom()>=(collided->posRect.top()+SL_HeightTopLeft(*this, collided)+1.0)))||
                             (isCollideRight(*this, collided)&&(blk->shape==LVL_Block::shape_tr_bottom_right)
                             &&(posRect.top()<=(collided->posRect.bottom()-SL_HeightTopLeft(*this, collided)-1.0)))
                           )
                    {
                        if(blk->isHidden) break;
                        collided_right[(intptr_t)collided]=collided;//left of player
                        if(blk->setup->lava) kill(DEAD_burn);
                        else if(blk->setup->danger==1||blk->setup->danger==3||blk->setup->danger==4) harm(1);
                    }


                    float c=forceCollideCenter? 0.0f : 1.0f;
                    //*****************************Center****************************/
                    if( ((!forceCollideCenter && blk->shape==LVL_Block::shape_rect)||(forceCollideCenter))
                            && blk->posRect.collideRectDeep(posRect,
                                                     fabs(_velocityX_prev+_velocityX_add)*c+c*2.0,
                                                     fabs(_velocityY_prev+_velocityY_add)*c+c*2.0)
                            )
                    {
                        if(blk->isHidden && !forceCollideCenter) break;
                        collided_center[(intptr_t)collided]=collided;
                    }
                    break;
                }
            default: break;
            }
            break;
        }
        case PGE_Phys_Object::LVLWarp:
        {
            contactedWarp = static_cast<LVL_Warp*>(collided);
            if(contactedWarp)
                contactedWithWarp=true;
            break;
        }
        case PGE_Phys_Object::LVLBGO:
        {
            LVL_Bgo *bgo= static_cast<LVL_Bgo*>(collided);
            if(bgo)
            {
                if(bgo->setup->climbing)
                {
                    bool set=climbable_map.isEmpty();
                    climbable_map[(intptr_t)collided]=collided;
                    if(set)
                        climbableHeight=collided->posRect.top();
                    else if(collided->top()<climbableHeight)
                        climbableHeight=collided->top();
                }
            }
            break;
        }
        case PGE_Phys_Object::LVLNPC:
        {
            LVL_Npc *npc= static_cast<LVL_Npc*>(collided);
            if(npc)
            {
                if(npc->isKilled())        break;

                if(!npc->data.msg.isEmpty())
                {
                    collided_talkable_npc=npc;
                }
                if(npc->data.friendly) break;
                if(npc->isGenerator) break;
                if(npc->setup->climbable)
                {
                    bool set=climbable_map.isEmpty();
                    climbable_map[(intptr_t)collided]=collided;
                    if(set)
                        climbableHeight=collided->posRect.top();
                    else if(collided->top()<climbableHeight)
                        climbableHeight=collided->top();
                }

                if((!npc->data.friendly)&&(npc->setup->takable))
                {
                    collided_talkable_npc=NULL;
                    npc->doHarm(LVL_Npc::DAMAGE_TAKEN);
                    kill_npc(npc, LVL_Player::NPC_Taked_Coin);
                }


                if( ((!forceCollideCenter)&&(!collided->posRect.collideRect(posRect)))||
                    ((forceCollideCenter)&&(!collided->posRect.collideRectDeep(posRect, 1.0, -3.0))) )
                {
                    break;
                }

                if((bumpUp||bumpDown)&&(!forceCollideCenter))
                    break;

                if(!npc->isActivated) break;

                //PGE_PointF c1 = posRect.center();
                //PGE_RectF &r1 = posRect;
                //PGE_PointF cc = collided->posRect.center();
                //PGE_RectF  rc = collided->posRect;

                switch(collided->collide_player)
                {
                    case COLLISION_TOP:
                    {
                        if(isCollideFloorToponly(*this, collided))
                        {
                            collided_bottom[(intptr_t)collided]=collided;//bottom of player
                            #ifdef COLLIDE_DEBUG
                            qDebug() << "Top of block";
                            #endif
                        } else {
                            if( npc->posRect.collideRect(posRect))
                            {
                               if(npc->setup->hurt_player & !npc->setup->kill_on_jump) harm(1);
                            }
                        }

                    }
                    break;
                    case COLLISION_ANY:
                    {
                        #ifdef COLLIDE_DEBUG
                        bool found=false;
                        #endif
                        //double xSpeed = Maths::max(fabs(speedX()+_velocityX_add), fabs(_velocityX_prev+_velocityX_add)) * Maths::sgn(speedX()+_velocityX_add);
                        //double ySpeed = Maths::max(fabs(speedY()+_velocityY_add), fabs(_velocityY_prev+_velocityY_add)) * Maths::sgn(speedY()+_velocityY_add);
                        //*****************************Feet of player****************************/
                        if(isCollideFloor(*this, collided))
                        {
                                collided_bottom[(intptr_t)collided]=collided;//bottom of player
                                #ifdef COLLIDE_DEBUG
                                qDebug() << "Top of block";
                                found=true;
                                #endif
                        }
                        //*****************************Head of player****************************/
                        else if( isCollideCelling(*this, collided, _heightDelta, forceCollideCenter) )
                        {
                            collided_top[(intptr_t)collided]=collided;//top of player
                            if(npc->setup->hurt_player) harm(1);
                            #ifdef COLLIDE_DEBUG
                            qDebug() << "Bottom of block";
                            found=true;
                            #endif
                        }
                        //*****************************Left****************************/
                        else if( isCollideLeft(*this, collided) )
                        {
                            collided_left[(intptr_t)collided]=collided;//right of player
                            if(npc->setup->hurt_player) harm(1);
                            #ifdef COLLIDE_DEBUG
                            qDebug() << "Right of block";
                            #endif
                        }
                        //*****************************Right****************************/
                        else if( isCollideRight(*this, collided) )
                        {
                            collided_right[(intptr_t)collided]=collided;//left of player
                            if(npc->setup->hurt_player) harm(1);
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
                        if( npc->posRect.collideRectDeep(posRect,
                                                         fabs(_velocityX_prev)*c+c*2.0,
                                                         fabs(_velocityY_prev)*c+c*2.0)
                                )
                        {
                            if(npc->setup->hurt_player) harm(1);
                            if(!forceCollideCenter) break;
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
                case COLLISION_NONE:
                    { //Detect top of stompable NPC!
                        if(!npc->setup->kill_on_jump)
                        {
                            if( npc->posRect.collideRect(posRect))
                            {
                               if(npc->setup->hurt_player) harm(1);
                            }
                            break;
                        }

//                        PGE_RectF &r1=posRect;
//                        PGE_RectF  rc = collided->posRect;
//                        float summSpeedY=(speedY()+_velocityY_add)-(collided->speedY()+collided->_velocityY_add);
//                        float summSpeedYprv=_velocityY_prev-collided->_velocityY_prev;
                        if(isCollideFloorToponly(*this, collided))
//                                (
//                                    (summSpeedY >= 0.0)
//                                    &&
//                                    (r1.bottom() < rc.top()+summSpeedYprv)
//                                    &&
//                                    (
//                                         (r1.left()<rc.right()-1 ) &&
//                                         (r1.right()>rc.left()+1 )
//                                     )
//                                 )
//                                ||
//                                (r1.bottom() <= rc.top())
//                                )
                        {
                            npc->doHarm(LVL_Npc::DAMAGE_STOMPED);
                            this->bump(true);
                            //Reset floating state
                            floating_timer = floating_maxtime;
                            if(floating_isworks)
                            {
                                floating_isworks=false;
                                setGravityScale(climbing?0:physics_cur.gravity_scale);
                            }
                            kill_npc(npc, NPC_Stomped);
                        } else {
                            if( npc->posRect.collideRect(posRect))
                            {
                                //The second half of this "if" statement is there to fix a bug with falling collisions,
                                //and should NOT remain here because it is not how we should handle this!
                                //The bug occurs when the player is moving fast enough when they land on an enemy that they
                                //partially enter the enemy before the collision is resolved, causing the engine to think
                                //that the player is INSIDE the NPC, when they are actually only JUMPING on the NPC.
                                if(npc->setup->hurt_player & (!npc->setup->kill_on_jump || colliding_ySpeed > -1.0f)) harm(1);
                            }
                        }
                        break;
                    }//case COLLISION_NONE
                default: break;
                }
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

