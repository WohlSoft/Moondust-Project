/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include "../common_features/app_path.h"
#include "../common_features/graphics_funcs.h"
#include <data_configs/config_manager.h>
#include <graphics/gl_renderer.h>

#include <QtDebug>
#include <QFile>

#include <QImage>
#include <QPainter>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QGLWidget>

#include <SDL2/SDL.h>
#undef main



RasterFont::RasterFont() : first_line_only("\n.*")
{
    letter_width=0;
    letter_height=0;
}

RasterFont::~RasterFont()
{
    glDisable(GL_TEXTURE_2D);
    while(!textures.isEmpty())
    {
        glDeleteTextures(1, &textures.last() );
        textures.pop_back();
    }

}

void RasterFont::loadFontMap(QString fontmap_ini)
{

}

QSize RasterFont::textSize(QString &text, int max_line_lenght, bool cut)
{
    int lastspace=0;//!< index of last found space character
    int count=1;    //!< Count of lines
    int maxWidth=0; //!< detected maximal width of message

    if(cut) text.remove(first_line_only);

    /****************Word wrap*********************/
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

    return QSize(letter_width*maxWidth, letter_height*count);

}


















bool FontManager::isInit=false;
//TTF_Font * FontManager::defaultFont=NULL;

GLuint FontManager::textTexture=0;
QMap<QChar, GLuint> FontManager::fontTable_1;
QMap<QChar, GLuint> FontManager::fontTable_2;

int     FontManager::fontID;
QFont *FontManager::defaultFont=NULL;
bool FontManager::double_pixled=false;

void FontManager::init()
{
    //    int ok = TTF_Init();
    //    if(ok==-1)
    //    {
    //        qDebug() << "Can't load Font manager: " << TTF_GetError() << "\n";
    //        return;
    //    }

    //defaultFont = buildFont(ApplicationPath + "/fonts/PressStart2P.ttf", 14);
    //if(defaultFont==NULL)
    //    return;
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
#ifdef __APPLE__
    defaultFont->setPointSize(18);
#else
    defaultFont->setPointSize(12);
#endif
    defaultFont->setStyleStrategy(QFont::PreferBitmap);
    defaultFont->setLetterSpacing(QFont::AbsoluteSpacing, 1);
    //defaultFont = buildFont_RW(":/PressStart2P.ttf", 14);

    isInit = true;
}

void FontManager::quit()
{
    //Clean font cache
    glDisable(GL_TEXTURE_2D);
    while(!fontTable_1.isEmpty())
    {
        glDeleteTextures(1, &fontTable_1.first() );
        fontTable_1.remove(fontTable_1.firstKey());
    }
    while(!fontTable_2.isEmpty())
    {
        glDeleteTextures(1, &fontTable_2.first() );
        fontTable_2.remove(fontTable_2.firstKey());
    }

    if(defaultFont)
        delete defaultFont;
}

//TTF_Font *FontManager::buildFont(QString _fontPath, GLint size)
//{
//    TTF_Font *temp_font = TTF_OpenFont(_fontPath.toLocal8Bit(), size);
//    if(!temp_font)
//    {
//        qDebug() << "TTF_OpenFont: " << TTF_GetError() << "\n";
//        return NULL;
//    }

//    TTF_SetFontStyle( temp_font, TTF_STYLE_NORMAL );

//    return temp_font;
//}

//TTF_Font *FontManager::buildFont_RW(QString _fontPath, GLint size)
//{
//    QFile font(_fontPath);
//    if(!font.open(QFile::ReadOnly))
//        {
//            qDebug() << "Can't open file" << _fontPath;
//            return NULL;
//        }
//    QDataStream in(&font);
//    int bufferSize = font.size();
//    char *temp = new char[bufferSize];
//    qDebug() << "read data";
//    in.readRawData(temp, bufferSize);

//    qDebug() << "read font";

//    TTF_Font *temp_font = TTF_OpenFontRW(SDL_RWFromMem(temp, (int)bufferSize), 0, size);
//    if(!temp_font)
//    {
//        qDebug() << "TTF_OpenFont: " << TTF_GetError() << "\n";
//        return NULL;
//    }

//    //temp[bufferSize-1] = '\n';
//    //delete[] temp;

//    TTF_SetFontStyle( temp_font, TTF_STYLE_NORMAL );
//    return temp_font;
//}

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

    text_image = QGLWidget::convertToGLFormat(text_image);//.mirrored(false, true);

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

void FontManager::SDL_string_texture_create(QFont &font, QRect limitRect, int fontFlags, QRgb color, QString &text, GLuint *texture, bool borders)
{
    if(!isInit) return;

    QImage text_image;
    int off = (borders ? 4 : 0);

    QPainterPath path;
    text_image = QImage(limitRect.size(), QImage::Format_ARGB32);
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

    text_image = QGLWidget::convertToGLFormat(text_image);//.mirrored(false, true);

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

    QPointF point;
        point = GlRenderer::MapToGl(x, y);
    float left = point.x();
    float top = point.y();
        point = GlRenderer::MapToGl(x+w, y+h);
    float right = point.x();
    float bottom = point.y();

    glColor4f( 1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
        glTexCoord2f(0.0,0.0);glVertex3f(left, bottom, 0.0) ;
        glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);
        glTexCoord2f(1.0,1.0);glVertex3f(right, top, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void FontManager::printText(QString text, int x, int y)
{
    if(!isInit) return;
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


        QPointF point;
            point = GlRenderer::MapToGl(x+offsetX, y+offsetY);
        float left = point.x();
        float top = point.y();
            point = GlRenderer::MapToGl(x+offsetX+w, y+h+offsetY);
        float right = point.x();
        float bottom = point.y();

        glColor4f( 1.f, 1.f, 1.f, 1.f);
        glBegin(GL_QUADS);
            glTexCoord2f(0.0,1.0);glVertex3f(left, top, 0.0);
            glTexCoord2f(0.0,0.0);glVertex3f(left, bottom, 0.0) ;
            glTexCoord2f(1.0,0.0);glVertex3f(right, bottom, 0.0);
            glTexCoord2f(1.0,1.0);glVertex3f(right, top, 0.0);
        glEnd();

        glDisable(GL_TEXTURE_2D);
        width=w;
        height=h;
        offsetX+=w;
    }
}

void FontManager::printText(QString text, int x, int y, int pointSize, QRgb color)
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

GLuint FontManager::TextToTexture(QString text, QRect rectangle, int alignFlags, bool borders)
{
    if(!isInit) return 0;

    GLuint fontTexture;
    SDL_string_texture_create(*defaultFont,rectangle, alignFlags, qRgba(255,255,255,255), text, &fontTexture, borders);
    return fontTexture;
    //SDL_string_render2D(x, y, &textTexture );
    //glDisable(GL_TEXTURE_2D);
    //glDeleteTextures(1, &textTexture );
}

GLuint FontManager::getChar1(QChar _x)
{
    if(fontTable_1.contains(_x))
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

        text_image = QGLWidget::convertToGLFormat(text_image);//.mirrored(false, true);

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
    if(fontTable_2.contains(_x))
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

        text_image = QGLWidget::convertToGLFormat(text_image);//.mirrored(false, true);

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
        fontTable_2[_x] = texture;

        return fontTable_2[_x];
    }
}


QFont FontManager::font()
{
    return *defaultFont;
}

