#ifndef PGE_MENUBOXBASE_H
#define PGE_MENUBOXBASE_H


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

#include "pge_boxbase.h"
#include "../scenes/scene.h"

#include <graphics/gl_renderer.h>
#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>
#include <string>
#include <vector>

#include "pge_menu.h"
#include "pge_msgbox.h"

class Controller;

class PGE_MenuBoxBase : public PGE_BoxBase
{
    public:
        PGE_MenuBoxBase(Scene *_parentScene = NULL, PGE_Menu::menuAlignment alignment = PGE_Menu::menuAlignment::VERTICLE, int gapSpace = 0, std::string _title = "Menu is works!",
                        msgType _type = msg_info, PGE_Point boxCenterPos = PGE_Point(-1, -1), double _padding = -1, std::string texture = "");
        PGE_MenuBoxBase(const PGE_MenuBoxBase &mb);

        void construct(std::string _title = "Menu is works!",
                       msgType _type = msg_info, PGE_Point pos = PGE_Point(-1, -1),
                       double _padding = -1, std::string texture = "");

        ~PGE_MenuBoxBase();

        void setParentScene(Scene *_parentScene);
        void setType(msgType _type);
        void setTitleFont(std::string fontName);
        void setTitleFontColor(GlColor color);
        void setTitleText(std::string text);
        void setPadding(int _padding);
        void setPadding(double _padding);

        void clearMenu();
        void addMenuItem(std::string &menuitem);
        void addMenuItems(std::vector<std::string>& menuitems);

        void setPos(double x, double y);
        void setMaxMenuItems(size_t items);
        void setBoxSize(double _Width, double _Height, double _padding);
        void update(double ticks);
        void render();
        void restart();
        bool isRunning();
        void exec();
        void setRejectSnd(long sndRole);
        size_t answer();

        void reject();

        void processKeyEvent(SDL_Keycode &key);

        void processLoader(double ticks);
        void processBox(double);
        void processUnLoader(double ticks);

    protected:
        PGE_Menu _menu;

    private:
        void updateSize();
        int     _page = 0;
        bool    running = false;
        int     fontID = 0;
        GlColor fontRgba;
        int     m_borderWidth = 32;
        size_t  _answer_id = 0;

        size_t  reject_snd = 0;
        PGE_Point _pos;
        Controller *_ctrl1 = nullptr;
        Controller *_ctrl2 = nullptr;
        msgType type = msg_info;

        PGE_Rect _sizeRect;
        std::string  title;
        PGE_Size title_size;

        double width = 32.0;
        double height = 32.0;
        double padding = 2.0;
        GlColor bg_color;
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
