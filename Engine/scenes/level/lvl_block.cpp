/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../scene_level.h"
#include <audio/pge_audio.h>
#include <graphics/gl_renderer.h>

void LVL_Block::construct()
{
    m_blocked[1] = Block_ALL;
    m_blocked[2] = Block_ALL;
    animated = false;
    sizable = false;
    animator_ID = 0;
    m_isHidden = false;
    m_destroyed = false;
    setup = nullptr;
    taskToTransform = -1;
    taskToTransform_t = 0;
    transformedFromBlockID = -1ul;
    transformedFromNpcID = -1ul;
    _isInited = false;
}

LVL_Block::LVL_Block(LevelScene *_parent) :
    PGE_Phys_Object(_parent)
{
    type = LVLBlock;
    m_bodytype = Body_STATIC;
    hitDirection = up;
    fadeOffset = 0;
    targetOffset = 0;
    fade_step = 0;
    fadeSpeed = 0;
    manual_ticks = 0.0;
    offset_x = 0.0;
    offset_y = 0.0;
    construct();
}

LVL_Block::~LVL_Block()
{}


void LVL_Block::init(bool force)
{
    if(_isInited && !force)
        return;

    if(!_isInited)
        dataInitial = data;
    else
    if(force)
    {
        construct();//reset ALL values into initial state
        data = dataInitial;
    }

    m_scene->layers.registerItem(data.layer, this);
    transformTo_x(data.id);
    _isInited = true;
    m_momentum.saveOld();
}

void LVL_Block::transformTo(unsigned long id, int type)
{
    if(id == 0) return;

    if(type == 2) //Other block
    {
        transformTask_block t;
        t.block = this;
        t.id = id;
        t.type = type;
        m_scene->block_transforms.push_back(t);
    }

    if(type == 1) //Other NPC
    {
        LevelNPC def = FileFormats::CreateLvlNpc();
        def.id = id;
        def.x = long(round(posX()));
        def.y = long(round(posY()));
        def.direct = 0;
        def.generator = false;
        def.layer = data.layer;
        def.attach_layer = "";
        def.event_activate = "";
        def.event_die = "";
        def.event_talk = "";
        def.event_emptylayer = "";
        LVL_Npc *npc = m_scene->spawnNPC(def,
                                        LevelScene::GENERATOR_APPEAR,
                                        LevelScene::SPAWN_UP, true);

        if(npc)
        {
            npc->transformedFromBlock = this;
            npc->transformedFromBlockID = data.id;
            npc->setCenterPos(m_momentum.centerX(), m_momentum.centerY());
            npc->m_momentum.saveOld();
            npc->data.x = long(round(npc->m_momentum.x));
            npc->data.y = long(round(npc->m_momentum.y));
        }

        destroy(false);
    }
}

void LVL_Block::transformTo_x(unsigned long id)
{
    obj_block *newSetup = nullptr;
    if(_isInited)
    {
        if(data.id == id)
            return;

        if(!ConfigManager::lvl_block_indexes.contains(id))
            return;

        newSetup = &ConfigManager::lvl_block_indexes[id];

        //Remove registration of switch block
        if(setup->setup.switch_Block &&
           (((setup->setup.switch_ID != newSetup->setup.switch_ID) && (newSetup->setup.switch_Block)) || (!newSetup->setup.switch_Block)))
        {
            LevelScene::SwitchBlocksMap::iterator i = m_scene->switch_blocks.find(setup->setup.switch_ID);
            if(i != m_scene->switch_blocks.end())
            {
                std::remove(i->second.begin(), i->second.end(), this);
            }
        }
        transformedFromBlockID = data.id;//Remember transform source
    }
    else
        newSetup = &ConfigManager::lvl_block_indexes[data.id];

    data.id = id;
    setup = newSetup;

    if(setup->setup.sizable)
    {
        z_index = LevelScene::zOrder.blockBack2 +
                  (static_cast<long double>(data.y) / 1000000000.0L) + 1 -
                  (static_cast<long double>(data.w) * 0.00000000000001L);
    }
    else
    {
        if(setup->setup.view == 1)
            z_index = LevelScene::zOrder.blockFront1;
        else
            z_index = LevelScene::zOrder.blockBack1;

        m_scene->zCounter += 0.0000000000001L;
        z_index += m_scene->zCounter;

        if(m_scene->zCounter >= 1.0L)
            m_scene->zCounter = 0.0L;
    }

    bool do_init_player_switch = ((setup->animator_ID <= 0) && (setup->setup.plSwitch_Button));
    bool do_init_player_filter = ((setup->animator_ID <= 0) && (setup->setup.plFilter_Block));
    int tID = ConfigManager::getBlockTexture(data.id);
    if(tID >= 0)
    {
        texId       = ConfigManager::level_textures[tID].texture;
        texture     = ConfigManager::level_textures[tID];
        animated    = setup->setup.animated;
        animator_ID = setup->animator_ID;
    }

    if(!setup->setup.sizable)
    {
        data.w = texture.w;
        data.h = (unsigned(texture.h) / setup->setup.frames);
    }

    if(!_isInited)
    {
        m_momentum.x = data.x;
        m_momentum.y = data.y;
    }

    setSize(data.w, data.h);
    sizable = setup->setup.sizable;
    //LEGACY_collide_player = COLLISION_ANY;
    m_blocked[1] = Block_ALL;
    m_blocked[2] = Block_ALL;
    m_slippery_surface = data.slippery;

    if((setup->setup.sizable) || (setup->setup.collision == 2))
    {
        m_blocked[1] = Block_TOP;
        m_blocked[2] = Block_TOP;
    }
    else if(setup->setup.collision == 0)
    {
        m_blocked[1] = Block_NONE;
        m_blocked[2] = Block_NONE;
    }

    memcpy(m_blockedOrigin, m_blocked, sizeof(int)*BLOCK_FILTER_COUNT);
    lua_setInvisible(data.invisible);

    switch(setup->setup.phys_shape)
    {
    default:
    case 0:
        m_shape = SL_Rect;
        break;

    case 1:
        m_shape = SL_LeftBottom;
        break;

    case -1:
        m_shape = SL_RightBottom;
        break;

    case 2:
        m_shape = SL_LeftTop;
        break;

    case -2:
        m_shape = SL_RightTop;
        break;
    }

    m_danger[1] = Block_NONE;
    m_danger[2] = Block_NONE;

    switch(setup->setup.danger)
    {
    case 1:
        m_danger[1] = Block_LEFT;
        break;

    case -1:
        m_danger[1] = Block_RIGHT;
        break;

    case 2:
        m_danger[1] = Block_TOP;
        break;

    case -2:
        m_danger[1] = Block_BOTTOM;
        break;

    case 3:
        m_danger[1] = Block_LEFT | Block_RIGHT;
        break;

    case -3:
        m_danger[1] = Block_TOP | Block_BOTTOM;
        break;

    case 4:
        m_danger[1] = Block_ALL;
        break;
    }

    // Register switch block
    if(setup->setup.switch_Block)
    {
        LevelScene::SwitchBlocksMap::iterator i = m_scene->switch_blocks.find(setup->setup.switch_ID);
        if(i == m_scene->switch_blocks.end())
        {
            m_scene->switch_blocks.insert({setup->setup.switch_ID, LevelScene::BlocksList()});
            i = m_scene->switch_blocks.find(setup->setup.switch_ID);
            SDL_assert(i != m_scene->switch_blocks.end());
        }
        i->second.push_back(this);

        //Fill switch states until it will be fited to defined SwitchID
        while(static_cast<unsigned int>(m_scene->switch_states.size()) <= setup->setup.switch_ID)
            m_scene->switch_states.push_back(false);
    }

    //Register player switch block if needed
    if(do_init_player_switch)
        m_scene->character_switchers.buildSwitch(*setup);

    //Register player filter block if needed
    if(do_init_player_filter)
        m_scene->character_switchers.buildBrick(*setup);
}


void LVL_Block::render(double camX, double camY)
{
    //Don't draw hidden block before it will be hitten
    if(m_isHidden) return;

    if(m_destroyed) return;

    PGE_RectF blockG;
    blockG.setRect(posX() - camX + offset_x,
                   posY() - camY + offset_y,
                   m_width_registered,
                   m_height_registered);
    AniPos x(0, 1);

    if(animated) //Get current animated frame
        x = ConfigManager::Animator_Blocks[size_t(animator_ID)].image();

    GlRenderer::BindTexture(&texture);
    GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, 1.0f);

    if(sizable)
    {
        int w = int(round(m_width_registered));
        int h = int(round(m_height_registered));
        int x, y, x2, y2, i, j;
        int hc, wc;
        x = qRound(qreal(texture.w) / 3); // Width of one piece
        y = qRound(qreal(texture.h) / 3); // Height of one piece
        //Double size
        x2 = x << 1;
        y2 = y << 1;
        int pWidth = texture.w - x2; //Width of center piece
        int pHeight = texture.h - y2; //Height of center piece
        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width
        int dX = 0; //Draw Offset. This need for crop junk on small sizes
        int dY = 0;

        if(w < x2) dX = (x2 - w) >> 1;
        else dX = 0;

        if(h < y2) dY = (y2 - h) >> 1;
        else dY = 0;

        int totalW = ((w - x2) / pWidth);
        int totalH = ((h - y2) / pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc = 0;

            for(i = 0; i < totalH; i++)
            {
                drawPiece(blockG, PGE_RectF(0, x + hc, x - dX, pHeight), PGE_RectF(0, y, x - dX, pHeight));
                hc += pHeight;
            }

            fLnt = (h - y2) % pHeight;

            if(fLnt != 0)
                drawPiece(blockG, PGE_RectF(0, x + hc, x - dX, fLnt), PGE_RectF(0, y, x - dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc = 0;

            for(i = 0; i < totalW; i++)
            {
                drawPiece(blockG, PGE_RectF(x + hc, 0, pWidth, y - dY), PGE_RectF(x, 0, pWidth, y - dY));
                hc += pWidth;
            }

            fLnt = (w - (x2)) % pWidth;

            if(fLnt != 0)
                drawPiece(blockG, PGE_RectF(x + hc, 0, fLnt, y - dY), PGE_RectF(x, 0, fLnt, y - dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc = 0;

            for(i = 0; i < totalW; i++)
            {
                drawPiece(blockG, PGE_RectF(x + hc, h - y + dY, pWidth, y - dY), PGE_RectF(x, texture.h - y + dY, pWidth, y - dY));
                hc += pWidth;
            }

            fLnt = (w - (x2)) % pWidth;

            if(fLnt != 0)
                drawPiece(blockG, PGE_RectF(x + hc, h - y + dY, fLnt, y - dY), PGE_RectF(x, texture.h - y + dY, fLnt, y - dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc = 0;

            for(i = 0; i < totalH; i++)
            {
                drawPiece(blockG, PGE_RectF(w - x + dX, y + hc, x - dX, pHeight), PGE_RectF(texture.w - x + dX, y, x - dX, pHeight));
                hc += pHeight;
            }

            fLnt = (h - y2) % pHeight;

            if(fLnt != 0)
                drawPiece(blockG, PGE_RectF(w - x + dX, y + hc, x - dX, fLnt), PGE_RectF(texture.w - x + dX, y, x - dX, fLnt));
        }

        //C Draw center
        if((w > (x2)) && (h > (y2)))
        {
            hc = 0;
            wc = 0;

            for(i = 0; i < totalH; i++)
            {
                hc = 0;

                for(j = 0; j < totalW; j++)
                {
                    drawPiece(blockG, PGE_RectF(x + hc, y + wc, pWidth, pHeight), PGE_RectF(x, y, pWidth, pHeight));
                    hc += pWidth;
                }

                fLnt = (w - x2) % pWidth;

                if(fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x + hc, y + wc, fLnt, pHeight), PGE_RectF(x, y, fLnt, pHeight));

                wc += pHeight;
            }

            fWdt = (h - y2) % pHeight;

            if(fWdt != 0)
            {
                hc = 0;

                for(j = 0; j < totalW; j++)
                {
                    drawPiece(blockG, PGE_RectF(x + hc, y + wc, pWidth, fWdt), PGE_RectF(x, y, pWidth, fWdt));
                    hc += pWidth;
                }

                fLnt = (w - x2) % pWidth;

                if(fLnt != 0)
                    drawPiece(blockG, PGE_RectF(x + hc, y + wc, fLnt, fWdt), PGE_RectF(x, y, fLnt, fWdt));
            }
        }

        //Draw corners
        //1 Left-top
        drawPiece(blockG, PGE_RectF(0, 0, x - dX, y - dY), PGE_RectF(0, 0, x - dX, y - dY));
        //2 Right-top
        drawPiece(blockG, PGE_RectF(w - x + dX, 0, x - dX, y - dY), PGE_RectF(texture.w - x + dX, 0, x - dX, y - dY));
        //3 Right-bottom
        drawPiece(blockG, PGE_RectF(w - x + dX, h - y + dY, x - dX, y - dY), PGE_RectF(texture.w - x + dX, texture.h - y + dY, x - dX, y - dY));
        //4 Left-bottom
        drawPiece(blockG, PGE_RectF(0, h - y + dY, x - dX, y - dY), PGE_RectF(0, texture.h - y + dY, x - dX, y - dY));
    }
    else
        GlRenderer::renderTextureCur(float(blockG.left()), float(blockG.top()), float(blockG.width()), float(blockG.height()), float(x.first), float(x.second));

    GlRenderer::UnBindTexture();
}

bool LVL_Block::isInited()
{
    return _isInited;
}

long LVL_Block::lua_getID()
{
    return long(data.id);
}

int LVL_Block::lua_contentID_old()
{
    return int(data.npc_id + 1000);
}

void LVL_Block::lua_setContentID_old(int npcid)
{
    data.npc_id = npcid - 1000;
}

int LVL_Block::lua_contentID()
{
    return int(data.npc_id);
}

void LVL_Block::lua_setContentID(int npcid)
{
    data.npc_id = npcid;
}

bool LVL_Block::lua_invisible()
{
    return m_isHidden;
}

void LVL_Block::lua_setInvisible(bool iv)
{
    if(m_isHidden && !iv)
        memcpy(m_blocked, m_blockedOrigin, sizeof(int)*BLOCK_FILTER_COUNT);
    else if(!m_isHidden && iv)
    {
        m_blocked[1] = Block_BOTTOM;
        m_blocked[2] = Block_BOTTOM;
    }

    data.invisible = iv;
    m_isHidden = iv;
}

bool LVL_Block::lua_slippery()
{
    return data.slippery;
}

void LVL_Block::lua_setSlippery(bool sl)
{
    data.slippery = sl;
}

bool LVL_Block::lua_isSolid()
{
    return (m_blocked[2] == Block_ALL);
}

void LVL_Block::drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture)
{
    PGE_RectF tx;
    tx.setLeft(texture.left() / this->texture.w);
    tx.setRight(texture.right() / this->texture.w);
    tx.setTop(texture.top() / this->texture.h);
    tx.setBottom(texture.bottom() / this->texture.h);
    PGE_RectF blockG;
    blockG.setX(target.x() + block.x());
    blockG.setY(target.y() + block.y());
    blockG.setRight(target.x() + block.x() + block.width());
    blockG.setBottom(target.y() + block.y() + block.height());
    GlRenderer::renderTextureCur(float(blockG.left()), float(blockG.top()), float(blockG.width()), float(blockG.height()), float(tx.top()), float(tx.bottom()), float(tx.left()), float(tx.right()));
}




void LVL_Block::hit(LVL_Block::directions _dir)
{
    hitDirection = _dir;

    if(m_isHidden)
        lua_setInvisible(false);

    bool doFade = false, triggerEvent = false, playHitSnd = false;
    PGE_Audio::playSoundByRole(obj_sound_role::BlockHit);

    if((setup->setup.destroyable) && (data.npc_id == 0))
    {
        destroy(true);
        return;
    }

    if(data.npc_id < 0)
    {
        triggerEvent = true;
        playHitSnd = true;
        //Coin!
        PGE_Audio::playSoundByRole(obj_sound_role::BonusCoin);
        data.npc_id++;
        doFade = true;

        if((!setup->setup.bounce) && (!setup->setup.switch_Button))
        {
            if(data.npc_id == 0)
                transformTo(static_cast<unsigned long>(setup->setup.transfororm_on_hit_into), 2);
        }

        if(!m_scene->player_states.empty())
            m_scene->player_states[0].appendCoins(1);

        //! TEMPORARY AND EXPERIMENTAL!, REPLACE THIS WITH LUA
        {
            SpawnEffectDef effect;
            effect.id = 11;
            effect.startX = posCenterX();
            effect.startY = top() - 16.;
            effect.gravity = 20.0;
            effect.start_delay = 0u;
            effect.max_vel_y = 25.;
            effect.velocityY = -7.5;
            effect.delay = 710;
            effect.animationLoops = 0;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(0);
            effect.frame_sequence.append(1);
            effect.frame_sequence.append(2);
            effect.frame_sequence.append(3);
            effect.framespeed = 32;
            m_scene->launchEffect(effect, true);
            effect.id = 11;
            effect.startX = posCenterX();
            effect.startY = top() - 32.0;
            effect.gravity = 0.0;
            effect.start_delay = 710;
            effect.max_vel_y = 25.0;
            effect.velocityY = 0.0;
            effect.animationLoops = 1;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(4);
            effect.frame_sequence.append(5);
            effect.frame_sequence.append(6);
            effect.framespeed = 125;
            effect.delay = 0;
            m_scene->launchEffect(effect, true);
            //Points!
            effect.id = 79;
            effect.startX = posCenterX() + static_cast<double>((rand() % 16) * (rand() % 2 ? 1 : -1));
            effect.startY = top() - 16.0 + static_cast<double>((rand() % 16) * (rand() % 2 ? 1 : -1));
            effect.gravity = 1.8;
            effect.start_delay = 710.0f;
            effect.max_vel_y = 7.0;
            effect.velocityY = -1.8;
            effect.animationLoops = 0;
            effect.frame_sequence.clear();
            effect.frame_sequence.append(0);
            effect.framespeed = 125;
            effect.delay = 1000;
            m_scene->launchEffect(effect, true);
        }
    }
    else if(data.npc_id > 0)
    {
        triggerEvent = true;
        playHitSnd = true;

        //NPC!
        if(ConfigManager::lvl_npc_indexes.contains(static_cast<unsigned long>(data.npc_id)))
        {
            obj_npc &npcSet = ConfigManager::lvl_npc_indexes[static_cast<unsigned long>(data.npc_id)];

            if(npcSet.block_spawn_sound)
                PGE_Audio::playSoundByRole(obj_sound_role::BlockOpen);

            doFade = true;

            if((!setup->setup.bounce) && (!setup->setup.switch_Button))
                transformTo(static_cast<unsigned long>(setup->setup.transfororm_on_hit_into), 2);

            LevelNPC npcDef = FileFormats::CreateLvlNpc();
            npcDef.id = unsigned(data.npc_id);
            data.npc_id = 0;
            npcDef.direct = 0;
            npcDef.x = data.x;
            npcDef.y = data.y - (hitDirection == up ? data.h : (-data.h * 2));
            LVL_Npc *npc;
            npc = m_scene->spawnNPC(npcDef,
                                   (npcSet.block_spawn_type == 0) ?
                                   LevelScene::GENERATOR_WARP :
                                   LevelScene::GENERATOR_APPEAR,
                                   hitDirection == up ? LevelScene::SPAWN_UP : LevelScene::SPAWN_DOWN,
                                   false);

            if(npc)
            {
                npc->setCenterX(m_momentum.centerX());

                if(_dir == up)
                    npc->setPosY(m_momentum.top() - npc->height());
                else
                    npc->setPosY(m_momentum.bottom());

                if(npcSet.block_spawn_type == 1)
                    npc->setSpeedY(fabs(npcSet.block_spawn_speed) * ((_dir == up) ? -1 : 1));
            }
        }
    }

    if(setup->setup.switch_Button)
    {
        triggerEvent = true;
        m_scene->toggleSwitch(setup->setup.switch_ID);
    }

    if(setup->setup.hitable)
    {
        triggerEvent = true;
        transformTo(setup->setup.spawn_obj_id, setup->setup.spawn_obj);
        doFade = true;
        playHitSnd = !m_destroyed;
    }

    if(playHitSnd && (setup->setup.hit_sound_id > 0))
        PGE_Audio::playSound(setup->setup.hit_sound_id);

    if(triggerEvent)
    {
        //Register block as "destroyed" to be able turn it into it's initial state
        m_scene->layers.registerItem(DESTROYED_LAYER_NAME, this);
    }

    if(triggerEvent && (!data.event_hit.empty()))
        m_scene->events.triggerEvent(data.event_hit);

    if(doFade)
    {
        if(!isFading())
            m_scene->fading_blocks.push_back(this);

        fadeOffset = 0.0;
        setFade(5, 1.0, 0.07);
    }
}

void LVL_Block::hit(directions _dir, LVL_Player *, int numHits)
{
    while(numHits > 0)
    {
        hit(_dir);
        numHits--;
    }
}

void LVL_Block::destroy(bool playEffect)
{
    if(playEffect)
    {
        if(setup->setup.destroy_sound_id == 0)
            PGE_Audio::playSoundByRole(obj_sound_role::BlockSmashed);
        else
            PGE_Audio::playSound(setup->setup.destroy_sound_id);

        Scene_Effect_Phys p;
        p.decelerate_x = 1.5;
        p.max_vel_y    = 25.0;
#define ROFFSET ((static_cast<double>(rand()%10))*0.1)
        m_scene->launchStaticEffectC(1, posCenterX(), posCenterY(), 0, 5000, -3.0 + ROFFSET, -6.0 + ROFFSET, 18.0, 0, p);
        m_scene->launchStaticEffectC(1, posCenterX(), posCenterY(), 0, 5000, -4.0 + ROFFSET, -7.0 + ROFFSET, 18.0, 0, p);
        m_scene->launchStaticEffectC(1, posCenterX(), posCenterY(), 0, 5000,  3.0 + ROFFSET, -6.0 + ROFFSET, 18.0, 0, p);
        m_scene->launchStaticEffectC(1, posCenterX(), posCenterY(), 0, 5000,  4.0 + ROFFSET, -7.0 + ROFFSET, 18.0, 0, p);
#undef ROFFSET
    }

    m_blocked[1] = Block_NONE;
    m_blocked[2] = Block_NONE;
    m_destroyed = true;
    std::string oldLayer = data.layer;
    m_scene->layers.removeRegItem(data.layer, this);
    data.layer = DESTROYED_LAYER_NAME;
    m_scene->layers.registerItem(data.layer, this);

    if(!data.event_destroy.empty())
        m_scene->events.triggerEvent(data.event_destroy);

    if(!data.event_emptylayer.empty())
    {
        if(m_scene->layers.isEmpty(oldLayer))
            m_scene->events.triggerEvent(data.event_emptylayer);
    }
}

void LVL_Block::setDestroyed(bool dstr)
{
    if(!m_isHidden)
    {
        if(m_destroyed && !dstr)
            memcpy(m_blocked, m_blockedOrigin, sizeof(int)*BLOCK_FILTER_COUNT);
    }
    else
    {
        if(!m_destroyed && dstr)
        {
            m_blocked[1] = Block_NONE;
            m_blocked[2] = Block_NONE;
        }
        else if(m_destroyed && !dstr)
        {
            m_blocked[1] = Block_BOTTOM;
            m_blocked[2] = Block_NONE;
        }
    }

    m_destroyed = dstr;
}

long double LVL_Block::zIndex()
{
    if(fadeOffset != 0.0)
        return z_index + 0.0001L;
    else
        return z_index;
}


/**************************Fader*******************************/
void LVL_Block::setFade(double speed, double target, double step)
{
    fade_step = fabs(step);
    targetOffset = target;
    fadeSpeed = speed;
    manual_ticks = speed;
}

bool LVL_Block::isFading()
{
    return memcmp(&fadeOffset, &targetOffset, sizeof(double));
}

bool LVL_Block::tickFader(double ticks)
{
    if(fadeSpeed < 1.0)
        return true; //Idling animation

    manual_ticks -= fabs(ticks);

    while(manual_ticks <= 0)
    {
        fadeStep();
        manual_ticks += fadeSpeed;
    }

    return !isFading();
}

void LVL_Block::fadeStep()
{
    if(fadeOffset < targetOffset)
        fadeOffset += fade_step;
    else
        fadeOffset -= fade_step;

    if(fadeOffset >= 1.0)
        setFade(fadeSpeed, 0.0, fade_step);

    if(fadeOffset > 1.0) fadeOffset = 1.0;
    else if(fadeOffset < 0.0) fadeOffset = 0.0;

    switch(hitDirection)
    {
    case up:
        offset_y = -16 * fadeOffset;
        break;

    case down:
        offset_y = 16 * fadeOffset;
        break;

    case left:
        offset_x = -16 * fadeOffset;
        break;

    case right:
        offset_x = 16 * fadeOffset;
        break;
        //default: break;
    }
}
/**************************Fader**end**************************/
