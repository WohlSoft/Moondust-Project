/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <data_configs/config_manager.h>
#include <Utils/maths.h>
#include <graphics/graphics.h>
#include <graphics/gl_renderer.h>
#include <graphics/window.h>
#include <fmt/fmt_format.h>
#include <common_features/logger.h>

#include "lvl_backgrnd.h"

//This fixes a weak-vtable warning of CLang
LevelBackgroundBase::~LevelBackgroundBase() {}

#include "bg/bg_standard.h"
#include "bg/bg_multilayer.h"


LVL_Background::LVL_Background(const LVL_Background &_bg)
{
    m_isInitialized = _bg.m_isInitialized;
    m_color = _bg.m_color;
    m_box   = _bg.m_box;
    m_blankBackground = _bg.m_blankBackground;
    if(!m_blankBackground && _bg.m_setup)
    {
        obj_BG *setup = _bg.m_setup;
        setBg(*setup);
    }
    else
    {
        m_setup = _bg.m_setup;
    }
}

void LVL_Background::setBg(obj_BG &bg, LevelScene *scene)
{
    m_setup = &bg;
    m_bgId = bg.setup.id;

    // Set default fill color
    m_color = bg.fill_color;

    if(bg.setup.use_legacy_bg_engine)
        m_bg_base.reset(new StandardBackground);    //Legacy backgrounds model, kept for compatibility with legacy engine
    else
        m_bg_base.reset(new MultilayerBackground);  //Modern backgrounds model
    m_bg_base->init(*m_setup);
    m_bg_base->setScene(scene);

    m_blankBackground = false;
}

void LVL_Background::setBlank()
{
    m_bg_base.reset();
    m_blankBackground = true;
    m_bgId = 0;
    m_color.r = 0.0f;
    m_color.g = 0.0f;
    m_color.b = 0.0f;
}

void LVL_Background::setBox(const PGE_RectF &box)
{
    m_box = box;
}

void LVL_Background::drawBack(double x, double y, double w, double h)
{
    GlRenderer::renderRect(0.0f, 0.0f, static_cast<float>(w), static_cast<float>(h), m_color.r, m_color.g, m_color.b, 1.0f);
    if(m_blankBackground)
        return; //draw BG if is set
    if(m_bg_base)
        m_bg_base->renderBackground(m_box, x, y, w, h);
}

void LVL_Background::drawInScene(double x, double y, double w, double h)
{
    if(m_blankBackground)
        return; //draw BG if is set
    if(m_bg_base)
        m_bg_base->renderInScene(m_box, x, y, w, h);
}

void LVL_Background::drawFront(double x, double y, double w, double h)
{
    if(m_blankBackground)
        return; //draw BG if is set
    if(m_bg_base)
        m_bg_base->renderForeground(m_box, x, y, w, h);
}

void LVL_Background::process(double frameDelay)
{
    if(!m_blankBackground && m_bg_base)
        m_bg_base->process(frameDelay);
}

bool LVL_Background::isInit()
{
    return m_isInitialized;
}

unsigned long LVL_Background::curBgId()
{
    if(m_isInitialized)
        return 0;
    else
        return m_bgId;
}
