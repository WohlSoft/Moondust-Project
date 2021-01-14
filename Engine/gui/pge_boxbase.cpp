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

#include "pge_boxbase.h"
#include <SDL2/SDL_opengl.h>
#include <common_features/graphics_funcs.h>
#include <Utils/maths.h>
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <common_features/pge_delay.h>

// Scenes
#include <data_configs/config_select_scene/scene_config_select.h>
#include <scenes/scene_level.h>
#include <scenes/scene_world.h>
#include <scenes/scene_gameover.h>
#include <scenes/scene_title.h>

PGE_BoxBase::PGE_BoxBase(Scene *parentScene)
{
    construct(parentScene);
}

PGE_BoxBase::PGE_BoxBase(const PGE_BoxBase &bb)
{
    m_keys =              bb.m_keys;

    m_page =            bb.m_page;
    m_running =         bb.m_running;

    m_fontID =          bb.m_fontID;
    m_fontRgba =        bb.m_fontRgba;
    m_borderWidth =     bb.m_borderWidth;

    m_bgColor =         bb.m_bgColor;

    m_type =            bb.m_type;
    m_sizeRect =        bb.m_sizeRect;

    m_width   =         bb.m_width;
    m_height  =         bb.m_height;
    m_padding =         bb.m_padding;

    m_faderOpacity =    bb.m_faderOpacity;
    m_targetOpacity =   bb.m_targetOpacity;
    m_fadeStep =        bb.m_fadeStep;
    m_fadeSpeed =       bb.m_fadeSpeed;
    m_manualTicks =     bb.m_manualTicks;
    m_parentScene =     bb.m_parentScene;
    m_uTickf =          bb.m_uTickf;
    m_uTick =           bb.m_uTick;
    m_textureUsed =     bb.m_textureUsed;
    m_styleTexture =    bb.m_styleTexture;
}

void PGE_BoxBase::construct(Scene *parentScene)
{
    updateTickValue();
    restart();

    m_fontID =          0;
    m_fontRgba =        GlColor(0xFFFFFFFF);
    m_borderWidth =     32;

    m_keys = Controller::noKeys();

    setType(msg_info);

    m_faderOpacity = 0.0;
    m_fadeStep = 0.02;
    m_targetOpacity = 0.0;
    m_fadeSpeed = 10;
    m_textureUsed = false;

    setParentScene(parentScene);
}

PGE_BoxBase::~PGE_BoxBase()
{
    if(m_textureUsed)
        GlRenderer::deleteTexture(m_styleTexture);
}

void PGE_BoxBase::setParentScene(Scene *parentScene)
{
    if(parentScene == nullptr)
        m_parentScene = PGE_Window::m_currentScene;// Use current scene by default
    else
        m_parentScene = parentScene;

    if(!m_parentScene)
        GlRenderer::setClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    initControllers();
}

void PGE_BoxBase::setType(PGE_BoxBase::msgType type)
{
    switch(type)
    {
    case msg_info:
        m_bgColor =       GlColor(0, 0, 0);
        break;
    case msg_info_light:
        m_bgColor =      GlColor(0, 0, 0.490196078);
        break;
    case msg_warn:
        m_bgColor =      GlColor(1.0, 0.62745098, 0.0);
        break;
    case msg_error:
        m_bgColor =      GlColor(0.490196078, 0, 0);
        break;
    case msg_fatal:
        m_bgColor =      GlColor(1.0, 0, 0);
        break;
    default:
        m_bgColor =      GlColor(0, 0, 0);
        break;
    }

    m_type = type;
}

void PGE_BoxBase::setBoxSize(double width, double height, double padding)
{
    m_width = width;
    m_height = height;
    m_padding = padding;
}

void PGE_BoxBase::processLoader(double tickDelay)
{
    updateControllers();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);
        if(event.type == SDL_QUIT)
            m_faderOpacity = 1.0;
    }

    tickFader(tickDelay);

    if(m_faderOpacity >= 1.0)
        nextPage();
}

void PGE_BoxBase::processBox(double tickDelay)
{
    (void)tickDelay;
    updateControllers();
}

void PGE_BoxBase::processUnLoader(double tickDelay)
{
    updateControllers();

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        PGE_Window::processEvents(event);
        if(event.type == SDL_QUIT)
            m_faderOpacity = 0.0;
    }

    tickFader(tickDelay);
    if(m_faderOpacity <= 0.0)
        nextPage();
}

void PGE_BoxBase::restart()
{
    m_running = true;
    m_page = PageStart;
    setFade(10, 1.0, 0.05);
}

void PGE_BoxBase::nextPage()
{
    m_page++;
}

bool PGE_BoxBase::isRunning()
{
    return m_running;
}

void PGE_BoxBase::exec()
{
    restart();

    while(m_running)
    {
        Uint32 start_render = SDL_GetTicks();

        update(m_uTickf);
        PGE_BoxBase::render();
        render();
        GlRenderer::flush();
        GlRenderer::repaint();

        if((!PGE_Window::vsync) && (m_uTick > static_cast<signed>(SDL_GetTicks() - start_render)))
        {
            Uint32 delay = static_cast<Uint32>(m_uTick) - (SDL_GetTicks() - start_render);
            SDL_assert(delay < 2000u);
            PGE_Delay(delay);
        }
    }
}

void PGE_BoxBase::update(double tickDelay)
{
    switch(m_page)
    {
    case PageStart:
        nextPage();
        break;
    case PageLoading:
        processLoader(tickDelay);
        break;
    case PageRunning:
        processBox(tickDelay);
        break;
    case PageUnLoading:
        processUnLoader(tickDelay);
        break;
    default:
    case PageClose:
        m_running = false;
        break;
    }
}

void PGE_BoxBase::render()
{
    if(m_parentScene)
        m_parentScene->render();
    else
        GlRenderer::clearScreen();
}



/**************************Fader*******************************/
void PGE_BoxBase::setFade(int speed, double target, double step)
{
    m_fadeStep = fabs(step);
    m_targetOpacity = target;
    m_fadeSpeed = speed;
    m_manualTicks = speed;
}

bool PGE_BoxBase::tickFader(double ticks)
{
    if(m_fadeSpeed < 1)
        return true; //Idling animation

    m_manualTicks -= std::max(ticks, 0.0);

    while(m_manualTicks <= 0.0)
    {
        fadeStep();
        m_manualTicks += m_fadeSpeed;
    }

    return Maths::equals(m_faderOpacity, m_targetOpacity);
}

void PGE_BoxBase::fadeStep()
{
    if(m_faderOpacity < m_targetOpacity)
        m_faderOpacity += m_fadeStep;
    else
        m_faderOpacity -= m_fadeStep;

    if(m_faderOpacity > 1.0)
        m_faderOpacity = 1.0;
    else if(m_faderOpacity < 0.0)
        m_faderOpacity = 0.0;
}
/**************************Fader**end**************************/

/********************************Texture************************************/
void PGE_BoxBase::loadTexture(std::string path)
{
    if(path.empty())
        return;

    if(m_textureUsed)
    {
        //remove old texture
        GlRenderer::deleteTexture(m_styleTexture);
    }

    GlRenderer::loadTextureP(m_styleTexture, path);

    if(m_styleTexture.texture > 0)
        m_textureUsed = true;
}

void PGE_BoxBase::updateTickValue()
{
    m_uTickf = PGE_Window::frameDelay;//1000.0f/(float)PGE_Window::TicksPerSecond;
    m_uTick = static_cast<int>(round(m_uTickf));

    if(m_uTick <= 0)
        m_uTick = 1;
    if(m_uTickf <= 0.0)
        m_uTickf = 1.0;
}

void PGE_BoxBase::drawTexture(int left, int top, int right, int bottom, int border, float opacity)
{
    PGE_Rect x;
    x.setLeft(left);
    x.setTop(top);
    x.setRight(right);
    x.setBottom(bottom);
    drawTexture(x, border, opacity);
}

void PGE_BoxBase::drawTexture(PGE_Rect _rect, int border, float opacity)
{
    if(m_textureUsed)
    {
        GlRenderer::BindTexture(&m_styleTexture);
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        int w = _rect.width();
        int h = _rect.height();
        int x, y, x2, y2, i, j;
        int hc, wc;
        x = border; // Width of one piece
        y = border; // Height of one piece
        //Double size
        x2 = x << 1;
        y2 = y << 1;
        int pWidth = m_styleTexture.w - x2; //Width of center piece
        int pHeight = m_styleTexture.h - y2; //Height of center piece
        int fLnt = 0; // Free Lenght
        int fWdt = 0; // Free Width
        int dX = 0; //Draw Offset. This need for crop junk on small sizes
        int dY = 0;

        if(w < x2) dX = (x2 - w) / 2;
        else dX = 0;

        if(h < y2) dY = (y2 - h) / 2;
        else dY = 0;

        int totalW = ((w - x2) / pWidth);
        int totalH = ((h - y2) / pHeight);

        //L Draw left border
        if(h > (y2))
        {
            hc = 0;

            for(i = 0; i < totalH; i++)
            {
                drawPiece(_rect, PGE_RectF(0, x + hc, x - dX, pHeight), PGE_RectF(0, y, x - dX, pHeight));
                hc += pHeight;
            }

            fLnt = (h - y2) % pHeight;

            if(fLnt != 0)
                drawPiece(_rect, PGE_RectF(0, x + hc, x - dX, fLnt), PGE_RectF(0, y, x - dX, fLnt));
        }

        //T Draw top border
        if(w > (x2))
        {
            hc = 0;

            for(i = 0; i < totalW; i++)
            {
                drawPiece(_rect, PGE_RectF(x + hc, 0, pWidth, y - dY), PGE_RectF(x, 0, pWidth, y - dY));
                hc += pWidth;
            }

            fLnt = (w - (x2)) % pWidth;

            if(fLnt != 0)
                drawPiece(_rect, PGE_RectF(x + hc, 0, fLnt, y - dY), PGE_RectF(x, 0, fLnt, y - dY));
        }

        //B Draw bottom border
        if(w > (x2))
        {
            hc = 0;

            for(i = 0; i < totalW; i++)
            {
                drawPiece(_rect, PGE_RectF(x + hc, h - y + dY, pWidth, y - dY), PGE_RectF(x, m_styleTexture.h - y + dY, pWidth, y - dY));
                hc += pWidth;
            }

            fLnt = (w - x2) % pWidth;

            if(fLnt != 0)
                drawPiece(_rect, PGE_RectF(x + hc, h - y + dY, fLnt, y - dY), PGE_RectF(x, m_styleTexture.h - y + dY, fLnt, y - dY));
        }

        //R Draw right border
        if(h > (y2))
        {
            hc = 0;

            for(i = 0; i < totalH; i++)
            {
                drawPiece(_rect, PGE_RectF(w - x + dX, y + hc, x - dX, pHeight), PGE_RectF(m_styleTexture.w - x + dX, y, x - dX, pHeight));
                hc += pHeight;
            }

            fLnt = (h - y2) % pHeight;

            if(fLnt != 0)
                drawPiece(_rect, PGE_RectF(w - x + dX, y + hc, x - dX, fLnt), PGE_RectF(m_styleTexture.w - x + dX, y, x - dX, fLnt));
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
                    drawPiece(_rect, PGE_RectF(x + hc, y + wc, pWidth, pHeight), PGE_RectF(x, y, pWidth, pHeight));
                    hc += pWidth;
                }

                fLnt = (w - x2) % pWidth;

                if(fLnt != 0)
                    drawPiece(_rect, PGE_RectF(x + hc, y + wc, fLnt, pHeight), PGE_RectF(x, y, fLnt, pHeight));

                wc += pHeight;
            }

            fWdt = (h - y2) % pHeight;

            if(fWdt != 0)
            {
                hc = 0;

                for(j = 0; j < totalW; j++)
                {
                    drawPiece(_rect, PGE_RectF(x + hc, y + wc, pWidth, fWdt), PGE_RectF(x, y, pWidth, fWdt));
                    hc += pWidth;
                }

                fLnt = (w - x2) % pWidth;

                if(fLnt != 0)
                    drawPiece(_rect, PGE_RectF(x + hc, y + wc, fLnt, fWdt), PGE_RectF(x, y, fLnt, fWdt));
            }
        }

        //Draw corners
        //1 Left-top
        drawPiece(_rect, PGE_RectF(0, 0, x - dX, y - dY), PGE_RectF(0, 0, x - dX, y - dY));
        //2 Right-top
        drawPiece(_rect, PGE_RectF(w - x + dX, 0, x - dX, y - dY), PGE_RectF(m_styleTexture.w - x + dX, 0, x - dX, y - dY));
        //3 Right-bottom
        drawPiece(_rect, PGE_RectF(w - x + dX, h - y + dY, x - dX, y - dY), PGE_RectF(m_styleTexture.w - x + dX, m_styleTexture.h - y + dY, x - dX, y - dY));
        //4 Left-bottom
        drawPiece(_rect, PGE_RectF(0, h - y + dY, x - dX, y - dY), PGE_RectF(0, m_styleTexture.h - y + dY, x - dX, y - dY));
        GlRenderer::UnBindTexture();
        //glDisable(GL_TEXTURE_2D);
    }
}

void PGE_BoxBase::drawPiece(PGE_RectF target, PGE_RectF block, PGE_RectF texture)
{
    PGE_RectF tx;
    tx.setLeft(texture.left() / this->m_styleTexture.w);
    tx.setRight(texture.right() / this->m_styleTexture.w);
    tx.setTop(texture.top() / this->m_styleTexture.h);
    tx.setBottom(texture.bottom() / this->m_styleTexture.h);
    GlRenderer::renderTextureCur(static_cast<float>(target.x() + block.x()),
                                 static_cast<float>(target.y() + block.y()),
                                 static_cast<float>(block.width()),
                                 static_cast<float>(block.height()),
                                 static_cast<float>(tx.top()),
                                 static_cast<float>(tx.bottom()),
                                 static_cast<float>(tx.left()),
                                 static_cast<float>(tx.right()));
}


void PGE_BoxBase::initControllers()
{
    m_ctrl1 = nullptr;
    m_ctrl2 = nullptr;

    if(m_parentScene != nullptr)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            auto s = dynamic_cast<LevelScene *>(m_parentScene);
            if(s)
            {
                m_ctrl1 = s->m_player1Controller;
                m_ctrl2 = s->m_player2Controller;
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            auto s = dynamic_cast<WorldScene *>(m_parentScene);
            if(s)
            {
                m_ctrl1 = s->m_player1Controller;
                m_ctrl2 = nullptr;
            }
        }
        else if(m_parentScene->type() == Scene::GameOver)
        {
            auto s = dynamic_cast<GameOverScene *>(m_parentScene);
            if(s)
            {
                m_ctrl1 = s->player1Controller;
                m_ctrl2 = nullptr;
            }
        }
        else if(m_parentScene->type() == Scene::ConfigSelect)
        {
            auto s = dynamic_cast<ConfigSelectScene *>(m_parentScene);
            if(s)
            {
                m_ctrl1 = s->controller;
                m_ctrl2 = nullptr;
            }
        }
        else if(m_parentScene->type() == Scene::Title)
        {
            auto s = dynamic_cast<TitleScene *>(m_parentScene);
            if(s)
            {
                m_ctrl1 = s->controller;
                m_ctrl2 = nullptr;
            }
        }
    }
}

void PGE_BoxBase::updateControllers()
{
    SDL_PumpEvents();

    if(!m_ctrl1)
        m_keys = Controller::noKeys();

    if(m_ctrl1)
    {
        m_ctrl1->update();
        m_ctrl1->sendControls();
        m_keys = m_ctrl1->keys;
    }

    if(m_ctrl2)
    {
        m_ctrl2->update();
        m_ctrl2->sendControls();
    }

    if(m_parentScene)
    {
        if(m_parentScene->type() == Scene::Level)
        {
            auto s = dynamic_cast<LevelScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
            }
        }
        else if(m_parentScene->type() == Scene::World)
        {
            auto s = dynamic_cast<WorldScene *>(m_parentScene);
            if(s)
            {
                s->tickAnimations(m_uTickf);
                s->m_fader.tickFader(m_uTickf);
            }
        }
        else if(m_parentScene->type() == Scene::Title)
        {
            auto s = dynamic_cast<TitleScene *>(m_parentScene);
            if(s)
                s->m_fader.tickFader(m_uTickf);
        }
    }
}
