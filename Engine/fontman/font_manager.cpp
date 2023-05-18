/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "font_manager.h"
#include "font_manager_private.h"
#include "raster_font.h"
#include "ttf_font.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <Logger/logger.h>
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>

#include <common_features/engine_resources.h>

#include <vector>

BaseFontEngine::~BaseFontEngine()
{}

//! Complete array of available raster fonts
static VPtrList<RasterFont> g_rasterFonts;
static VPtrList<TtfFont>    g_ttfFonts;
static VPtrList<BaseFontEngine*>    g_anyFonts;

//! Default raster font to render text
static RasterFont      *g_defaultRasterFont = nullptr;

//! Default raster font to render text
static TtfFont         *g_defaultTtfFont = nullptr;

//! Is font manager initialized
static bool             g_fontManagerIsInit = false;

typedef std::unordered_map<std::string, int> FontsHash;
//! Database of available fonts
static FontsHash        g_fontNameToId;

static bool             g_double_pixled = false;

static void registerFont(BaseFontEngine* font)
{
    g_anyFonts.push_back(font);
    g_fontNameToId.insert({font->getFontName(), g_anyFonts.size() - 1});
}

void *FontManager::getDefaultTtfFont()
{
    return reinterpret_cast<void*>(g_defaultTtfFont);
}

void FontManager::initBasic()
{
    if(!g_ft)
        initializeFreeType();

    unsigned char *memory = nullptr;
    size_t  fileSize = 0;
    bool ok = RES_getMem("fonts/UKIJTuzB.ttf", memory, fileSize);
    SDL_assert_release(ok);

    g_ttfFonts.emplace_back();
    TtfFont &mainFont = g_ttfFonts.back();

    ok = mainFont.loadFont(reinterpret_cast<const char*>(memory), fileSize);
    SDL_assert_release(ok);
    g_defaultTtfFont = &mainFont;
    registerFont(g_defaultTtfFont);

    g_double_pixled = false;
    g_fontManagerIsInit = true;
}

void FontManager::initFull()
{
    if(!g_ft)
        initializeFreeType();

    g_double_pixled = ConfigManager::setup_fonts.double_pixled;

    /***************Load raster font support****************/
    DirMan fontsDir(ConfigManager::config_dirSTD + "/fonts");

    std::vector<std::string> files;
    fontsDir.getListOfFiles(files, {".font.ini"});
    std::sort(files.begin(), files.end());
    for(std::string &fonFile : files)
    {
        g_rasterFonts.emplace_back();
        RasterFont& rf = g_rasterFonts.back();
        rf.loadFont(fontsDir.absolutePath() + "/" + fonFile);

        if(!rf.isLoaded())   //Pop broken font from array
            g_rasterFonts.pop_back();
        else   //Register font name in a table
            registerFont(&rf);
    }

    if(!g_rasterFonts.empty())
        g_defaultRasterFont = &g_rasterFonts.front();

    /***************Load TTF font support****************/
    if(!ConfigManager::setup_fonts.fontname.empty())
    {
        g_ttfFonts.emplace_back();
        TtfFont& tf = g_ttfFonts.back();
        tf.loadFont(fontsDir.absolutePath() + "/" + ConfigManager::setup_fonts.fontname);
        if(!tf.isLoaded())   //Pop broken font from array
            g_ttfFonts.pop_back();
        else   //Register font name in a table
        {
            registerFont(&tf);
            // Override default TTF font
            g_defaultTtfFont = &tf;
        }
    }

    for(std::string &fontFile : ConfigManager::setup_fonts.ttfFonts)
    {
        g_ttfFonts.emplace_back();
        TtfFont& tf = g_ttfFonts.back();
        tf.loadFont(fontsDir.absolutePath() + "/" + fontFile);
        if(!tf.isLoaded())   //Pop broken font from array
            g_ttfFonts.pop_back();
        else   //Register font name in a table
            registerFont(&tf);
    }
}

void FontManager::quit()
{
    g_fontNameToId.clear();
    g_anyFonts.clear();
    g_ttfFonts.clear();
    g_rasterFonts.clear();
    closeFreeType();
}


PGE_Size FontManager::textSize(std::string &text, int fontID,
                               uint32_t max_line_lenght,
                               bool cut,
                               uint32_t ttfFontSize)
{
    SDL_assert_release(g_fontManagerIsInit);// Font manager is not initialized!

    if(!g_fontManagerIsInit)
        return PGE_Size(27 * 20, static_cast<int>(std::count(text.begin(), text.end(), '\n') + 1) * 20);

    if(text.empty())
        return PGE_Size(0, 0);

    if(max_line_lenght <= 0)
        max_line_lenght = 1000;

    if(cut)
    {
        std::string::size_type i = text.find('\n');
        if(i != std::string::npos)
            text.erase(i, text.size() - i);
    }

    //Use one of loaded fonts
    if((fontID >= 0) && (static_cast<size_t>(fontID) < g_anyFonts.size()))
    {
        if(g_anyFonts[fontID]->isLoaded())
            return g_anyFonts[fontID]->textSize(text, max_line_lenght);
    }

    if(g_defaultTtfFont->isLoaded())
        return g_defaultTtfFont->textSize(text, max_line_lenght, false, ttfFontSize);

    return PGE_Size(27 * 20, static_cast<int>(std::count(text.begin(), text.end(), '\n') + 1) * 20);
}

int FontManager::getFontID(std::string fontName)
{
    FontsHash::iterator i = g_fontNameToId.find(fontName);
    if(i == g_fontNameToId.end())
        return DefaultRaster;
    else
        return i->second;
}



void FontManager::printText(std::string text,
                            int x, int y, int font,
                            float Red, float Green, float Blue, float Alpha,
                            uint32_t ttf_FontSize)
{
    if(!g_fontManagerIsInit)
        return;

    if(text.empty())
        return;

    if((font >= 0) && (static_cast<size_t>(font) < g_anyFonts.size()))
    {
        if(g_anyFonts[font]->isLoaded())
        {
            g_anyFonts[font]->printText(text, x, y, Red, Green, Blue, Alpha);
            return;
        }
    }

    switch(font)
    {
    case DefaultRaster:
        if(g_defaultRasterFont && g_defaultRasterFont->isLoaded())
        {
            g_defaultRasterFont->printText(text, x, y, Red, Green, Blue, Alpha, ttf_FontSize);
            break;
        } /*fallthrough*/
    case DefaultTTF_Font:
    default:
        if(g_defaultTtfFont->isLoaded())
            g_defaultTtfFont->printText(text, x, y, Red, Green, Blue, Alpha, ttf_FontSize);
    }
}


void FontManager::optimizeText(std::string &text, size_t max_line_lenght, int *numLines, int *numCols)
{
    /****************Word wrap*********************/
    size_t  lastspace = 0;
    int     count = 1;
    size_t  maxWidth = 0;

    for(size_t x = 0, i = 0; i < text.size(); i++, x++)
    {
        switch(text[i])
        {
        case '\t':
        case ' ':
            lastspace = i;
            break;

        case '\n':
            lastspace = 0;
            if((maxWidth < x) && (maxWidth < max_line_lenght))
                maxWidth = x;
            x = 0;
            count++;
            break;
        }

        if(x >= max_line_lenght) //If lenght more than allowed
        {
            maxWidth = x;

            if(lastspace > 0)
            {
                text[lastspace] = '\n';
                i = lastspace - 1;
                lastspace = 0;
            }
            else
            {
                text.insert(i, 1, '\n');
                x = 0;
                count++;
            }
        }

        UTF8 uch = static_cast<unsigned char>(text[i]);
        i += static_cast<size_t>(trailingBytesForUTF8[uch]);
    }

    if(count == 1)
        maxWidth = text.length();

    /****************Word wrap*end*****************/

    if(numLines)
        *numLines = count;

    if(numCols)
        *numCols = static_cast<int>(maxWidth);
}

std::string FontManager::cropText(std::string text, size_t max_symbols)
{
    if(max_symbols == 0)
        return text;

    size_t utf8len = utf8_substrlen(text, max_symbols);
    if(text.size() > utf8len)
    {
        text.erase(utf8len, text.size() - utf8len);
        text.append("...");
    }
    return text;
}

