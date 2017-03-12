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

#include "font_manager.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <PGE_File_Formats/ConvertUTF.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>

#include <vector>

#include <common_features/graphics_funcs.h>

static int char2int(const char32_t &ch)
{
    return static_cast<int>(ch - U'0');
}

/* //Currently unused, uncommend when it needed
static int char2int(const char& ch)
{
    return ch - '0';
}
*/

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
static const char trailingBytesForUTF8[256] =
{
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
             0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/**
 * @brief Get single UTF32 character from firs character of UTF8 string
 * @param str pointer to position on UTF8 string where need to find a character
 * @return UTF32 character
 */
static inline char32_t get_utf8_char(const char *str)
{
    const UTF8 *source = reinterpret_cast<const UTF8*>(str);
    UTF32 ch = 0;
    uint16_t extraBytesToRead = static_cast<uint16_t>(trailingBytesForUTF8[*source]);

    switch (extraBytesToRead)
    {
        case 5: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 4: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 3: ch += *source++; ch <<= 6;
        case 2: ch += *source++; ch <<= 6;
        case 1: ch += *source++; ch <<= 6;
        case 0: ch += *source++;
    }
    ch -= offsetsFromUTF8[extraBytesToRead];
    return static_cast<char32_t>(ch);
}

std::u32string std_to_utf32(const std::string &src)
{
    std::u32string dst_tmp;
    dst_tmp.resize(src.size());
    const UTF8* src_begin = reinterpret_cast<const UTF8*>(src.c_str());
    const UTF8* src_end   = src_begin + src.size();
    UTF32*      dst_begin = reinterpret_cast<UTF32*>(&dst_tmp[0]);
    UTF32*      dst_end   = dst_begin + dst_tmp.size();
    PGEFF_ConvertUTF8toUTF32(&src_begin, src_end, &dst_begin, dst_end, strictConversion);
    return std::u32string(reinterpret_cast<char32_t*>(&dst_tmp[0]));
}

RasterFont::RasterFont()
{
    letter_width    = 0;
    letter_height   = 0;
    space_width     = 0;
    interletter_space = 0;
    newline_offset  = 0;
    matrix_width    = 0;
    matrix_height   = 0;
    isReady         = false;
    ttf_borders     = false;
    fontName        = fmt::format("font{0}", rand());
}

RasterFont::RasterFont(const RasterFont &rf)
{
    letter_width      = rf.letter_width;
    letter_height     = rf.letter_height;
    interletter_space = rf.interletter_space;
    space_width       = rf.space_width;
    newline_offset    = rf.newline_offset;
    matrix_width      = rf.matrix_width;
    matrix_height     = rf.matrix_height;
    isReady           = rf.isReady;
    ttf_borders       = rf.ttf_borders;
    fontMap           = rf.fontMap;
    textures          = rf.textures;
    fontName          = rf.fontName;
}

RasterFont::~RasterFont()
{
    for(PGE_Texture &t : textures)
        GlRenderer::deleteTexture(t);

    textures.clear();
}

void RasterFont::loadFont(std::string font_ini)
{
    if(!Files::fileExists(font_ini))
    {
        pLogWarning("Can't load font %s: file not exist", font_ini.c_str());
        return;
    }

    std::string root = DirMan(Files::dirname(font_ini)).absolutePath() + "/";
    IniProcessing font(font_ini);

    size_t tables = 0;
    font.beginGroup("font");
    font.read("tables", tables, 0);
    font.read("name", fontName, fontName);
    font.read("ttf-borders", ttf_borders, false);
    font.read("space-width", space_width, 0);
    font.read("interletter-space", interletter_space, 0);
    font.read("newline-offset", newline_offset, 0);
    font.endGroup();
    std::vector<std::string> tables_list;
    tables_list.reserve(tables);

    font.beginGroup("tables");

    for(size_t i = 1; i <= tables; i++)
    {
        std::string table;
        font.read(fmt::format("table{0}", i).c_str(), table, "");

        if(!table.empty())
            tables_list.push_back(table);
    }

    font.endGroup();

    for(std::string &tbl : tables_list)
        loadFontMap(root + tbl);
}

void RasterFont::loadFontMap(std::string fontmap_ini)
{
    std::string root = DirMan(Files::dirname(fontmap_ini)).absolutePath() + "/";

    if(!Files::fileExists(fontmap_ini))
    {
        pLogWarning("Can't load font map %s: file not exist", fontmap_ini.c_str());
        return;
    }

    IniProcessing font(fontmap_ini);
    std::string texFile;
    int w = letter_width, h = letter_height;
    font.beginGroup("font-map");
    font.read("texture", texFile, "");
    font.read("width", w, 0);
    font.read("height", h, 0);
    matrix_width = w;
    matrix_height = h;

    if((w <= 0) || (h <= 0))
    {
        pLogWarning("Wrong width and height values! %d x %d",  w, h);
        return;
    }

    if(!Files::fileExists(root + texFile))
    {
        pLogWarning("Failed to load font texture! file not exists: %s",
                    (root + texFile).c_str());
        return;
    }

    PGE_Texture fontTexture;
    GlRenderer::loadTextureP(fontTexture, root + texFile);

    if(!fontTexture.inited)
        pLogWarning("Failed to load font texture! Invalid image!");

    textures.push_back(fontTexture);
    PGE_Texture *loadedTexture = &textures.back();

    if((letter_width == 0) || (letter_height == 0))
    {
        letter_width    = fontTexture.w / w;
        letter_height   = fontTexture.h / h;

        if(space_width == 0)
            space_width = letter_width;

        if(newline_offset == 0)
            newline_offset = letter_height;
    }

    font.endGroup();
    font.beginGroup("entries");
    std::vector<std::string> entries = font.allKeys();

    //qDebug()<<entries;

    for(std::string &x : entries)
    {
        std::string charPosX = "0", charPosY = "0";

        std::string::size_type begPos = 0;
        std::string::size_type endPos = x.find('-', begPos);

        //QStringList tmp = x.split('-');
        if(endPos == std::string::npos)
            endPos = x.size();//Use entire string

        charPosX = x.substr(begPos, endPos);

        if(charPosX.empty())
        {
            pLogDebug("=invalid-X=%d=", x.c_str());
            continue;
        }

        if(matrix_width > 1)
        {
            if(endPos == x.size())
            {
                pLogWarning("=invalid-Y=%d= in the raster font map %s", x.c_str(), fontmap_ini.c_str());
                continue;
            }
            begPos = endPos + 1;//+1 to skip '-' divider
            endPos = x.find('-', begPos);
            if(endPos == std::string::npos)
                endPos = x.size();//Use entire string
            charPosY = x.substr(begPos, endPos);
            if(charPosY.empty())
            {
                pLogWarning("=invalid-Y=%d= in the raster font map %s", x.c_str(), fontmap_ini.c_str());
                continue;
            }
        }

        std::string charX;
        font.read(x.c_str(), charX, "");

        /*Format of entry: X23
         * X - UTF-8 Symbol
         * 2 - padding left [for non-mono fonts]
         * 3 - padding right [for non-mono fonts]
        */
        if(charX.empty())
            continue;

        std::u32string ucharX = std_to_utf32(charX);
        char32_t ch = ucharX[0];
        //qDebug()<<"=char=" << ch << "=id="<<charPosX.toInt()<<charPosY.toInt()<<"=";
        RasChar rch;
        #ifndef DEBUG_BUILD
        try
        {
        #endif
            rch.tx              =  loadedTexture;
            rch.l               =  std::stof(charPosY.c_str()) / matrix_width;
            rch.r               = (std::stof(charPosY.c_str()) + 1.0f) / matrix_width;
            rch.padding_left    = (ucharX.size() > 1) ? char2int(ucharX[1]) : 0;
            rch.padding_right   = (ucharX.size() > 2) ? char2int(ucharX[2]) : 0;
            rch.t               =  std::stof(charPosX.c_str()) / matrix_height;
            rch.b               = (std::stof(charPosX.c_str()) + 1.0f) / matrix_height;
            rch.valid = true;
        #ifndef DEBUG_BUILD
        }
        catch(std::exception &e)
        {
            pLogWarning("Invalid entry of font map: entry: %s, reason: %s, file: %s", x.c_str(), e.what(), fontmap_ini.c_str());
        }
        #endif

        fontMap[ch] = rch;
    }

    font.endGroup();

    if(!fontMap.empty())
        isReady = true;
}

PGE_Size RasterFont::textSize(std::string &text, int max_line_lenght, bool cut)
{
    if(text.empty())
        return PGE_Size(0, 0);

    size_t lastspace = 0; //!< index of last found space character
    size_t count = 1;  //!< Count of lines
    int maxWidth = 0; //!< detected maximal width of message
    int widthSumm = 0;
    int widthSummMax = 0;

    //unsigned short extraBytesToRead = trailingBytesForUTF8[*source];

    if(cut)
    {
        for(size_t i = 0; i < text.size(); i++)
        {
            if(text[i] == '\n')
            {
                text.erase(i, text.size() - i);
                break;
            }
        }
    }

    /****************Word wrap*********************/
    int x = 0;
    for(size_t i = 0; i < text.size(); i++, x++)
    {
        char &cx = text[i];
        UTF8 uch = static_cast<unsigned char>(cx);
        switch(cx)
        {
        case '\t':
        case ' ':
            lastspace = i;
            widthSumm += space_width + interletter_space / 2;
            if(widthSumm > widthSummMax)
                widthSummMax = widthSumm;
            break;

        case '\n':
            lastspace = 0;
            if((maxWidth < x) && (maxWidth < max_line_lenght)) maxWidth = x;
            x = 0;
            widthSumm = 0;
            count++;
            break;

        default:
            RasChar rch = fontMap[get_utf8_char(&cx)];
            if(rch.valid)
            {
                widthSumm += (letter_width - rch.padding_left - rch.padding_right + interletter_space);
                if(widthSumm > widthSummMax)
                    widthSummMax = widthSumm;
            }
            else
            {
                widthSumm += (letter_width + interletter_space);
                if(widthSumm > widthSummMax)
                    widthSummMax = widthSumm;
            }

            break;
        }

        if((max_line_lenght > 0) && (x >= max_line_lenght)) //If lenght more than allowed
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
        i += static_cast<size_t>(trailingBytesForUTF8[uch]);
    }

    if(count == 1)
        maxWidth = static_cast<int>(text.length());

    /****************Word wrap*end*****************/
    return PGE_Size(widthSummMax, newline_offset * static_cast<int>(count));
}

void RasterFont::printText(const std::string &text, int x, int y, float Red, float Green, float Blue, float Alpha)
{
    if(fontMap.empty())
        return;

    if(text.empty())
        return;

    int offsetX = 0;
    int offsetY = 0;
    GLint w = letter_width;
    GLint h = letter_height;

    const char *strIt  = text.c_str();
    const char *strEnd = strIt + text.size();
    for(; strIt < strEnd; strIt++)
    {
        const char &cx = *strIt;
        UTF8 ucx = static_cast<unsigned char>(cx);

        switch(cx)
        {
        case '\n':
            offsetX = 0;
            offsetY += newline_offset;
            continue;

        case '\t':
            offsetX += offsetX + offsetX % w;
            continue;

        case ' ':
            offsetX += space_width + interletter_space / 2;
            continue;
        }

        RasChar rch = fontMap[get_utf8_char(strIt)];
        if(rch.valid)
        {
            GlRenderer::BindTexture(rch.tx);
            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTextureCur(x + offsetX - rch.padding_left,
                                         y + offsetY,
                                         w,
                                         h,
                                         rch.t,
                                         rch.b,
                                         rch.l,
                                         rch.r);
            offsetX += w - rch.padding_left - rch.padding_right + interletter_space;
        }
        else
        {
            #ifdef PGE_TTF
            PGE_Texture c;
            if(ttf_borders)
                FontManager::getChar2(cx, letter_width, c);
            else
                FontManager::getChar1(cx, letter_width, c);
            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTexture(&c, x + offsetX, y + offsetY);
            offsetX += c.w + interletter_space;
            #else
            offsetX += interletter_space;
            #endif
        }
        strIt += static_cast<size_t>(trailingBytesForUTF8[ucx]);
    }

    GlRenderer::UnBindTexture();
}

bool RasterFont::isLoaded()
{
    return isReady;
}

std::string RasterFont::getFontName()
{
    return fontName;
}





RasterFont                 *FontManager::rFont = nullptr;
VPtrList<RasterFont>        FontManager::rasterFonts;

bool FontManager::isInit = false;
#ifdef PGE_TTF
std::unordered_map<FontManager::TTFCharType, PGE_Texture> FontManager::fontTable_1;
std::unordered_map<FontManager::TTFCharType, PGE_Texture> FontManager::fontTable_2;
#endif

FontManager::FontsHash FontManager::fonts;

int     FontManager::fontID;
#ifdef PGE_TTF
QFont  *FontManager::defaultFont = nullptr;
#endif
bool    FontManager::double_pixled = false;

void FontManager::initBasic()
{
    #ifdef PGE_TTF
    if(!defaultFont)
        defaultFont = new QFont();

    fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    fontID = QFontDatabase::addApplicationFont(":/JosefinSans-Regular.ttf");
    #endif
    double_pixled = false;

    #ifdef PGE_TTF
    //Josefin Sans
    QString family("Monospace");

    if(!QFontDatabase::applicationFontFamilies(fontID).empty())
        family = QFontDatabase::applicationFontFamilies(fontID).at(0);

    defaultFont->setFamily(family);//font.setWeight(14);
    //#ifdef __APPLE__
    //  defaultFont->setPixelSize(16);
    //#else
    defaultFont->setPixelSize(16);
    //#endif
    defaultFont->setStyleStrategy(QFont::PreferBitmap);
    defaultFont->setLetterSpacing(QFont::AbsoluteSpacing, 1);
    //defaultFont = buildFont_RW(":/PressStart2P.ttf", 14);
    #endif

    isInit = true;
}

void FontManager::initFull()
{
    #ifdef PGE_TTF
    if(ConfigManager::setup_fonts.fontname.empty())
        fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    else
        fontID = QFontDatabase::addApplicationFont(
                     QString::fromStdString(ConfigManager::data_dirSTD + "/" +
                     ConfigManager::setup_fonts.fontname));
    #endif

    double_pixled = ConfigManager::setup_fonts.double_pixled;
    /***************Load raster font support****************/
    DirMan fontsDir(ConfigManager::config_dirSTD + "/fonts");
    std::vector<std::string> files;
    //filter << "*.font.ini";
    fontsDir.getListOfFiles(files, {".font.ini"});
    std::sort(files.begin(), files.end());

    for(std::string &fonFile : files)
    {
        RasterFont rfont;
        rasterFonts.push_back(rfont);
        {
            RasterFont& rf = rasterFonts.back();
            rf.loadFont(fontsDir.absolutePath() + "/" + fonFile);

            if(!rf.isLoaded())   //Pop broken font from array
                rasterFonts.pop_back();
            else   //Register font name in a table
                fonts.insert({rf.getFontName(), rasterFonts.size() - 1});
        }
    }

    if(!rasterFonts.empty())
        rFont = &rasterFonts.front();
}

void FontManager::quit()
{
    //Clean font cache
    //glDisable(GL_TEXTURE_2D);
    #ifdef PGE_TTF
    std::unordered_map<TTFCharType, PGE_Texture>::iterator i;

    for(i = fontTable_1.begin(); i != fontTable_1.end(); ++i)
        GlRenderer::deleteTexture(i.value());

    for(i = fontTable_2.begin(); i != fontTable_2.end(); ++i)
        GlRenderer::deleteTexture(i.value());

    fontTable_1.clear();
    fontTable_2.clear();
    #endif

    fonts.clear();
    rasterFonts.clear();

    #ifdef PGE_TTF
    if(defaultFont)
        delete defaultFont;
#endif
}

size_t FontManager::utf8_strlen(const char *str)
{
    size_t size = 0;
    while(str)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*str)]);
        size += charLen;
        str  += charLen;
    }
    return size;
}

size_t FontManager::utf8_strlen(const char *str, size_t len)
{
    size_t utf8_pos = 0;
    while(str && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*str)]);
        str  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return utf8_pos;
}

size_t FontManager::utf8_strlen(const std::string &str)
{
    return utf8_strlen(str.c_str(), str.size());
}

size_t FontManager::utf8_substrlen(const std::string &str, size_t utf8len)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char *cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == utf8len)
            return pos;
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return pos;
}

std::string FontManager::utf8_substr(const std::string &str, size_t utf8_begin, size_t utf8_len)
{
    size_t utf8_pos = 0;
    const char *cstr = str.c_str();
    size_t len = str.length();
    std::string out;
    while(cstr && (len > 0) && (utf8_len > 0))
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos >= utf8_begin)
        {
            out.append(cstr, charLen);
            utf8_len--;
        }
        cstr += charLen;
        len  -= charLen;
        utf8_pos++;
    }
    return out;
}

void FontManager::utf8_pop_back(std::string &str)
{
    size_t pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        len  -= charLen;
        if(len == 0)
        {
            str.erase(str.begin() + static_cast<std::string::difference_type>(pos), str.end());
            return;
        }
        cstr += charLen;
        pos  += charLen;
    }
}

void FontManager::utf8_erase_at(std::string &str, size_t begin)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == begin)
        {
            str.erase(str.begin() + static_cast<std::string::difference_type>(pos), str.end());
            return;
        }
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
}

void FontManager::utf8_erase_before(std::string &str, size_t end)
{
    size_t pos = 0;
    size_t utf8_pos = 0;
    const char*cstr = str.c_str();
    size_t len = str.length();
    while(cstr && len > 0)
    {
        size_t  charLen = 1 + static_cast<size_t>(trailingBytesForUTF8[static_cast<UTF8>(*cstr)]);
        if(utf8_pos == end)
        {
            str.erase(str.begin(), str.begin() + static_cast<std::string::difference_type>(pos) + 1);
            return;
        }
        cstr += charLen;
        pos  += charLen;
        len  -= charLen;
        utf8_pos++;
    }
}

PGE_Size FontManager::textSize(std::string &text, int fontID, int max_line_lenght, bool cut, int ttfFontPixelSize)
{
    assert(isInit && "Font manager is not initialized!");

    if(!isInit)
        return PGE_Size(27 * 20, (std::count(text.begin(), text.end(), '\n') + 1) * 20);

    if(text.empty())
        return PGE_Size(0, 0);

    if(max_line_lenght <= 0)
        max_line_lenght = 1000;

    if(cut)
    {
        for(size_t i = 0; i < text.size(); i++)
        {
            if(text[i] == '\n')
            {
                text.erase(i, text.size() - i);
                break;
            }
        }
    }

    //Use Raster font
    if((fontID >= 0) && (static_cast<size_t>(fontID) < rasterFonts.size()))
    {
        if(rasterFonts[fontID].isLoaded())
            return rasterFonts[fontID].textSize(text, max_line_lenght);
    }

    #ifdef PGE_TTF
    //Use TTF font
    QFont fnt = font();

    if(ttfFontPixelSize > 0)
        fnt.setPixelSize(ttfFontPixelSize);

    QFontMetrics meter(fnt);
    QString qtext = QString::fromStdString(text);
    optimizeText(qtext, max_line_lenght);
    QSize meterSize = meter.boundingRect(qtext).size();
    return PGE_Size(meterSize.width(), meterSize.height());
    #else
    //assert(false && "TTF FONTS SUPPORT IS DISABLED!");
    (void)ttfFontPixelSize;
    return PGE_Size(27 * 20, (std::count(text.begin(), text.end(), '\n') + 1) * 20);
    #endif
}

void FontManager::optimizeText(std::string &text, size_t max_line_lenght, int *numLines, int *numCols)
{
    /****************Word wrap*********************/
    size_t lastspace = 0;
    int count = 1;
    size_t maxWidth = 0;

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
        *numCols = maxWidth;
}

std::string FontManager::cropText(std::string text, size_t max_symbols)
{
    if(max_symbols <= 0)
        return text;

    size_t utf8len = 0;
    for(std::string::size_type i = 0; i < text.size(); i++)
    {
        if(utf8len >= max_symbols)
        {
            text.erase(i, text.size() - i);
            text.append("...");
            break;
        }
        UTF8 uch = static_cast<UTF8>(text[i]);
        i += static_cast<size_t>(trailingBytesForUTF8[uch]);
        utf8len++;
    }

    return text;
}







void FontManager::printText(std::string text, int x, int y, int font, float Red, float Green, float Blue, float Alpha, int ttf_FontSize)
{
    if(!isInit) return;

    if(text.empty())
        return;

    if((font >= 0) && (static_cast<size_t>(font) < rasterFonts.size()))
    {
        if(rasterFonts[font].isLoaded())
        {
            rasterFonts[font].printText(text, x, y, Red, Green, Blue, Alpha);
            return;
        }
    }
    #ifdef PGE_TTF
    else
    if(font == DefaultTTF_Font)
    {
        int offsetX = 0;
        int offsetY = 0;
        int height = 32;
        int width = 32;
        GlRenderer::setTextureColor(Red, Green, Blue, Alpha);

        for(char &cx : text)
        {
            switch(cx)
            {
            case '\n':
                offsetX = 0;
                offsetY += height;
                continue;

            case '\t':
                offsetX += offsetX + offsetX % width;
                continue;
            }

            PGE_Texture c;
            getChar2(cx, ttf_FontSize, c);
            GlRenderer::renderTexture(&c, x + offsetX, y + offsetY);
            width = c.w;
            height = c.h;
            offsetX += c.w;
        }

        return;
    }
    else
        printTextTTF(text, x, y, ttf_FontSize,
                     qRgba(static_cast<int>(Red * 255.0f),
                           static_cast<int>(Green * 255.0f),
                           static_cast<int>(Blue * 255.0f),
                           static_cast<int>(Alpha * 255.0f)));
    #else
    else
    {
        (void)ttf_FontSize;
        //assert(false && "TTF FONTS SUPPORT IS DISABLED!");
    }
    #endif
}

#ifdef PGE_TTF
void FontManager::printTextTTF(std::string text, int x, int y, int pointSize, QRgb color)
{
    assert(isInit);
    if(!isInit)
        return;
    #ifdef PGE_TTF
    QString(family);

    if(!QFontDatabase::applicationFontFamilies(fontID).empty())
        family = QFontDatabase::applicationFontFamilies(fontID).at(0);

    QFont font(family);//font.setWeight(14);
    font.setPixelSize(pointSize);
    PGE_Texture tex;
    SDL_string_texture_create(font, color, QString::fromStdString(text), &tex);
    GlRenderer::renderTexture(&tex, x, y);
    GlRenderer::deleteTexture(tex);
    #else
    assert(false && "TTF SUPPORT IS DISABLED!");
    #endif
}

int FontManager::getFontID(QString fontName)
{
    return getFontID(fontName.toStdString());
}
#endif

int FontManager::getFontID(std::string fontName)
{
    FontsHash::iterator i = fonts.find(fontName);
    if(i == fonts.end())
        return DefaultRaster;
    else
        return i->second;
}

#ifdef PGE_TTF

void FontManager::getChar1(QChar _x, int px_size, PGE_Texture &tex)
{
    TTFCharType cht = {_x, px_size};
    PGE_Texture &c = fontTable_1[cht];

    if(c.inited)
        tex = c;
    else
    {
        if(!isInit)
            return;

        if(tex.inited)
            GlRenderer::deleteTexture(tex);

        QImage text_image;
        QFont font_i = *defaultFont;
        font_i.setPixelSize(px_size);
        QFontMetrics meter(font_i);
        text_image = QImage(meter.width(_x), meter.height(), QImage::Format_ARGB32);
        text_image.fill(Qt::transparent);
        QPainter x(&text_image);
        x.setFont(font_i);
        x.setRenderHint(QPainter::Antialiasing, false);
        x.setBrush(QBrush(Qt::white));
        x.setPen(QPen(Qt::white));
        QString s;
        s.append(_x);
        x.drawText(text_image.rect(), s);
        x.end();
        GlRenderer::QImage2Texture(&text_image, c);
        tex = c;
    }
}


void FontManager::getChar2(QChar _x, int px_size, PGE_Texture &tex)
{
    TTFCharType cht = {_x, px_size};
    PGE_Texture &c = fontTable_2[cht];

    if(c.inited)
        tex = c;
    else
    {
        if(!isInit)
            return;

        if(tex.inited)
            GlRenderer::deleteTexture(tex);

        QImage text_image;
        int off = 4;
        QFont font_i = *defaultFont;
        font_i.setPixelSize(px_size);
        QFontMetrics meter(*defaultFont);
        QPainterPath path;
        text_image = QImage(meter.width(_x) + off, meter.height() + off * 4, QImage::Format_ARGB32);
        text_image.fill(Qt::transparent);
        QPainter x(&text_image);
        x.setFont(font_i);
        x.setRenderHint(QPainter::Antialiasing, false);
        x.setBrush(QBrush(Qt::white));
        x.setPen(QPen(Qt::white));
        path.addText(off, meter.height() + off, font_i, QString(_x));
        x.strokePath(path, QPen(QColor(Qt::black), off));
        x.fillPath(path, QBrush(Qt::white));
        x.end();
        //if(double_pixled)
        //{
        //    //Pixelizing
        //    text_image = text_image.scaled(text_image.width()/2, text_image.height()/2);
        //    text_image = text_image.scaled(text_image.width()*2, text_image.height()*2);
        //}
        GlRenderer::QImage2Texture(&text_image, c);
        //return c.texture;
        tex = c;
    }
}


QFont FontManager::font()
{
    return *defaultFont;
}




void FontManager::SDL_string_texture_create(QFont &font, QRgb color, QString &text, PGE_Texture *texture, bool borders)
{
    if(!isInit) return;

    int off = (borders ? 4 : 0);
    QPainterPath path;
    QFontMetrics meter(font);
    QImage text_image = QImage(meter.width(text) * 2 + off, (off + meter.height()) * text.split('\n').size(), QImage::Format_ARGB32);
    text_image.fill(Qt::transparent);
    QPainter x(&text_image);
    QFont font_i = font;

    if(borders)
        font_i.setPixelSize(font_i.pixelSize() - 1);

    x.setFont(font_i);
    x.setBrush(QBrush(color));
    x.setPen(QPen(QBrush(color), 1, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

    if(borders)
    {
        path.addText(off, meter.height() + off, font_i, text);
        x.strokePath(path, QPen(QColor(Qt::black), 2));
        x.fillPath(path, QBrush(color));
    }
    else
        x.drawText(text_image.rect(), text);

    x.end();
    GlRenderer::QImage2Texture(&text_image, *texture);
}

void FontManager::SDL_string_texture_create(QFont &font, PGE_Rect limitRect, int fontFlags, QRgb color, QString &text, PGE_Texture *texture, bool borders)
{
    if(!isInit) return;

    QImage text_image;
    int off = (borders ? 4 : 0);
    QPainterPath path;
    text_image = QImage(QSize(limitRect.width(), limitRect.height()), QImage::Format_ARGB32);
    text_image.fill(Qt::transparent);
    QFont font_i = font;

    if(borders)
        font_i.setPixelSize(font_i.pixelSize() - 1);

    QFontMetrics meter(font_i);
    QPainter x(&text_image);
    x.setFont(font_i);
    x.setBrush(QBrush(color));
    x.setPen(QPen(color));

    if(borders)
    {
        path.addText(off, meter.height() + off * 2, font_i, text);
        x.strokePath(path, QPen(QColor(Qt::black), off));
        x.fillPath(path, QBrush(color));
    }
    else
        x.drawText(text_image.rect(), fontFlags, text);

    x.end();

    if(double_pixled)
    {
        //Pixelizing
        text_image = text_image.scaled(text_image.width() / 2, text_image.height() / 2);
        text_image = text_image.scaled(text_image.width() * 2, text_image.height() * 2);
    }

    GlRenderer::QImage2Texture(&text_image, *texture);
}

#endif
