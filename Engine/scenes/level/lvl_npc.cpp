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

void LVL_Npc::kill()
{
    killed=true;
    sct()->unregisterElement(this);
    LvlSceneP::s->dead_npcs.push_back(this);
    LvlSceneP::s->launchStaticEffectC(setup->effect_1, posCenterX(), posCenterY(), 1, 0, 0, 0, 0, direction);
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

    int imgOffsetX = (int)round( - ( ( (double)setup->gfx_w - (double)setup->width ) / 2 ) );
    int imgOffsetY = (int)round( - (double)setup->gfx_h + (double)setup->height + (double)setup->gfx_offset_y);
    offset.setPoint(imgOffsetX, imgOffsetY);
    frameSize.setSize(setup->gfx_w, setup->gfx_h);
    animator.construct(texture, *setup);

    setDefaults();
    setGravityScale(setup->gravity ? 1.0f : 0.f);

    if(setup->block_player)
        collide = COLLISION_ANY;
    else
    if(setup->block_player_top)
        collide = COLLISION_TOP;
    else
        collide = COLLISION_NONE;
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
        if(!blocks_left.isEmpty())
            direction=1;
        else
        if(!blocks_right.isEmpty())
            direction=-1;
        setSpeedX((motionSpeed*accelCof)*direction);
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
            x=ConfigManager::Animator_NPC[animator_ID].image(direction);
        else
            x=animator.image(direction);
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
                    float wOfsF = _width/warpFrameW; //Relative hitbox width
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
                    float wOfHB = _width/warpFrameW;                 //Relative width of hitbox
                    float wWAbs = warpFrameW*fWw;                   //Absolute width of frame
                    tPos.setRight(tPos.right()-(fWw-wOfHB-wOfs)-(warpSpriteOffset*wOfHB));
                    npc.setLeft( npc.left()+(warpSpriteOffset*_width) );
                    npc.setRight( npc.right()-(wWAbs-offset.x()-_width) );
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
    direction=data.direct;
    if(direction==0) direction = (rand()%2) ? -1 : 1;
    if(!setup) return;
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
