#ifndef PGE_MENUBOXBASE_H
#define PGE_MENUBOXBASE_H


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

#include "pge_boxbase.h"
#include "../scenes/scene.h"

#include <graphics/gl_renderer.h>
#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>

#include <QColor>
#include <QString>
#include <QStringList>

#include "pge_menu.h"
#include "pge_msgbox.h"

class Controller;

class PGE_MenuBoxBase : public PGE_BoxBase
{
public:
    PGE_MenuBoxBase(Scene * _parentScene=NULL, PGE_Menu::menuAlignment alignment=PGE_Menu::menuAlignment::VERTICLE, int gapSpace=0, QString _title="Menu is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    PGE_MenuBoxBase(const PGE_MenuBoxBase &mb);

    void construct(QString _title="Menu is works!",
                    msgType _type=msg_info, PGE_Point pos=PGE_Point(-1,-1), float _padding=-1, QString texture="");

    ~PGE_MenuBoxBase();

    void setParentScene(Scene * _parentScene);
    void setType(msgType _type);
    void setTitleFont(QString fontName);
    void setTitleFontColor(GlColor color);
    void setTitleText(QString text);
    void setPadding(int _padding);

    void clearMenu();
    void addMenuItem(QString &menuitem);
    void addMenuItems(QStringList &menuitems);

    void setPos(float x, float y);
    void setMaxMenuItems(int items);
    void setBoxSize(float _Width, float _Height, float _padding);
    void update(float ticks);
    void render();
    void restart();
    bool isRunning();
    void exec();
    void setRejectSnd(long sndRole);
    int  answer();

    void reject();

    void processKeyEvent(SDL_Keycode &key);

    void processLoader(float ticks);
    void processBox(float);
    void processUnLoader(float ticks);

    static void info(QString msg);
    //static void info(std::string msg);
    static void warn(QString msg);
    //static void warn(std::string msg);
    static void error(QString msg);
    //static void error(std::string msg);
    static void fatal(QString msg);
    //static void fatal(std::string msg);

protected:
    PGE_Menu _menu;

private:
    void updateSize();
    int     _page;
    bool    running;
    int     fontID;
    GlColor fontRgba;
    int     _answer_id;

    long    reject_snd;
    PGE_Point _pos;
    Controller *_ctrl1;
    Controller *_ctrl2;
    msgType type;

    PGE_Rect _sizeRect;
    QString  title;
    PGE_Size title_size;

    float width;
    float height;
    float padding;
    QColor bg_color;
    void initControllers();
    void updateControllers();

    virtual void onUpButton() {}
    virtual void onDownButton() {}
    virtual void onLeftButton() {}
    virtual void onRightButton() {}
    virtual void onJumpButton() {}
    virtual void onAltJumpButton() {}
    virtual void onRunButton() {}
    virtual void onAltRunButton() {}
    virtual void onStartButton() {}
    virtual void onDropButton() {}
};

#endif // PGE_MENUBOXBASE_H
