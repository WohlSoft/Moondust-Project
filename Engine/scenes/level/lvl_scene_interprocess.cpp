/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#include "../scene_level.h"

#include <data_configs/config_manager.h>
#include <networking/intproc.h>
#include <common_features/logger.h>
#include <audio/pge_audio.h>
#include <fontman/font_manager.h>

void LevelScene::process_InterprocessCommands()
{
    if(!IntProc::isEnabled())
        return;

    IntProc::cmdLock();

    //Recive external commands!
    if(IntProc::hasCommand())
    {
        switch(IntProc::commandType())
        {
        case IntProc::MsgBox:
        {
            m_messages.showMsg(IntProc::getCMD());
            break;
        }

        case IntProc::Cheat:
            break;

        case IntProc::SetLayer:
        {
            std::string layer = IntProc::getCMD();
            m_placingMode_bgo.layer = layer;
            m_placingMode_block.layer = layer;
            m_placingMode_npc.layer = layer;
            break;
        }

        case IntProc::PlaceItem:
        {
            std::string raw = IntProc::getCMD();
            pLogDebug(raw.c_str());
            LevelData got;
            PGE_FileFormats_misc::RawTextInput raw_file(&raw);
            FileFormats::ReadExtendedLvlFile(raw_file, got);

            if(!got.meta.ReadFileValid)
            {
                LogDebug(got.meta.ERROR_info);
                break;
            }

            //Don't show placing item sprite before mouse will be on screen
            m_placingMode_renderAt.setPoint(PGE_Window::Width + m_placingMode_drawSize.x() + 16,
                                          PGE_Window::Height + m_placingMode_drawSize.y() + 16);
            PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrab2);

            if(raw.compare(0, 11, "BLOCK_PLACE") == 0)
            {
                if(got.blocks.empty())
                    break;
                m_placingMode = true;
                m_placingMode_item_type = 0;
                m_placingMode_block = got.blocks[0];
                int tID = ConfigManager::getBlockTexture(m_placingMode_block.id);
                if(tID  >= 0)
                {
                    m_placingMode_texture = ConfigManager::level_textures[tID];
                    obj_block &bl = ConfigManager::lvl_block_indexes[m_placingMode_block.id];
                    m_placingMode_animated = bl.setup.animated;
                    m_placingMode_animatorID = bl.animator_ID;
                    m_placingMode_drawSize.setX(m_placingMode_texture.frame_w);
                    m_placingMode_drawSize.setY(m_placingMode_texture.frame_h);
                }
            }
            else if(raw.compare(0, 9, "BGO_PLACE") == 0)
            {
                if(got.bgo.empty())
                    break;
                m_placingMode = true;
                m_placingMode_item_type = 1;
                m_placingMode_bgo = got.bgo[0];
                int tID = ConfigManager::getBgoTexture(m_placingMode_bgo.id);
                if(tID  >= 0)
                {
                    m_placingMode_texture = ConfigManager::level_textures[tID];
                    obj_bgo &bg = ConfigManager::lvl_bgo_indexes[m_placingMode_bgo.id];
                    m_placingMode_animated = bg.setup.animated;
                    m_placingMode_animatorID = bg.animator_ID;
                    m_placingMode_drawSize.setX(m_placingMode_texture.frame_w);
                    m_placingMode_drawSize.setY(m_placingMode_texture.frame_h);
                }
            }
            else if(raw.compare(0, 9, "NPC_PLACE") == 0)
            {
                if(got.npc.empty())
                    break;
                m_placingMode = true;
                m_placingMode_item_type = 2;
                m_placingMode_npc = got.npc[0];
                int tID = ConfigManager::getNpcTexture(m_placingMode_npc.id);
                if(tID >= 0)
                {
                    m_placingMode_texture = ConfigManager::level_textures[tID];
                    obj_npc &np = ConfigManager::lvl_npc_indexes[m_placingMode_npc.id];
                    m_placingMode_animated = ((np.setup.frames > 1) || (np.setup.framestyle > 0));

                    if(np.animator_ID < 0)
                    {
                        if((np.setup.frames > 1) || (np.setup.framestyle > 0))
                        {
                            AdvNpcAnimator animator(m_placingMode_texture, np);
                            ConfigManager::Animator_NPC.push_back(animator);
                            ConfigManager::Animator_NPC.back().start();
                            np.animator_ID = ConfigManager::Animator_NPC.size() - 1;
                        }
                        else
                            m_placingMode_animated = false;
                    }

                    m_placingMode_animatorID = np.animator_ID;
                    m_placingMode_drawSize.setX(np.setup.gfx_w);
                    m_placingMode_drawSize.setY(np.setup.gfx_h);
                    int d = m_placingMode_npc.direct;

                    if(d == 0) d = -1;

                    double physW = static_cast<double>(np.setup.width);
                    double physH = static_cast<double>(np.setup.height);
                    double gfxW = static_cast<double>(np.setup.gfx_w);
                    double gfxH = static_cast<double>(np.setup.gfx_h);
                    double offsetX = static_cast<double>(np.setup.gfx_offset_x);
                    double offsetY = static_cast<double>(np.setup.gfx_offset_y);
                    double imgOffsetX = -(round((gfxW - physW) / -2.0) + (-offsetX * d));
                    double imgOffsetY = round(- gfxH + physH + offsetY);
                    m_placingMode_renderOffset.setPoint(imgOffsetX, imgOffsetY);
                }
            }
            else PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);

            break;
        }// place
        }// switch
    }

    IntProc::cmdUnLock();
}

void LevelScene::drawPlacingItem()
{
    if(!m_placingMode) return;

    AniPos x(0, 1);
    int d = 0;

    double posX = m_placingMode_renderAt.x();
    double posY = m_placingMode_renderAt.y();
    double posW = m_placingMode_drawSize.x();
    double posH = m_placingMode_drawSize.y();

    switch(m_placingMode_item_type)
    {
    case 0:
        if(m_placingMode_animated)
            x = ConfigManager::Animator_Blocks[m_placingMode_animatorID].image();

        GlRenderer::renderTexture(&m_placingMode_texture,
                                  static_cast<float>(posX),
                                  static_cast<float>(posY),
                                  static_cast<float>(posW),
                                  static_cast<float>(posH),
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
        if(!m_placingMode_block.layer.empty() &&
            SDL_strcasecmp(m_placingMode_block.layer.c_str(), "Default") != 0)
        {
            FontManager::printText(m_placingMode_block.layer,
                                   int(posX) + 28, int(posY) + 34);
        }
        break;

    case 1:
        if(m_placingMode_animated)
            x = ConfigManager::Animator_BGO[m_placingMode_animatorID].image();

        GlRenderer::renderTexture(&m_placingMode_texture,
                                  static_cast<float>(posX),
                                  static_cast<float>(posY),
                                  static_cast<float>(posW),
                                  static_cast<float>(posH),
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
        if(!m_placingMode_bgo.layer.empty() &&
            SDL_strcasecmp(m_placingMode_bgo.layer.c_str(), "Default") != 0)
        {
            FontManager::printText(m_placingMode_bgo.layer,
                                   int(posX) + 28, int(posY) + 34);
        }
        break;

    case 2:
        if(m_placingMode_animated)
            x = ConfigManager::Animator_NPC[m_placingMode_animatorID].image(m_placingMode_npc.direct);
        else
            x = AniPos(0, m_placingMode_drawSize.y() / m_placingMode_texture.h);

        d = m_placingMode_npc.direct;

        if(d == 0) d = -1;

        GlRenderer::renderTexture(&m_placingMode_texture,
                                  static_cast<float>(posX + m_placingMode_renderOffset.x()*d),
                                  static_cast<float>(posY + m_placingMode_renderOffset.y()),
                                  static_cast<float>(posW),
                                  static_cast<float>(posH),
                                  static_cast<float>(x.first),
                                  static_cast<float>(x.second));
        if(!m_placingMode_npc.layer.empty() &&
            SDL_strcasecmp(m_placingMode_npc.layer.c_str(), "Default") != 0)
        {
            FontManager::printText(m_placingMode_npc.layer,
                                   int(posX) + 28, int(posY) + 34);
        }
        break;
    }
}

void LevelScene::onMousePressed(SDL_MouseButtonEvent &mbevent)
{
    if(!m_placingMode) return;

    PGE_Point mousePos = GlRenderer::MapToScr(mbevent.x, mbevent.y);
    mousePos.setX(mousePos.x() - static_cast<int>(m_placingMode_drawSize.x()) / 2);
    mousePos.setY(mousePos.y() - static_cast<int>(m_placingMode_drawSize.y()) / 2);

    if(mbevent.button == SDL_BUTTON_LEFT)
        placeItemByMouse(mousePos.x(), mousePos.y());
    else if(mbevent.button == SDL_BUTTON_RIGHT)
        m_placingMode = false;
}

void LevelScene::onMouseMoved(SDL_MouseMotionEvent &mvevent)
{
    if(!m_placingMode) return;

    m_placingMode_renderAt = GlRenderer::MapToScr(mvevent.x, mvevent.y);
    m_placingMode_renderAt.setX(m_placingMode_renderAt.x() - m_placingMode_drawSize.x() / 2);
    m_placingMode_renderAt.setY(m_placingMode_renderAt.y() - m_placingMode_drawSize.y() / 2);
}

void LevelScene::onMouseReleased(SDL_MouseButtonEvent &)
{
    if(!m_placingMode) return;
}


void LevelScene::placeItemByMouse(int x, int y)
{
    if(!m_placingMode)
        return;

    for(size_t i = 0; i < m_cameras.size(); i++)
    {
        PGE_Rect camRect;
        camRect.setRect(0, m_cameras[i].h() * i, m_cameras[i].w(), m_cameras[i].h());
        if(camRect.collidePoint(x, y))
        {
            x += m_cameras[i].posX();
            y += m_cameras[i].posY();
            break;
        }
    }

    pLogDebug("Placed: %d in pos: %d x %d", m_placingMode_item_type, x, y);

    switch(m_placingMode_item_type)
    {
    case 0:
        m_placingMode_block.x = x;
        m_placingMode_block.y = y;
        spawnBlock(m_placingMode_block);
        break;

    case 1:
        m_placingMode_bgo.x = x;
        m_placingMode_bgo.y = y;
        spawnBGO(m_placingMode_bgo);
        break;

    case 2:
        m_placingMode_npc.x = x;
        m_placingMode_npc.y = y;
        spawnNPC(m_placingMode_npc, GENERATOR_APPEAR, SPAWN_UP);
        break;
    }
}
