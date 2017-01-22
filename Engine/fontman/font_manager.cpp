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
#include <common_features/pge_qt_application.h>
#include <common_features/graphics_funcs.h>
#include <common_features/logger.h>
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>
#include <IniProcessor/ini_processing.h>
#include <fmt/fmt_format.h>

#include <vector>

#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QImage>
#include <QPainter>
#include <QFontMetrics>
#include <QFontDatabase>

#include <common_features/graphics_funcs.h>

static int char2int(const QChar &ch)
{
    return ch.toLatin1() - '0';
}

/* //Currently unused, uncommend when it needed
static int char2int(const char& ch)
{
    return ch - '0';
}
*/

RasterFont::RasterFont() : first_line_only("\n.*")
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

RasterFont::RasterFont(const RasterFont &rf) : first_line_only("\n.*")
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

    int tables = 0;
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

    for(int i = 1; i <= tables; i++)
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
    PGE_Texture *loadedTexture = &textures.last();

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
                pLogDebug("=invalid-Y=%d=", x.c_str());
                continue;
            }
            begPos = endPos;
            endPos = x.find('-', begPos);
            if(endPos == std::string::npos)
                endPos = x.size();//Use entire string
            charPosY = x.substr(begPos, endPos);
            if(charPosY.empty())
            {
                pLogDebug("=invalid-Y=%d=", x.c_str());
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

        QString ucharX = QString::fromStdString(charX);
        QChar ch = ucharX[0];
        //qDebug()<<"=char=" << ch << "=id="<<charPosX.toInt()<<charPosY.toInt()<<"=";
        RasChar rch;
        rch.valid = true;
        rch.tx              =  loadedTexture;
        rch.l               =  std::stof(charPosY.c_str()) / matrix_width;
        rch.r               = (std::stof(charPosY.c_str()) + 1.0f) / matrix_width;
        rch.padding_left    = (ucharX.size() > 1) ? char2int(ucharX[1]) : 0;
        rch.padding_right   = (ucharX.size() > 2) ? char2int(ucharX[2]) : 0;
        rch.t               =  std::stof(charPosX.c_str()) / matrix_height;
        rch.b               = (std::stof(charPosX.c_str()) + 1.0f) / matrix_height;
        fontMap[ch] = rch;
    }

    font.endGroup();

    if(!fontMap.isEmpty())
        isReady = true;
}

PGE_Size RasterFont::textSize(QString &text, int max_line_lenght, bool cut)
{
    if(text.isEmpty())
        return PGE_Size(0, 0);

    int lastspace = 0; //!< index of last found space character
    int count = 1;  //!< Count of lines
    int maxWidth = 0; //!< detected maximal width of message
    int widthSumm = 0;
    int widthSummMax = 0;

    if(cut)
    {
        for(int i = 0; i < text.size(); i++)
        {
            if(text[i] == QChar(QChar::LineFeed))
            {
                text.remove(i, text.size() - i);
                break;
            }
        }
    }

    /****************Word wrap*********************/
    for(int x = 0, i = 0; i < text.size(); i++, x++)
    {
        switch(text[i].toLatin1())
        {
        case '\t':
        case ' ':
            lastspace = i;
            widthSumm += space_width + interletter_space / 2;

            if(widthSumm > widthSummMax) widthSummMax = widthSumm;

            break;

        case '\n':
            lastspace = 0;

            if((maxWidth < x) && (maxWidth < max_line_lenght)) maxWidth = x;

            x = 0;
            widthSumm = 0;
            count++;
            break;

        default:
            RasChar rch = fontMap[text[i]];

            if(rch.valid)
            {
                widthSumm += (letter_width - rch.padding_left - rch.padding_right + interletter_space);

                if(widthSumm > widthSummMax) widthSummMax = widthSumm;
            }
            else
            {
                widthSumm += (letter_width + interletter_space);

                if(widthSumm > widthSummMax) widthSummMax = widthSumm;
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
                text.insert(i, QChar('\n'));
                x = 0;
                count++;
            }
        }
    }

    if(count == 1)
        maxWidth = text.length();

    /****************Word wrap*end*****************/
    return PGE_Size(widthSummMax, newline_offset * count);
}

void RasterFont::printText(QString text, int x, int y, float Red, float Green, float Blue, float Alpha)
{
    if(fontMap.isEmpty()) return;

    if(text.isEmpty()) return;

    int offsetX = 0;
    int offsetY = 0;
    GLint w = letter_width;
    GLint h = letter_height;

    for(QChar &cx : text)
    {
        switch(cx.toLatin1())
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

        RasChar rch = fontMap[cx];

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
            PGE_Texture c;

            if(ttf_borders)
                FontManager::getChar2(cx, letter_width, c);
            else
                FontManager::getChar1(cx, letter_width, c);

            GlRenderer::setTextureColor(Red, Green, Blue, Alpha);
            GlRenderer::renderTexture(&c, x + offsetX, y + offsetY);
            offsetX += c.w + interletter_space;
        }
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








RasterFont         *FontManager::rFont = nullptr;
QList<RasterFont>   FontManager::rasterFonts;

bool FontManager::isInit = false;
QHash<FontManager::TTFCharType, PGE_Texture> FontManager::fontTable_1;
QHash<FontManager::TTFCharType, PGE_Texture> FontManager::fontTable_2;

FontManager::FontsHash FontManager::fonts;

int     FontManager::fontID;
QFont  *FontManager::defaultFont = nullptr;
bool    FontManager::double_pixled = false;

void FontManager::initBasic()
{
    if(!defaultFont)
        defaultFont = new QFont();

    fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    fontID = QFontDatabase::addApplicationFont(":/JosefinSans-Regular.ttf");
    double_pixled = false;
    //Josefin Sans
    QString family("Monospace");

    if(!QFontDatabase::applicationFontFamilies(fontID).isEmpty())
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
    isInit = true;
}

void FontManager::initFull()
{
    if(ConfigManager::setup_fonts.fontname.empty())
        fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    else
        fontID = QFontDatabase::addApplicationFont(
                     QString::fromStdString(ConfigManager::data_dirSTD + "/" +
                     ConfigManager::setup_fonts.fontname));

    double_pixled = ConfigManager::setup_fonts.double_pixled;
    /***************Load raster font support****************/
    DirMan fontsDir(ConfigManager::config_dirSTD + "/fonts");
    std::vector<std::string> files;
    //filter << "*.font.ini";
    fontsDir.getListOfFiles(files, {".font.ini"});

    for(std::string &fonFile : files)
    {
        RasterFont rfont;
        rasterFonts.push_back(rfont);
        rasterFonts.last().loadFont(fontsDir.absolutePath() + "/" + fonFile);

        if(!rasterFonts.last().isLoaded())   //Pop broken font from array
            rasterFonts.pop_back();
        else   //Register font name in a table
            fonts.insert({rasterFonts.last().getFontName(), rasterFonts.size() - 1});
    }

    if(!rasterFonts.isEmpty())
        rFont = &rasterFonts.first();
}

void FontManager::quit()
{
    //Clean font cache
    //glDisable(GL_TEXTURE_2D);
    QHash<TTFCharType, PGE_Texture>::iterator i;

    for(i = fontTable_1.begin(); i != fontTable_1.end(); ++i)
        GlRenderer::deleteTexture(i.value());

    for(i = fontTable_2.begin(); i != fontTable_2.end(); ++i)
        GlRenderer::deleteTexture(i.value());

    fontTable_1.clear();
    fontTable_2.clear();
    fonts.clear();
    rasterFonts.clear();

    if(defaultFont)
        delete defaultFont;
}

PGE_Size FontManager::textSize(QString &text, int fontID, int max_line_lenght, bool cut, int ttfFontPixelSize)
{
    if(!isInit) return PGE_Size(text.length() * 20, text.split(QChar(QChar::LineFeed)).size() * 20);

    if(text.isEmpty()) return PGE_Size(0, 0);

    if(max_line_lenght <= 0)
        max_line_lenght = 1000;

    if(cut)
    {
        for(int i = 0; i < text.size(); i++)
        {
            if(text[i] == QChar(QChar::LineFeed))
            {
                text.remove(i, text.size() - i);
                break;
            }
        }
    }

    //Use Raster font
    if((fontID >= 0) && (fontID < rasterFonts.size()))
    {
        if(rasterFonts[fontID].isLoaded())
            return rasterFonts[fontID].textSize(text, max_line_lenght);
    }

    //Use TTF font
    QFont fnt = font();

    if(ttfFontPixelSize > 0)
        fnt.setPixelSize(ttfFontPixelSize);

    QFontMetrics meter(fnt);
    optimizeText(text, max_line_lenght);
    QSize meterSize = meter.boundingRect(text).size();
    return PGE_Size(meterSize.width(), meterSize.height());
}

void FontManager::optimizeText(QString &text, int max_line_lenght, int *numLines, int *numCols)
{
    /****************Word wrap*********************/
    int lastspace = 0;
    int count = 1;
    int maxWidth = 0;

    for(int x = 0, i = 0; i < text.size(); i++, x++)
    {
        switch(text[i].toLatin1())
        {
        case '\t':
        case ' ':
            lastspace = i;
            break;

        case '\n':
            lastspace = 0;

            if((maxWidth < x) && (maxWidth < max_line_lenght)) maxWidth = x;

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
                text.insert(i, QChar('\n'));
                x = 0;
                count++;
            }
        }
    }

    if(count == 1)
        maxWidth = text.length();

    /****************Word wrap*end*****************/

    if(numLines)
        *numLines = count;

    if(numCols)
        *numCols = maxWidth;
}

QString FontManager::cropText(QString text, int max_symbols)
{
    if(max_symbols <= 0)
        return text;

    if(text.length() <= max_symbols)
        return text;

    int i = max_symbols - 1;
    text.remove(i, text.size() - i);
    text.append("...");
    return text;
}







void FontManager::printText(QString text, int x, int y, int font, float Red, float Green, float Blue, float Alpha, int ttf_FontSize)
{
    if(!isInit) return;

    if(text.isEmpty()) return;

    if((font >= 0) && (font < rasterFonts.size()))
    {
        if(rasterFonts[font].isLoaded())
        {
            rasterFonts[font].printText(text, x, y, Red, Green, Blue, Alpha);
            return;
        }
    }
    else if(font == DefaultTTF_Font)
    {
        int offsetX = 0;
        int offsetY = 0;
        int height = 32;
        int width = 32;
        GlRenderer::setTextureColor(Red, Green, Blue, Alpha);

        for(QChar &cx : text)
        {
            switch(cx.toLatin1())
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
}

void FontManager::printTextTTF(QString text, int x, int y, int pointSize, QRgb color)
{
    if(!isInit) return;

    QString(family);

    if(!QFontDatabase::applicationFontFamilies(fontID).isEmpty())
        family = QFontDatabase::applicationFontFamilies(fontID).at(0);

    QFont font(family);//font.setWeight(14);
    font.setPixelSize(pointSize);
    PGE_Texture tex;
    SDL_string_texture_create(font, color, text, &tex);
    GlRenderer::renderTexture(&tex, x, y);
    GlRenderer::deleteTexture(tex);
}


int FontManager::getFontID(QString fontName)
{
    return getFontID(fontName.toStdString());
}

int FontManager::getFontID(std::string fontName)
{
    FontsHash::iterator i = fonts.find(fontName);
    if(i == fonts.end())
        return DefaultRaster;
    else
        return i->second;
}

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
