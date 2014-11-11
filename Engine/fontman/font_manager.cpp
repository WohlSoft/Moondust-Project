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

#include <QtDebug>
#include <QFile>

#include <QImage>
#include <QPainter>
#include <QFontDatabase>
#include <QGLWidget>

#include <SDL2/SDL.h>
#undef main

bool FontManager::isInit=false;
//TTF_Font * FontManager::defaultFont=NULL;

GLuint FontManager::textTexture=0;

QFont FontManager::defaultFont;

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

    int id = QFontDatabase::addApplicationFont(":/PressStart2P.ttf");

    QString family("Press Start 2P");
    if(!QFontDatabase::applicationFontFamilies(id).isEmpty())
        family = QFontDatabase::applicationFontFamilies(id).at(0);

    defaultFont.setFamily(family);//font.setWeight(14);
    defaultFont.setPointSize(12);

    //defaultFont = buildFont_RW(":/PressStart2P.ttf", 14);

    isInit = true;
}

void FontManager::quit()
{
    glDisable(GL_TEXTURE_2D);
    glDeleteTextures(1, &textTexture );
    //TTF_Quit();
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

void FontManager::SDL_string_texture_create(QFont &font, QString &text, GLuint *texture)
{
    if(!isInit) return;

    //    SDL_Surface *text_image = NULL;
    //    SDL_Surface *temp_image = NULL;

    //     SDL_Color fg_color, bg_color;

    //     GLfloat f_color[4];
    //     GLfloat b_color[4];

    //      glColor4f( 1.f, 1.f, 1.f, 1.f);

    //      glGetFloatv(GL_CURRENT_COLOR, f_color);
    //      fg_color.r=(GLint) (f_color[0]*255);
    //      fg_color.g=(GLint) (f_color[1]*255);
    //      fg_color.b=(GLint) (f_color[2]*255);
    //      glGetFloatv(GL_CURRENT_SECONDARY_COLOR, b_color);
    //      bg_color.r=(GLint) (b_color[0]*255);
    //      bg_color.g=(GLint) (b_color[1]*255);
    //      bg_color.b=(GLint) (b_color[2]*255);

      QImage text_image = QImage(text.size()*16, 32, QImage::Format_ARGB32);
      text_image.fill(Qt::transparent);

      QPainter x(&text_image);
      x.setFont(font);
      x.setBrush(QBrush(Qt::white));
      x.setPen(QPen(Qt::white));
      x.drawText(text_image.rect(), text);
      x.end();

      text_image = QGLWidget::convertToGLFormat(text_image);//.mirrored(false, true);


//      switch (solid)
//      {
//          case 0: text_image = TTF_RenderUTF8_Solid(font, text , fg_color);break;
//          case 1:text_image = TTF_RenderUTF8_Shaded(font, text , fg_color, bg_color); break;
//          case 2: text_image = TTF_RenderUTF8_Blended(font, text , fg_color); break;
//          default: text_image = TTF_RenderUTF8_Solid(font, text , fg_color);break;
//      }

//      temp_image = SDL_CreateRGBSurface(0, text_image->w, text_image->h, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
//      SDL_BlitSurface(text_image, 0, temp_image, 0);

//      SDL_FreeSurface(text_image);

//      GraphicsHelps::flipVertically(temp_image);

      if(*texture==0)
      {
        glGenTextures(1, texture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glBindTexture(GL_TEXTURE_2D, *texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        glTexImage2D(GL_TEXTURE_2D, 0,  4, text_image.width(), text_image.height(), 0, GL_BGRA, GL_UNSIGNED_BYTE, text_image.bits() );

        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
      }
      else
      {
          glBindTexture(GL_TEXTURE_2D, *texture);
          glTexSubImage2D(GL_TEXTURE_2D, 0,0,0, text_image.width(), text_image.height(), GL_BGRA, GL_UNSIGNED_BYTE, text_image.bits() );
      }
      //SDL_FreeSurface(temp_image);
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

    glColor4f( 1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0,1.0);glVertex3f(x, y, 0.0);
        glTexCoord2f(0.0,0.0);glVertex3f(x, y+h, 0.0) ;
        glTexCoord2f(1.0,0.0);glVertex3f(x+w, y+h, 0.0);
        glTexCoord2f(1.0,1.0);glVertex3f(x+w, y, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void FontManager::printText(QString text, int x, int y)
{
    if(!isInit) return;
    SDL_string_texture_create(defaultFont, text, &textTexture);
    SDL_string_render2D(x, y, &textTexture );
}

