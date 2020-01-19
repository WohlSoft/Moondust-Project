/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGE_TextInputBox_H
#define PGE_TextInputBox_H

#include "pge_boxbase.h"
#include "../scenes/scene.h"

#include <graphics/gl_renderer.h>
#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>

#include <controls/control_keys.h>

#include <string>

class Controller;

class PGE_TextInputBox : public PGE_BoxBase
{
public:
    PGE_TextInputBox();
    explicit PGE_TextInputBox(Scene * _parentScene = nullptr, std::string msg = "Message box is works!",
               msgType _type = msg_info, PGE_Point boxCenterPos = PGE_Point(-1,-1),
               double _padding = -1, std::string texture = std::string());
    PGE_TextInputBox(const PGE_TextInputBox &mb);
    ~PGE_TextInputBox() override = default;

    void render() override;
    void restart() override;

    void processBox(double tickTime) override;

    void setInputText(std::string text);
    std::string inputText();

private:
    void construct(std::string msg = "Message box is works!",
                    msgType _type = msg_info, PGE_Point pos = PGE_Point(-1,-1),
                    double _padding = -1, std::string texture = "");

    std::string m_inputTextSrc;
    std::string m_inputText;
    std::string m_inputTextPrintable;
    void updatePrintable();

    Sint32      m_cursor = 0;
    Sint32      m_selectionLength = 0;
    int         m_textInput_h_offset = 0;

    bool   m_blinkShown = false;
    double m_blinkTimeout = 0;

    std::string message;
};

#endif // PGE_TextInputBox_H
