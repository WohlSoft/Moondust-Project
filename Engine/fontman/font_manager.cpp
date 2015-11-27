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

#include "font_manager.h"

#include <common_features/app_path.h>
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <QtDebug>
#include <QFile>

#include <QImage>
#include <QPainter>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QFileInfo>
#include <QDir>

#include <common_features/graphics_funcs.h>

#include <SDL2/SDL.h>
#undef main



RasterFont::RasterFont() : first_line_only("\n.*")
{
    letter_width=0;
    letter_height=0;

    space_width=0;
    interletter_space=0;
    newline_offset=0;

    matrix_width=0;
    matrix_height=0;
    isReady=false;
    ttf_borders=false;
    fontName=QString("font%1").arg(rand());
}

RasterFont::RasterFont(const RasterFont &rf) : first_line_only("\n.*")
{
    this->letter_width=rf.letter_width;
    this->letter_height=rf.letter_height;
    this->interletter_space=rf.interletter_space;
    this->space_width=rf.space_width;
    this->newline_offset=rf.newline_offset;
    this->matrix_width=rf.matrix_width;
    this->matrix_height=rf.matrix_height;
    this->isReady=rf.isReady;
    this->ttf_borders=rf.ttf_borders;

    this->fontMap = rf.fontMap;
    this->textures = rf.textures;
    this->fontName=rf.fontName;
}

RasterFont::~RasterFont()
{
    while(!textures.isEmpty())
    {
        GlRenderer::deleteTexture(textures.last() );
        textures.pop_back();
    }
}

void RasterFont::loadFont(QString font_ini)
{
    QFileInfo fm_ini(font_ini);
    if(!fm_ini.exists())
    {
        qWarning() <<"Can't load font "<<font_ini<<": file not exist";
        return;
    }
    QString root=fm_ini.absoluteDir().absolutePath()+"/";
    QSettings font(font_ini, QSettings::IniFormat);
    font.setIniCodec("UTF-8");

    int tables=0;
    font.beginGroup("font");
    tables=font.value("tables", 0).toInt();
    fontName=font.value("name", fontName).toString();
    ttf_borders=font.value("ttf-borders", false).toBool();
    space_width=font.value("space-width", 0).toInt();
    interletter_space=font.value("interletter-space", 0).toInt();
    newline_offset=font.value("newline-offset", 0).toInt();
    font.endGroup();

    QStringList tables_list;
    font.beginGroup("tables");
    for(int i=1;i<=tables;i++)
    {
        QString table=font.value(QString("table%1").arg(i), "").toString();
        if(!table.isEmpty())
            tables_list.append(table);
    }
    font.endGroup();

    foreach(QString tbl, tables_list)
    {
        loadFontMap(root+tbl);
    }
}

void RasterFont::loadFontMap(QString fontmap_ini)
{
    QFileInfo fm_ini(fontmap_ini);
    QString root=fm_ini.absoluteDir().absolutePath()+"/";
    if(!fm_ini.exists())
    {
        qWarning() <<"Can't load font map "<<fontmap_ini<<": file not exist";
        return;
    }
    QSettings font(fontmap_ini, QSettings::IniFormat);
    font.setIniCodec("UTF-8");
    QString texFile;
    int w=letter_width, h=letter_height;
    font.beginGroup("font-map");
    texFile=font.value("texture", "").toString();
    w=font.value("width", 0).toInt();
    h=font.value("height", 0).toInt();
    matrix_width=w;
    matrix_height=h;
    if((w<=0)||(h<=0))
    {
        qWarning() <<"Wrong width and height values ! "<<w<<h;
        return;
    }


    if(!QFileInfo(root+texFile).exists())
        return;
    PGE_Texture fontTexture;
    GlRenderer::loadTextureP(fontTexture, root+texFile);
    textures.push_back(fontTexture);

    if((letter_width==0)||(letter_height==0))
    {
        letter_width=fontTexture.w/w;
        letter_height=fontTexture.h/h;
        if(space_width==0)
            space_width=letter_width;
        if(newline_offset==0)
            newline_offset=letter_height;
    }
    font.endGroup();

    font.beginGroup("entries");
    QStringList entries=font.allKeys();

    //qDebug()<<entries;

    foreach(QString x, entries)
    {
        bool okX=false;
        bool okY=false;
        x=x.trimmed();
        QString charPosX="0", charPosY="0";
        QStringList tmp=x.split('-');
        if(tmp.isEmpty()) continue;
        charPosX=tmp[0];
        charPosX.toInt(&okX);
        if(!okX)
        {
            qDebug()<<"=invalid-X=" <<x<<"=";
            continue;
        }

        if(matrix_width>1)
        {
            if(tmp.size()<2) continue;
            charPosY=tmp[1];
            charPosY.toInt(&okY);
            if(!okY)
            {
                qDebug()<<"=invalid-Y=" <<x<<"=";
                continue;
            }
        }

        QString charX = font.value(x, "").toString();
        /*Format of entry: X23
         * X - UTF-8 Symbol
         * 2 - padding left [for non-mono fonts]
         * 3 - padding right [for non-mono fonts]
        */
        if(charX.isEmpty()) continue;
        QChar ch=charX[0];
        //qDebug()<<"=char=" << ch << "=id="<<charPosX.toInt()<<charPosY.toInt()<<"=";

        RasChar rch;
        rch.valid=true;
        rch.tx = fontTexture.texture;
        rch.l = charPosY.toFloat(&okY)/matrix_width;
        rch.r = (charPosY.toFloat(&okY)+1.0)/matrix_width;
        rch.padding_left=(charX.size()>1)? QString(charX[1]).toInt():0;
        rch.padding_right=(charX.size()>2)? QString(charX[2]).toInt():0;
        rch.t = charPosX.toFloat(&okX)/matrix_height;
        rch.b = (charPosX.toFloat(&okX)+1.0)/matrix_height;
        fontMap[ch]=rch;
    }
    font.endGroup();
    if(!fontMap.isEmpty()) isReady=true;
}

PGE_Size RasterFont::textSize(QString &text, int max_line_lenght, bool cut)
{
    if(text.isEmpty())
        return PGE_Size(0, 0);

    int lastspace=0;//!< index of last found space character
    int count=1;    //!< Count of lines
    int maxWidth=0; //!< detected maximal width of message
    int widthSumm=0;
    int widthSummMax=0;
    if(cut)
    {
        for(int i=0;i<text.size();i++)
        {
            if(text[i]==QChar(QChar::LineFeed))
            {
                text.remove(i, text.size()-i);
                break;
            }
        }
    }

    /****************Word wrap*********************/
    for(int x=0, i=0;i<text.size();i++, x++)
    {
        switch(text[i].toLatin1())
        {
            case '\t':
            case ' ':
                lastspace=i;
                widthSumm+=space_width+interletter_space/2;
                if(widthSumm>widthSummMax) widthSummMax=widthSumm;
                break;
            case '\n':
                lastspace=0;
                if((maxWidth<x)&&(maxWidth<max_line_lenght)) maxWidth=x;
                x=0;widthSumm=0;
                count++;
                break;
            default:
                RasChar rch=fontMap[text[i]];
                if(rch.valid)
                {
                    widthSumm+=(letter_width-rch.padding_left-rch.padding_right+interletter_space);
                    if(widthSumm>widthSummMax) widthSummMax=widthSumm;
                }
                else
                {
                    widthSumm+=(letter_width+interletter_space);
                    if(widthSumm>widthSummMax) widthSummMax=widthSumm;
                }
            break;
        }
        if((max_line_lenght>0)&&(x>=max_line_lenght))//If lenght more than allowed
        {
            maxWidth=x;
            if(lastspace>0)
            {
                text[lastspace]='\n';
                i=lastspace-1;
                lastspace=0;
            }
            else
            {
                text.insert(i, QChar('\n'));
                x=0;count++;
            }
        }
    }
    if(count==1)
    {
        maxWidth=text.length();
    }
    /****************Word wrap*end*****************/
    return PGE_Size(widthSummMax, newline_offset*count);
}

void RasterFont::printText(QString text, int x, int y, float Red, float Green, float Blue, float Alpha)
{
    if(fontMap.isEmpty()) return;
    if(text.isEmpty()) return;
    int offsetX=0;
    int offsetY=0;
    GLint w=letter_width;
    GLint h=letter_height;
    foreach(QChar cx, text)
    {
        switch(cx.toLatin1())
        {
        case '\n':
            offsetX=0;
            offsetY+=newline_offset;
            continue;
        case '\t':
            offsetX+=offsetX+offsetX%w;
            continue;
        case ' ':
            offsetX+=space_width+interletter_space/2;
            continue;
        }

        RasChar rch=fontMap[cx];
        if(rch.valid)
        {
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, rch.tx);
            PGE_PointF point;
                point = GlRenderer::MapToGl(x+offsetX-rch.padding_left, y+offsetY);
            float left = point.x();
            float top = point.y();
                point = GlRenderer::MapToGl(x+offsetX-rch.padding_left+w, y+h+offsetY);
            float right = point.x();
            float bottom = point.y();

            glColor4f( Red, Green, Blue, Alpha);
            glBegin(GL_TRIANGLES);
                glTexCoord2f(rch.l,rch.t);glVertex3f(left, top, 0.0);
                glTexCoord2f(rch.r,rch.t);glVertex3f(right, top, 0.0);
                glTexCoord2f(rch.r,rch.b);glVertex3f(right, bottom, 0.0);

                glTexCoord2f(rch.l,rch.t);glVertex3f(left, top, 0.0);
                glTexCoord2f(rch.l,rch.b);glVertex3f(left, bottom, 0.0) ;
                glTexCoord2f(rch.r,rch.b);glVertex3f(right, bottom, 0.0);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            offsetX+=w-rch.padding_left-rch.padding_right+interletter_space;
        }
        else
        {
            GLuint charTex;
            charTex = ttf_borders ? FontManager::getChar2(cx) : FontManager::getChar1(cx);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, charTex);
            PGE_PointF point;
                point = GlRenderer::MapToGl(x+offsetX, y+offsetY);
            float left = point.x();
            float top = point.y();
                point = GlRenderer::MapToGl(x+offsetX+w, y+h+offsetY);
            float right = point.x();
            float bottom = point.y();

            glColor4f( Red, Green, Blue, Alpha);
            glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
                glTexCoord2f(1.0,1.0);glVertex3f(right, top, 0.0);
                glTexCoord2f(0.0,0.0);glVertex3f(right, bottom, 0.0);

                glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
                glTexCoord2f(0.0,0.0);glVertex3f(left, bottom, 0.0) ;
                glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            offsetX+=w+interletter_space;
        }
    }
}

bool RasterFont::isLoaded()
{
    return isReady;
}

QString RasterFont::getFontName()
{
    return fontName;
}
















RasterFont *FontManager::rFont=NULL;
QList<RasterFont> FontManager::rasterFonts;



bool FontManager::isInit=false;
//TTF_Font * FontManager::defaultFont=NULL;

GLuint FontManager::textTexture=0;
QHash<QChar, GLuint> FontManager::fontTable_1;
QHash<QChar, GLuint> FontManager::fontTable_2;

QHash<QString, int> FontManager::fonts;

int     FontManager::fontID;
QFont *FontManager::defaultFont=NULL;
bool FontManager::double_pixled=false;

void FontManager::init()
{
    if(!defaultFont)
        defaultFont = new QFont();

    if(ConfigManager::setup_fonts.fontname.isEmpty())
        fontID = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");
    else
        fontID = QFontDatabase::addApplicationFont(
                                    ConfigManager::data_dir+"/"+
                                    ConfigManager::setup_fonts.fontname);

    double_pixled = ConfigManager::setup_fonts.double_pixled;

    QString family("Monospace");
    if(!QFontDatabase::applicationFontFamilies(fontID).isEmpty())
        family = QFontDatabase::applicationFontFamilies(fontID).at(0);
    defaultFont->setFamily(family);//font.setWeight(14);
//#ifdef __APPLE__
//    defaultFont->setPixelSize(16);
//#else
    defaultFont->setPixelSize(16);
//#endif
    defaultFont->setStyleStrategy(QFont::PreferBitmap);
    defaultFont->setLetterSpacing(QFont::AbsoluteSpacing, 1);
    //defaultFont = buildFont_RW(":/PressStart2P.ttf", 14);

    /***************Load raster font support****************/
    QDir fontsDir(ConfigManager::config_dir+"/fonts");
    QStringList filter;
    filter<<"*.font.ini";
    fontsDir.setNameFilters(filter);

    foreach(QString fonFile, fontsDir.entryList(QDir::Files))
    {
        RasterFont rfont;
        rasterFonts.push_back(rfont);
        rasterFonts.last().loadFont(fontsDir.absolutePath()+"/"+fonFile);
        if(!rasterFonts.last().isLoaded()) { //Pop broken font from array
            rasterFonts.pop_back();
        } else { //Register font name in a table
            fonts[rasterFonts.last().getFontName()] = rasterFonts.size()-1;
        }
    }

    if(!rasterFonts.isEmpty())
        rFont = &rasterFonts.first();

    isInit = true;
}


void FontManager::quit()
{
    //Clean font cache
    glDisable(GL_TEXTURE_2D);
    QHash<QChar, GLuint>::iterator i;
    for (i = fontTable_1.begin(); i != fontTable_1.end(); ++i)
    {
        glDeleteTextures(1, &i.value() );
    }
    fontTable_1.clear();
    for (i = fontTable_2.begin(); i != fontTable_2.end(); ++i)
    {
        glDeleteTextures(1, &i.value() );
    }
    fontTable_2.clear();

    fonts.clear();

    if(defaultFont)
        delete defaultFont;
}

PGE_Size FontManager::textSize(QString &text, int fontID, int max_line_lenght, bool cut)
{
    if(!isInit) return PGE_Size(text.length()*20, text.split(QChar(QChar::LineFeed)).size()*20);
    if(text.isEmpty()) return PGE_Size(0, 0);
    if(max_line_lenght<=0)
        max_line_lenght=1000;

    if(cut)
    {
        for(int i=0;i<text.size();i++)
        {
            if(text[i]==QChar(QChar::LineFeed))
            {
                text.remove(i, text.size()-i);
                break;
            }
        }
    }

    //Use Raster font
    if((fontID>=0)&&(fontID<rasterFonts.size()))
    {
        if(rasterFonts[fontID].isLoaded())
        {
            return rasterFonts[fontID].textSize(text, max_line_lenght);
        }
    }

    //Use TTF font
    QFont fnt = font();
    QFontMetrics meter(fnt);

    optimizeText(text, max_line_lenght);
    QSize meterSize = meter.size(Qt::TextExpandTabs, text);
    return PGE_Size(meterSize.width(), meterSize.height());
}

void FontManager::optimizeText(QString &text, int max_line_lenght, int *numLines, int *numCols)
{
    /****************Word wrap*********************/
    int lastspace=0;
    int count=1;
    int maxWidth=0;
    for(int x=0, i=0;i<text.size();i++, x++)
    {
        switch(text[i].toLatin1())
        {
            case '\t':
            case ' ':
                lastspace=i;
                break;
            case '\n':
                lastspace=0;
                if((maxWidth<x)&&(maxWidth<max_line_lenght)) maxWidth=x;
                x=0;count++;
                break;
        }

        if(x>=27)//If lenght more than allowed
        {
            maxWidth=x;
            if(lastspace>0)
            {
                text[lastspace]='\n';
                i=lastspace-1;
                lastspace=0;
            }
            else
            {
                text.insert(i, QChar('\n'));
                x=0;count++;
            }
        }
    }
    if(count==1)
    {
        maxWidth=text.length();
    }
    /****************Word wrap*end*****************/

    if(numLines)
        *numLines=count;
    if(numCols)
        *numCols=maxWidth;
}

QString FontManager::cropText(QString text, int max_symbols)
{
    if(max_symbols<=0)
        return text;
    if(text.length()<=max_symbols)
        return text;
    int i=max_symbols-1;

    text.remove(i, text.size()-i);
    text.append("...");
    return text;
}







void FontManager::printText(QString text, int x, int y, int font, float Red, float Green, float Blue, float Alpha, int ttf_FontSize)
{
    if(!isInit) return;
    if(text.isEmpty()) return;

    if((font>=0)&&(font<rasterFonts.size()))
    {
        if(rasterFonts[font].isLoaded())
        {
            rasterFonts[font].printText(text, x, y, Red, Green, Blue, Alpha);
            return;
        }
    }
    else if(font==DefaultTTF_Font)
    {
        int offsetX=0;
        int offsetY=0;
        int height=32;
        int width=32;
        foreach(QChar cx, text)
        {
            switch(cx.toLatin1())
            {
            case '\n':
                offsetX=0;
                offsetY+=height;
                continue;
            case '\t':
                offsetX+=offsetX+offsetX%width;
                continue;
            }
            GLint w;
            GLint h;
            GLuint charTex = getChar2(cx);

            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, charTex);
            glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH, &w);
            glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);


            PGE_PointF point;
                point = GlRenderer::MapToGl(x+offsetX, y+offsetY);
            float left = point.x();
            float top = point.y();
                point = GlRenderer::MapToGl(x+offsetX+w, y+h+offsetY);
            float right = point.x();
            float bottom = point.y();

            glColor4f(Red, Green, Blue, Alpha);
            glBegin(GL_TRIANGLES);
                glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
                glTexCoord2f(1.0,1.0);glVertex3f(right, top, 0.0);
                glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);

                glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
                glTexCoord2f(0.0,0.0);glVertex3f(left, bottom, 0.0) ;
                glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);
            glEnd();

            glDisable(GL_TEXTURE_2D);
            width=w;
            height=h;
            offsetX+=w;
        }
        return;
    }
    else {
        printTextTTF(text, x, y, ttf_FontSize, qRgba(Red*255.0, Green*255, Blue*255, Alpha*255));
    }
}

void FontManager::printTextTTF(QString text, int x, int y, int pointSize, QRgb color)
{
    if(!isInit) return;

    QString(family);

    if(!QFontDatabase::applicationFontFamilies(fontID).isEmpty())
            family = QFontDatabase::applicationFontFamilies(fontID).at(0);

    QFont font(family);//font.setWeight(14);
    font.setPointSize(pointSize);

    SDL_string_texture_create(font, color, text, &textTexture);
    SDL_string_render2D(x, y, &textTexture );
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &textTexture );
}


int FontManager::getFontID(QString fontName)
{
    if(fonts.contains(fontName))
        return fonts[fontName];
    else
        return DefaultRaster;
}

GLuint FontManager::getChar1(QChar _x)
{
    QChar c=fontTable_1[_x];
    if(!c.isNull())
        return fontTable_1[_x];
    else
    {
        if(!isInit) return 0;

        QImage text_image;
        QFont font_i = *defaultFont;
        QFontMetrics meter(font_i);
        text_image = QImage(meter.width(_x), meter.height(), QImage::Format_ARGB32);
        text_image.fill(Qt::transparent);
        QPainter x(&text_image);
        x.setFont(font_i);
        x.setBrush(QBrush(Qt::white));
        x.setPen(QPen(Qt::white));
        x.drawText(text_image.rect(), Qt::AlignLeft, QString(_x));
        x.end();

        if(double_pixled)
        {
            //Pixelizing
            text_image = text_image.scaled(text_image.width()/2, text_image.height()/2);
            text_image = text_image.scaled(text_image.width()*2, text_image.height()*2);
        }

        text_image = GraphicsHelps::convertToGLFormat(text_image);//.mirrored(false, true);

        GLuint texture;

        glGenTextures(1, &texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0,  4, text_image.width(), text_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text_image.bits() );

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        fontTable_1[_x] = texture;

        return fontTable_1[_x];
    }
}


GLuint FontManager::getChar2(QChar _x)
{
    QChar c=fontTable_2[_x];
    if(!c.isNull())
        return fontTable_2[_x];
    else
    {
        if(!isInit) return 0;

        QImage text_image;
        int off = 4;

        QFont font_i = *defaultFont;
        font_i.setPointSize(font_i.pointSize());
        QFontMetrics meter(*defaultFont);

        QPainterPath path;

        text_image = QImage(meter.width(_x)+off, meter.height()+off*4, QImage::Format_ARGB32);
        text_image.fill(Qt::transparent);

        QPainter x(&text_image);
        x.setFont(font_i);
        x.setBrush(QBrush(Qt::white));
        x.setPen(QPen(Qt::white));

        path.addText(off, meter.height()+off, font_i, QString(_x));
        x.strokePath(path, QPen(QColor(Qt::black), off));
        x.fillPath(path, QBrush(Qt::white));
        x.end();

        if(double_pixled)
        {
            //Pixelizing
            text_image = text_image.scaled(text_image.width()/2, text_image.height()/2);
            text_image = text_image.scaled(text_image.width()*2, text_image.height()*2);
        }

        text_image = GraphicsHelps::convertToGLFormat(text_image);//.mirrored(false, true);

        GLuint texture;

        glEnable(GL_TEXTURE_2D);
        glGenTextures(1, &texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0,  4, text_image.width(), text_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text_image.bits() );

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        fontTable_2[_x] = texture;
        glDisable(GL_TEXTURE_2D);

        return fontTable_2[_x];
    }
}


QFont FontManager::font()
{
    return *defaultFont;
}















GLuint FontManager::TextToTexture(QString text, PGE_Rect rectangle, int alignFlags, bool borders)
{
    if(!isInit) return 0;

    GLuint fontTexture;
    SDL_string_texture_create(*defaultFont,rectangle, alignFlags, qRgba(255,255,255,255), text, &fontTexture, borders);
    return fontTexture;
}


void FontManager::SDL_string_texture_create(QFont &font, QRgb color, QString &text, GLuint *texture, bool borders)
{
    if(!isInit) return;

    int off = (borders ? 4 : 0);
    QPainterPath path;
    QFontMetrics meter(font);
    QImage text_image = QImage(meter.width(text)+off, (off+meter.height())*text.split('\n').size(), QImage::Format_ARGB32);
    text_image.fill(Qt::transparent);

    QPainter x(&text_image);
    QFont font_i = font;
    if(borders)
        font_i.setPointSize(font_i.pointSize()-1);
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

    text_image = GraphicsHelps::convertToGLFormat(text_image);//.mirrored(false, true);

    glGenTextures(1, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0,  4, text_image.width(), text_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text_image.bits() );

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void FontManager::SDL_string_texture_create(QFont &font, PGE_Rect limitRect, int fontFlags, QRgb color, QString &text, GLuint *texture, bool borders)
{
    if(!isInit) return;

    QImage text_image;
    int off = (borders ? 4 : 0);

    QPainterPath path;
    text_image = QImage(QSize(limitRect.width(), limitRect.height()), QImage::Format_ARGB32);
    text_image.fill(Qt::transparent);
    QFont font_i = font;
    if(borders)
    {
        font_i.setPointSize(font_i.pointSize()-1);
    }
    QFontMetrics meter(font_i);
    QPainter x(&text_image);
    x.setFont(font_i);
    x.setBrush(QBrush(color));
    x.setPen(QPen(color));
    if(borders)
    {
        path.addText(off, meter.height()+off*2, font_i, text);
        x.strokePath(path, QPen(QColor(Qt::black), off));
        x.fillPath(path, QBrush(color));
    }
    else
        x.drawText(text_image.rect(), fontFlags, text);
    x.end();

    if(double_pixled)
    {
        //Pixelizing
        text_image = text_image.scaled(text_image.width()/2, text_image.height()/2);
        text_image = text_image.scaled(text_image.width()*2, text_image.height()*2);
    }

    text_image = GraphicsHelps::convertToGLFormat(text_image);//.mirrored(false, true);

    glGenTextures(1, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, *texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0,  4, text_image.width(), text_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, text_image.bits() );

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

void FontManager::SDL_string_render2D( GLuint x, GLuint y, GLuint *texture )
{
    if(!isInit) return;

    GLint w;
    GLint h;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH, &w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);

    PGE_PointF point;
        point = GlRenderer::MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = GlRenderer::MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glColor4f( 1.f, 1.f, 1.f, 1.f);
    glBegin(GL_TRIANGLES);
        glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
        glTexCoord2f(1.0,1.0);glVertex3f(right, top, 0.0);
        glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);
        glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
        glTexCoord2f(0.0,0.0);glVertex3f(left, bottom, 0.0) ;
        glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);        
    glEnd();

    glDisable(GL_TEXTURE_2D);
}





