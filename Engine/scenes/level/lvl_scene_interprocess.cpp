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

#include "../scene_level.h"

#include <data_configs/config_manager.h>
#include <networking/intproc.h>
#include <common_features/logger.h>
#include <audio/pge_audio.h>

void LevelScene::process_InterprocessCommands()
{
    //Recive external commands!
    if(IntProc::enabled && IntProc::hasCommand())
    {
        switch(IntProc::commandType())
        {
            case IntProc::MessageBox:
            {
                PGE_MsgBox msgBox = PGE_MsgBox(this, IntProc::getCMD(),
                                  PGE_MsgBox::msg_info, PGE_Point(-1, -1),
                                   ConfigManager::setup_message_box.box_padding,
                                   ConfigManager::setup_message_box.sprite);
                msgBox.exec(); break;
            }
            case IntProc::Cheat:
                break;
            case IntProc::PlaceItem:
            {
                QString raw = IntProc::getCMD();
                WriteToLog(QtDebugMsg, raw);
                LevelData got=FileFormats::ReadExtendedLvlFile(raw, ".");
                if(!got.ReadFileValid)
                {
                    WriteToLog(QtDebugMsg, FileFormats::errorString);
                    return;
                }

                //Don't show placing item sprite before mouse will be on screen
                placingMode_renderAt.setPoint(PGE_Window::Width+placingMode_drawSize.x()+16,
                                              PGE_Window::Height+placingMode_drawSize.y()+16);

                PGE_Audio::playSoundByRole(obj_sound_role::PlayerGrab2);

                if(raw.startsWith("BLOCK_PLACE", Qt::CaseInsensitive))
                {
                    if(got.blocks.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=0;
                    placingMode_block=got.blocks[0];
                    long tID = ConfigManager::getBlockTexture(placingMode_block.id);
                    if( tID  >= 0 )
                    {
                        placingMode_texture = ConfigManager::level_textures[tID];
                        obj_block &bl = ConfigManager::lvl_block_indexes[placingMode_block.id];
                        placingMode_animated = bl.animated;
                        placingMode_animatorID = bl.animator_ID;
                        placingMode_drawSize.setX(placingMode_texture.w);
                        placingMode_drawSize.setY(placingMode_texture.h/bl.frames);
                    }
                } else
                if(raw.startsWith("BGO_PLACE", Qt::CaseInsensitive))
                {
                    if(got.bgo.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=1;
                    placingMode_bgo=got.bgo[0];
                    long tID = ConfigManager::getBgoTexture(placingMode_bgo.id);
                    if( tID  >= 0 )
                    {
                        placingMode_texture = ConfigManager::level_textures[tID];
                        obj_bgo &bg=ConfigManager::lvl_bgo_indexes[placingMode_bgo.id];
                        placingMode_animated = bg.animated;
                        placingMode_animatorID = bg.animator_ID;
                        placingMode_drawSize.setX(placingMode_texture.w);
                        placingMode_drawSize.setY(placingMode_texture.h);
                    }
                } else
                if(raw.startsWith("NPC_PLACE", Qt::CaseInsensitive))
                {
                    if(got.npc.isEmpty()) break;
                    placingMode=true;
                    placingMode_item_type=2;
                    placingMode_npc=got.npc[0];
                    long tID = ConfigManager::getNpcTexture(placingMode_npc.id);
                    if( tID >= 0 )
                    {
                        placingMode_texture = ConfigManager::level_textures[tID];
                        obj_npc &np=ConfigManager::lvl_npc_indexes[placingMode_npc.id];
                        placingMode_animated = ((np.frames>1) || (np.framestyle>0));
                        if(np.animator_ID < 0)
                        {
                            if( (np.frames > 1) || (np.framestyle > 0) )
                            {
                                AdvNpcAnimator animator(placingMode_texture, np);
                                ConfigManager::Animator_NPC.push_back(animator);
                                ConfigManager::Animator_NPC.last().start();
                                np.animator_ID = ConfigManager::Animator_NPC.size()-1;
                            } else placingMode_animated=false;
                        }
                        placingMode_animatorID = np.animator_ID;
                        placingMode_drawSize.setX(np.gfx_w);
                        placingMode_drawSize.setY(np.gfx_h);

                        int d=placingMode_npc.direct; if(d==0) d=-1;
                        int imgOffsetX = -((int)round( - ( ( (double)np.gfx_w - (double)np.width ) / 2 ) )
                                            +(-((double)np.gfx_offset_x)*d));
                        int imgOffsetY = (int)round( - (double)np.gfx_h + (double)np.height + (double)np.gfx_offset_y);

                        placingMode_renderOffset.setPoint(imgOffsetX, imgOffsetY);
                    }
                }
                else PGE_Audio::playSoundByRole(obj_sound_role::WeaponExplosion);
                break;
            }
        }
    }
}

void LevelScene::drawPlacingItem()
{
    if(!placingMode) return;

    AniPos x(0,1);
    int d=0;
    switch(placingMode_item_type)
    {
        case 0:
            if(placingMode_animated) x = ConfigManager::Animator_Blocks[placingMode_animatorID].image();
            GlRenderer::renderTexture(&placingMode_texture,
                                      placingMode_renderAt.x(),
                                      placingMode_renderAt.y(),
                                      placingMode_drawSize.x(),
                                      placingMode_drawSize.y(),
                                      x.first, x.second);
            break;
        case 1:
            if(placingMode_animated) x = ConfigManager::Animator_BGO[placingMode_animatorID].image();
            GlRenderer::renderTexture(&placingMode_texture,
                                      placingMode_renderAt.x(),
                                      placingMode_renderAt.y(),
                                      placingMode_drawSize.x(),
                                      placingMode_drawSize.y(),
                                      x.first, x.second);
            break;
        case 2:
            if(placingMode_animated) x = ConfigManager::Animator_NPC[placingMode_animatorID].image(placingMode_npc.direct);
            d=placingMode_npc.direct; if(d==0) d=-1;
            GlRenderer::renderTexture(&placingMode_texture,
                                      placingMode_renderAt.x()+placingMode_renderOffset.x()*d,
                                      placingMode_renderAt.y()+placingMode_renderOffset.y(),
                                      placingMode_drawSize.x(),
                                      placingMode_drawSize.y(),
                                      x.first, x.second);
            break;
    }
}

void LevelScene::onMousePressed(SDL_MouseButtonEvent &mbevent)
{
    if(!placingMode) return;
    PGE_Point mousePos = GlRenderer::MapToScr(mbevent.x, mbevent.y);
    mousePos.setX(mousePos.x()-placingMode_drawSize.x()/2);
    mousePos.setY(mousePos.y()-placingMode_drawSize.y()/2);
    if( mbevent.button==SDL_BUTTON_LEFT ) {
        placeItemByMouse(mousePos.x(), mousePos.y());
    } else if( mbevent.button==SDL_BUTTON_RIGHT ) {
        placingMode=false;
    }
}

void LevelScene::onMouseMoved(SDL_MouseMotionEvent &mvevent)
{
    if(!placingMode) return;
    placingMode_renderAt = GlRenderer::MapToScr(mvevent.x, mvevent.y);
    placingMode_renderAt.setX(placingMode_renderAt.x()-placingMode_drawSize.x()/2);
    placingMode_renderAt.setY(placingMode_renderAt.y()-placingMode_drawSize.y()/2);
}

void LevelScene::onMouseReleased(SDL_MouseButtonEvent &)
{
    if(!placingMode) return;
}


void LevelScene::placeItemByMouse(int x, int y)
{
    if(!placingMode) return;
    for(int i=0;i<cameras.size(); i++)
    {
        PGE_Rect camRect;
        camRect.setRect(0, cameras[i].h()*i, cameras[i].w(), cameras[i].h());
        if(camRect.collidePoint(x, y))
        {
            x+=cameras[i].posX();
            y+=cameras[i].posY();
            break;
        }
    }

    qDebug()<<"Placed:" << placingMode_item_type<<" in pos:" << x << y;

    switch(placingMode_item_type)
    {
        case 0:
            placingMode_block.x=x;
            placingMode_block.y=y;
            spawnBlock(placingMode_block);
            break;
        case 1:
            placingMode_bgo.x=x;
            placingMode_bgo.y=y;
            spawnBGO(placingMode_bgo);
            break;
        case 2:
            placingMode_npc.x=x;
            placingMode_npc.y=y;
            spawnNPC(placingMode_npc, GENERATOR_APPEAR, SPAWN_UP);
            break;
    }
}

