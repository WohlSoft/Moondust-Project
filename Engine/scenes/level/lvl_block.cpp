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

#include "lvl_block.h"
#include "../../data_configs/config_manager.h"

#include "lvl_scene_ptr.h"
#include <audio/pge_audio.h>
#include <graphics/gl_renderer.h>

LVL_Block::LVL_Block() : PGE_Phys_Object()
{
    type = LVLBlock;

    animated=false;
    sizable=false;
    animator_ID=0;

    shape=0;
    shape_slope_angle_ratio=0.0;

    offset_x = 0.f;
    offset_y = 0.f;

    isHidden=false;
    destroyed=false;

    fadeOffset=0;
    targetOffset=0;
    fade_step=0;
    fadeSpeed=0;

    taskToTransform=-1;
    taskToTransform_t=0;
    _isInited=false;
}

LVL_Block::~LVL_Block()
{}


void LVL_Block::init()
{
    if(_isInited) return;
    LvlSceneP::s->layers.registerItem(data.layer, this);
    transformTo_x(data.id);
    setPos(data.x, data.y);
    _isInited=true;
}

void LVL_Block::transformTo(long id, int type)
{
    if(id<=0) return;

    if(type==2)//Other block
    {
        transformTask_block t;
        t.block = this;
        t.id=id;
        t.type=type;

        LvlSceneP::s->block_transforms.push_back(t);
    }
    if(type==1)//Other NPC
    {
        // :-P
    }
}

void LVL_Block::transformTo_x(long id)
{
    obj_block *newSetup=NULL;
    if(_isInited)
    {
        if(data.id==(unsigned)abs(id)) return;

        if(!ConfigManager::lvl_block_indexes.contains(id)) return;

        newSetup = &ConfigManager::lvl_block_indexes[id];

        //Remove registration of switch block
        if(setup->switch_Block &&
                ( ((setup->switch_ID != newSetup->switch_ID) && (newSetup->switch_Block)) || (!newSetup->switch_Block) ) )
        {
            if(LvlSceneP::s->switch_blocks.contains(setup->switch_ID))
                LvlSceneP::s->switch_blocks[setup->switch_ID].removeAll(this);
        }
    } else
        newSetup = &ConfigManager::lvl_block_indexes[data.id];
    data.id = id;

    setup = newSetup;

    if(setup->sizable)
    {
        z_index = LevelScene::Z_blockSizable +
                ((double)data.y/1000000000.0) + 1 -
                ((double)data.w *0.00000000000001);
    }
    else
    {

        if(setup->view==1)
            z_index = LevelScene::Z_BlockFore;
        else
            z_index = LevelScene::Z_Block;
        LevelScene::zCounter += 0.0000000001;
        z_index += LevelScene::zCounter;
    }
    long tID = ConfigManager::getBlockTexture(data.id);
    if( tID >= 0 )
    {
        texId = ConfigManager::level_textures[tID].texture;
        texture = ConfigManager::level_textures[tID];
        animated = setup->animated;
        animator_ID = setup->animator_ID;
    }

    if(!setup->sizable)
    {
        data.w = texture.w;
        data.h = (texture.h/setup->frames);
    }

    setSize(data.w, data.h);

    sizable = setup->sizable;
    isHidden = data.invisible;
    collide_player = COLLISION_ANY;
    slippery_surface = data.slippery;
    if((setup->sizable) || (setup->collision==2))
    {
        collide_player = COLLISION_TOP;
    }
    else
    if(setup->collision==0)
    {
        collide_player = COLLISION_NONE;
    }

    this->shape = setup->phys_shape;
    if(shape==shape_tr_top_right)
        shape_slope_angle_ratio=-(height()/width());
    if(shape==shape_tr_top_left)
        shape_slope_angle_ratio=(height()/width());
    if(shape==shape_tr_bottom_right)
        shape_slope_angle_ratio=(height()/width());
    if(shape==shape_tr_bottom_left)
        shape_slope_angle_ratio=-(height()/width());

    isRectangle=(setup->phys_shape==0);
    if(setup->algorithm==3)
         ConfigManager::Animator_Blocks[animator_ID].setFrames(1, -1);

    // Register switch block
    if(setup->switch_Block)
    {
        if(!LvlSceneP::s->switch_blocks.contains(setup->switch_ID) )
            LvlSceneP::s->switch_blocks[setup->switch_ID].clear();
        LvlSceneP::s->switch_blocks[setup->switch_ID].push_back(this);
    }

    collide_npc=collide_player;
}


void LVL_Block::render(double camX, double camY)
{
    //Don't draw hidden block before it will be hitten
    if(isHidden) return;
    if(destroyed) return;

    PGE_RectF blockG;
    blockG.setRect(posX()-camX+offset_x,
                         posY()-camY+offset_y,
                         _width,
                         _height);


    AniPos x(0,1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_Blocks[animator_ID].image();

    glEnable(GL_TEXTURE_2D);
    glColor4f( 1.f, 1.f, 1.f, 1.f);

    glBindTexture( GL_TEXTURE_2D, texId );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    if(sizable)
    {
        int w = _width;
        int h = _height;

        int x,y, x2, y2, i, j;
        int hc, wc;

        x = qRound(qreal(texture.w)/3);  // Width of one piece
        y = qRound(qreal(texture.h)/3); // Height of one piece

        //Double size
        x2 = x<<1;
        y2 = y<<1;

        int pWidth = texture.w-x2;//Width of center piece
        int pHeight = texture.h-y2;//Height of center piece

        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width

        int dX=0; //Draw Offset. This need for crop junk on small sizes
        int dY=0;

        if(w < x2) dX = (x2-w)>>1; else dX=0;
        if(h < y2) dY = (y2-h)>>1; else dY=0;

        int totalW=((w-x2)/pWidth);
        int totalH=((h-y2)/pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, PGE_RectF(0, x+hc, x-dX, pHeight), PGE_RectF(0, y, x-dX, pHeight));
                hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(0, x+hc, x-dX, fLnt), PGE_RectF(0, y, x-dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, PGE_RectF(x+hc, 0, pWidth, y-dY), PGE_RectF(x, 0, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x+hc, 0, fLnt, y-dY), PGE_RectF(x, 0, fLnt, y-dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc=0;
            for(i=0; i<totalW; i++ )
            {
                drawPiece(blockG, PGE_RectF(x+hc, h-y+dY, pWidth, y-dY), PGE_RectF(x, texture.h-y+dY, pWidth, y-dY));
                    hc+=pWidth;
            }
                fLnt = (w-(x2))%pWidth;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x+hc, h-y+dY, fLnt, y-dY), PGE_RectF(x, texture.h-y+dY, fLnt, y-dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc=0;
            for(i=0; i<totalH; i++ )
            {
                drawPiece(blockG, PGE_RectF(w-x+dX, y+hc, x-dX, pHeight), PGE_RectF(texture.w-x+dX, y, x-dX, pHeight));
                    hc+=pHeight;
            }
                fLnt = (h-y2)%pHeight;
                if( fLnt != 0)
                    drawPiece(blockG, PGE_RectF(w-x+dX, y+hc, x-dX, fLnt), PGE_RectF(texture.w-x+dX, y, x-dX, fLnt));
        }


        //C Draw center
        if( (w > (x2)) && (h > (y2)))
        {
            hc=0;
            wc=0;
            for(i=0; i< totalH; i++ )
            {
                hc=0;
                for(j=0; j< totalW; j++ )
                {
                    drawPiece(blockG, PGE_RectF(x+hc, y+wc, pWidth, pHeight), PGE_RectF(x, y, pWidth, pHeight));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, PGE_RectF(x+hc, y+wc, fLnt, pHeight), PGE_RectF(x, y, fLnt, pHeight));
                wc+=pHeight;
            }

            fWdt = (h-y2)%pHeight;
            if(fWdt !=0)
            {
                hc=0;
                for(j=0; j<totalW; j++ )
                {
                    drawPiece(blockG, PGE_RectF(x+hc, y+wc, pWidth, fWdt), PGE_RectF(x, y, pWidth, fWdt));
                    hc+=pWidth;
                }
                    fLnt = (w-x2)%pWidth;
                    if(fLnt != 0 )
                        drawPiece(blockG, PGE_RectF(x+hc, y+wc, fLnt, fWdt), PGE_RectF(x, y, fLnt, fWdt));
            }

        }

        //Draw corners
         //1 Left-top
        drawPiece(blockG, PGE_RectF(0,0,x-dX,y-dY), PGE_RectF(0,0,x-dX, y-dY));
         //2 Right-top
        drawPiece(blockG, PGE_RectF(w-x+dX, 0, x-dX, y-dY), PGE_RectF(texture.w-x+dX, 0, x-dX, y-dY));
         //3 Right-bottom
        drawPiece(blockG, PGE_RectF(w-x+dX, h-y+dY, x-dX, y-dY), PGE_RectF(texture.w-x+dX, texture.h-y+dY, x-dX, y-dY));
         //4 Left-bottom
        drawPiece(blockG, PGE_RectF(0, h-y+dY, x-dX, y-dY), PGE_RectF(0, texture.h-y+dY, x-dX, y-dY));

    }
    else
    {
        GlRenderer::renderTextureCur( blockG.left(), blockG.top(), blockG.width(), blockG.height(), x.first, x.second );
    }

    glDisable(GL_TEXTURE_2D);
}

bool LVL_Block::isInited()
{
    return _isInited;
}

void LVL_Block::drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture)
{
    PGE_RectF tx;
    tx.setLeft(texture.left()/this->texture.w);
    tx.setRight(texture.right()/this->texture.w);
    tx.setTop(texture.top()/this->texture.h);
    tx.setBottom(texture.bottom()/this->texture.h);

    PGE_RectF blockG;
    blockG.setX(target.x()+block.x());
    blockG.setY(target.y()+block.y());
    blockG.setRight(target.x()+block.x()+block.width());
    blockG.setBottom(target.y()+block.y()+block.height());

    GlRenderer::renderTextureCur( blockG.left(), blockG.top(), blockG.width(), blockG.height(), tx.top(), tx.bottom(), tx.left(), tx.right() );
}




void LVL_Block::hit(LVL_Block::directions _dir)
{
    hitDirection = _dir;
    isHidden=false;
    data.invisible=false;
    bool doFade=false, triggerEvent=false, playHitSnd=false;

    PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);
    if((setup->destroyable)&&(data.npc_id==0))
    {
        triggerEvent=true;
        if(setup->destroy_sound_id==0)
            PGE_Audio::playSoundByRole(obj_sound_role::BlockSmashed);
        else
            PGE_Audio::playSound(setup->destroy_sound_id);
        destroyed=true;
        QString oldLayer=data.layer;
        LvlSceneP::s->layers.removeRegItem(data.layer, this);
        data.layer="Destroyed Blocks";
        LvlSceneP::s->layers.registerItem(data.layer, this);
        if(!data.event_destroy.isEmpty())
        {
            LvlSceneP::s->events.triggerEvent(data.event_destroy);
        }
        if(!data.event_emptylayer.isEmpty())
        {
            if(LvlSceneP::s->layers.isEmpty(oldLayer))
                LvlSceneP::s->events.triggerEvent(data.event_emptylayer);
        }
        return;
    }

    if(data.npc_id<0)
    {
        triggerEvent=true;
        playHitSnd=true;
        //Coin!
        PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin);
        data.npc_id++;
        doFade=true;
        if((!setup->bounce)&&(!setup->switch_Button))
        {
            if(data.npc_id==0)
                transformTo(setup->transfororm_on_hit_into, 2);
        }
    }
    else
    if(data.npc_id>0)
    {
        triggerEvent=true;
        playHitSnd=true;
        //NPC!
        if(ConfigManager::lvl_npc_indexes.contains(data.npc_id))
        {
            obj_npc &npcSet=ConfigManager::lvl_npc_indexes[data.npc_id];

            if(npcSet.block_spawn_sound)
                PGE_Audio::playSoundByRole(obj_sound_role::BlockOpen);

            doFade=true;
            if((!setup->bounce)&&(!setup->switch_Button))
            {
                transformTo(setup->transfororm_on_hit_into, 2);
            }

            LevelNPC npcDef = FileFormats::CreateLvlNpc();
            npcDef.id=data.npc_id;
              data.npc_id=0;
            npcDef.direct = 0;
            npcDef.x=data.x;
            npcDef.y=data.y-(hitDirection==up?data.h:(-data.h*2));

            LVL_Npc * npc;
            npc = LvlSceneP::s->spawnNPC(npcDef,
                                        (npcSet.block_spawn_type==0)?
                                             LevelScene::GENERATOR_WARP:
                                             LevelScene::GENERATOR_APPEAR,
                                         hitDirection==up?LevelScene::SPAWN_UP:LevelScene::SPAWN_DOWN,
                                         false);
            if(npc)
            {
                npc->setCenterX(posRect.center().x());
                if(_dir==up)
                    npc->setPosY(posRect.top()-npc->height());
                else
                    npc->setPosY(posRect.bottom());

                if(npcSet.block_spawn_type==1)
                {
                    npc->setSpeedY(fabs(npcSet.block_spawn_speed)*((_dir==up)?-1:1));
                }
            }
        }
    }

    if(setup->switch_Button)
    {
        triggerEvent=true;
        LvlSceneP::s->toggleSwitch(setup->switch_ID);
    }

    if(setup->hitable)
    {
        triggerEvent=true;
        transformTo(setup->spawn_obj_id, setup->spawn_obj);
        doFade=true;
        playHitSnd=!destroyed;
    }

    if(playHitSnd && (setup->hit_sound_id>0))
    {
        PGE_Audio::playSound(setup->hit_sound_id);
    }

    if(triggerEvent && (!data.event_hit.isEmpty()))
    {
        LvlSceneP::s->events.triggerEvent(data.event_hit);
    }

    if(doFade)
    {
        if(!isFading())
            LvlSceneP::s->fading_blocks.push_back(this);
        fadeOffset=0.f;
        setFade(5, 1.0f, 0.07f);
    }
}

GLdouble LVL_Block::zIndex()
{
    if(fadeOffset!=0.f)
        return z_index+10;
    else
        return z_index;
}


/**************************Fader*******************************/
void LVL_Block::setFade(int speed, float target, float step)
{
    fade_step = fabs(step);
    targetOffset = target;
    fadeSpeed = speed;
    manual_ticks = speed;
}

bool LVL_Block::isFading()
{
    return (fadeOffset!=targetOffset);
}

bool LVL_Block::tickFader(int ticks)
{
    if(fadeSpeed<1) return true; //Idling animation
    manual_ticks-=abs(ticks);
        while(manual_ticks<=0)
        {
            fadeStep();
            manual_ticks+=fadeSpeed;
        }
    return (fadeOffset==targetOffset);
}

void LVL_Block::fadeStep()
{
    if(fadeOffset < targetOffset)
        fadeOffset+=fade_step;
    else
        fadeOffset-=fade_step;

    if(fadeOffset>=1.0f)
        setFade(fadeSpeed, 0.0f, fade_step);

    if(fadeOffset>1.0f) fadeOffset = 1.0f;
    else
    if(fadeOffset<0.0f) fadeOffset = 0.0f;

    switch(hitDirection)
    {
        case up:      offset_y = -16*fadeOffset; break;
        case down:    offset_y = 16*fadeOffset; break;
        case left:    offset_x = -16*fadeOffset; break;
        case right:   offset_x = 16*fadeOffset; break;
        default: break;
    }

}
/**************************Fader**end**************************/
