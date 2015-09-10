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

#include "../lvl_npc.h"
#include "../lvl_player.h"
#include "../lvl_block.h"
#include "../lvl_physenv.h"
#include "../lvl_bgo.h"
#include <data_configs/config_manager.h>
#include <common_features/number_limiter.h>
#include <common_features/maths.h>


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
    contacted_npc.clear();
    contacted_players.clear();

    collided_top.clear();
    collided_left.clear();
    collided_right.clear();
    collided_bottom.clear();
    collided_center.clear();
    cliffDetected=false;

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

    QVector<PGE_Phys_Object*> topbottom_blocks;
    QVector<PGE_Phys_Object*> floor_blocks;
    QVector<PGE_Phys_Object*> wall_blocks;
    QVector<PGE_Phys_Object*> add_speed_to;

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
                    _floorY_vel+=blk->speedYsum();
                    _floorY_num+=1.0;
                    _floorX_vel+=blk->speedXsum();
                    _floorX_num+=1.0;
                }
                case PGE_Phys_Object::LVLNPC:
                {
                    LVL_Npc *npc= static_cast<LVL_Npc*>(collided);
                    if(!npc) continue;
                    foot_contacts_map[(intptr_t)collided]=(intptr_t)collided;
                    if(npc->slippery_surface) foot_sl_contacts_map[(intptr_t)collided]=(intptr_t)collided;
                    //if(blk->setup->bounce) blocks_to_hit.push_back(blk);
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

        if(isFloor(floor_blocks, &cliffDetected))
        {
            PGE_Phys_Object*nearest = nearestBlockY(floor_blocks);
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
            if(!collided) continue;
            switch(collided->type)
            {
            case PGE_Phys_Object::LVLPlayer:
                {
                    if((collide_player==COLLISION_ANY)||(collide_player==COLLISION_TOP))
                        add_speed_to.push_back(collided);
                    continue;
                }
            break;
            case PGE_Phys_Object::LVLNPC:
                {
                    LVL_Npc *npc= static_cast<LVL_Npc*>(collided);
                    if(!npc) continue;
                    if(!npc->is_scenery && !npc->disableBlockCollision &&
                            ((collide_npc==COLLISION_ANY)||(collide_npc==COLLISION_TOP))
                            )
                    {
                        add_speed_to.push_back(collided);
                        continue;
                    }
                }
            break;
            default:break;
            }
            if(collided) topbottom_blocks.push_back(collided);
        }
        if(isFloor(topbottom_blocks))
        {
            PGE_Phys_Object*nearest = nearestBlockY(floor_blocks);
            if(nearest)
            {
                if(!resolveBottom) _floorY = nearest->posRect.bottom()+1;
                resolveTop=true;
            }
        }

        foreach(PGE_Phys_Object* x, floor_blocks)
            topbottom_blocks.push_back(x);
    }

    bool wall=false;
    if(!collided_left.isEmpty())
    {
        for(PlayerColliders::iterator it=collided_left.begin(); it!=collided_left.end() ; it++)
        {
            PGE_Phys_Object *collided= *it;
            if(collided) wall_blocks.push_back(collided);
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
            PGE_Phys_Object *collided= *it;
            if(collided) wall_blocks.push_back(collided);
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
        _isfloor=isFloor(floor_blocks, &cliffDetected);
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
        //float bumpSpeed=speedY();
        if(resolveTop)
            setSpeedY(0.0);
        else
            setSpeedY(_floorY_vel);
        _velocityY_add=0;
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

    for(int i=0;i<add_speed_to.size();i++)
    {
        if(add_speed_to[i]->_velocityX_add!=0.0f)
            add_speed_to[i]->setSpeedY(speedYsum());
        else
            add_speed_to[i]->setSpeed(add_speed_to[i]->speedX()+speedXsum(), speedYsum());
    }

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
            if(disableBlockCollision) break;

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

//            PGE_PointF c1 = posRect.center();
//            PGE_RectF &r1 = posRect;
//            PGE_PointF cc = collided->posRect.center();
//            PGE_RectF  rc = collided->posRect;

            switch(collided->collide_npc)
            {
                case COLLISION_TOP:
                {
//                    PGE_RectF &r1=posRect;
//                    PGE_RectF  rc = collided->posRect;
//                    float summSpeedY=(speedY()+_velocityY_add)-(collided->speedY()+collided->_velocityY_add);
//                    float summSpeedYprv=_velocityY_prev-collided->_velocityY_prev;
                    if(isCollideFloorToponly(collided))
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
//                    double xSpeed = Maths::max(fabs(speedXsum()), fabs(_velocityX_prev)) * Maths::sgn(speedXsum());
//                    double ySpeed = Maths::max(fabs(speedYsum()), fabs(_velocityY_prev)) * Maths::sgn(speedYsum());
//                    double xSpeedO = Maths::max(fabs(collided->speedXsum()), fabs(collided->_velocityX_prev)) * Maths::sgn(collided->speedXsum());
//                    double ySpeedO = Maths::max(fabs(collided->speedYsum()), fabs(collided->_velocityY_prev)) * Maths::sgn(collided->speedYsum());
                    //*****************************Feet of NPC****************************/
                    if(
//                            (
//                                /*(speedY() >= 0.0)
//                                &&*/
//                                (floor(r1.bottom()) < rc.top()+ySpeed+ySpeedO)
//                                &&( !( (r1.left()>=rc.right()-0.2) || (r1.right() <= rc.left()+0.2) ) )
//                             )
//                            ||
//                            (r1.bottom() <= rc.top())
                            isCollideFloor(collided)
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
                    else if( /*(
                                 (  ((!forceCollideCenter)&&(-fabs(speedYsum())<0.0))||(forceCollideCenter&&(-fabs(speedYsum())<=0.0))   )
                                 &&
                                 (r1.top() > rc.bottom()+ySpeed+ySpeedO-1.0+_heightDelta)
                                 &&( !( (r1.left()>=rc.right()-0.5 ) || (r1.right() <= rc.left()+0.5 ) ) )
                              )*/
                             isCollideCelling(collided, _heightDelta, forceCollideCenter)
                             )
                    {
                        collided_top[(intptr_t)collided]=collided;//top of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Bottom of block";
                        found=true;
                        #endif
                    }
                    //*****************************Left****************************/
                    else if( isCollideLeft(collided) )
                    {
                        if(blk->isHidden) break;
                        collided_left[(intptr_t)collided]=collided;//right of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Right of block";
                        #endif
                    }
                    //*****************************Right****************************/
//                    else if( (speedX()>0.0) && (c1.x() < cc.x()) && ( r1.right() <= rc.left()+xSpeed+xSpeedO+1.0)
//                             && ( (r1.top()<rc.bottom())&&(r1.bottom()>rc.top()) ) )
                    else if( isCollideRight(collided) )
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
                if(npc->isGenerator) break;
            }

            if( ((!forceCollideCenter)&&(!collided->posRect.collideRect(posRect)))||
                ((forceCollideCenter)&&(!collided->posRect.collideRectDeep(posRect, 1.0, -3.0))) )
            {
                break;
            }

            if(!npc->isActivated) break;

            contacted_npc[(intptr_t)collided]=collided;

            if(isGenerator) break;
            if(disableNpcCollision) break;
//            PGE_PointF c1 = posRect.center();
//            PGE_RectF &r1 = posRect;
//            PGE_PointF cc = collided->posRect.center();
//            PGE_RectF  rc = collided->posRect;

            if(disableBlockCollision) break;

            switch(collided->collide_npc)
            {
                case COLLISION_TOP:
                {
//                    PGE_RectF &r1=posRect;
//                    PGE_RectF  rc = collided->posRect;
                    if(isCollideFloorToponly(collided))
//                            (
//                                (speedY() >= 0.0)
//                                &&
//                                (r1.bottom() < rc.top()+_velocityY_prev+collided->_velocityY_prev)
//                                &&
//                                (
//                                     (r1.left()<rc.right()-1 ) &&
//                                     (r1.right()>rc.left()+1 )
//                                 )
//                             )
//                            ||
//                            (r1.bottom() <= rc.top())
//                            )
                    {
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
//                    double xSpeed = Maths::max(fabs(speedXsum()), fabs(_velocityX_prev)) * Maths::sgn(speedXsum());
//                    double ySpeed = Maths::max(fabs(speedYsum()), fabs(_velocityY_prev)) * Maths::sgn(speedYsum());
//                    double xSpeedO = Maths::max(fabs(collided->speedXsum()), fabs(collided->_velocityX_prev)) * Maths::sgn(collided->speedXsum());
//                    double ySpeedO = Maths::max(fabs(collided->speedYsum()), fabs(collided->_velocityY_prev)) * Maths::sgn(collided->speedYsum());
                    //*****************************Feet of NPC****************************/
                    if(isCollideFloor(collided))
//                            (
//                                (fabs(speedYsum()) >= 0.0)
//                                &&
//                                (floor(r1.bottom()) < rc.top()+ySpeed+ySpeedO)
//                                &&( !( (r1.left()>=rc.right()-0.2) || (r1.right() <= rc.left()+0.2) ) )
//                             )
//                            ||
//                            (r1.bottom() <= rc.top())
                    {
                            collided_bottom[(intptr_t)collided]=collided;//bottom of player
                            #ifdef COLLIDE_DEBUG
                            qDebug() << "Top of block";
                            found=true;
                            #endif
                    }
                    //*****************************Head of NPC****************************/
                    else if(isCollideCelling(collided, _heightDelta, forceCollideCenter))
//                    else if( (
//                                 (  ((!forceCollideCenter)&&(-fabs(speedYsum())<0.0))||(forceCollideCenter&&(-fabs(speedYsum())<=0.0))   )
//                                 &&
//                                 (r1.top() > rc.bottom()+ySpeed-1.0+ySpeedO+_heightDelta)
//                                 &&( !( (r1.left()>=rc.right()-0.5 ) || (r1.right() <= rc.left()+0.5 ) ) )
//                              )
//                             )
                    {
                        collided_top[(intptr_t)collided]=collided;//top of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Bottom of block";
                        found=true;
                        #endif
                    }
                    //*****************************Left****************************/
                    else if( /*(speedXsum()<0.0) && (c1.x() > cc.x()) && (r1.left() >= rc.right()+xSpeed+xSpeedO-1.0)
                             && ( (r1.top()<rc.bottom())&&(r1.bottom()>rc.top()) )*/ isCollideLeft(collided) )
                    {
                        collided_left[(intptr_t)collided]=collided;//right of player
                        #ifdef COLLIDE_DEBUG
                        qDebug() << "Right of block";
                        #endif
                    }
                    //*****************************Right****************************/
                    else if( /*(speedX()>0.0) && (c1.x() < cc.x()) && ( r1.right() <= rc.left()+xSpeed+xSpeedO+1.0)
                             && ( (r1.top()<rc.bottom())&&(r1.bottom()>rc.top()) )*/ isCollideRight(collided) )
                    {
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
                    if( npc->posRect.collideRectDeep(posRect,
                                                     fabs(_velocityX_prev)*c+c*2.0,
                                                     fabs(_velocityY_prev)*c+c*2.0)
                            )
                    {
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
            default: break;
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
    case PGE_Phys_Object::LVLPlayer:
        {
            contacted_players[(intptr_t)collided]=collided;

            //PGE_PointF c1 = posRect.center();
            PGE_RectF &r1 = posRect;
            //PGE_PointF cc = collided->posRect.center();
            PGE_RectF  rc = collided->posRect;

            //double xSpeed = Maths::max(fabs(speedX()), fabs(_velocityX_prev)) * Maths::sgn(speedX());
            double ySpeed = Maths::max(fabs(speedY()), fabs(_velocityY_prev)) * Maths::sgn(speedY());
            //double xSpeedO = Maths::max(fabs(collided->speedX()), fabs(collided->_velocityX_prev)) * Maths::sgn(collided->speedX());
            double ySpeedO = Maths::max(fabs(collided->speedY()), fabs(collided->_velocityY_prev)) * Maths::sgn(collided->speedY());
            if( (
                         (  ((!forceCollideCenter)&&(speedY()<0.0))||(forceCollideCenter&&(speedY()<=0.0))   )
                         &&
                         (r1.top() > rc.bottom()+ySpeed-1.0+ySpeedO+_heightDelta)
                         &&( !( (r1.left()>=rc.right()-0.5 ) || (r1.right() <= rc.left()+0.5 ) ) )
                      )
                     )
            {
                collided_top[(intptr_t)collided]=collided;//top of player
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
