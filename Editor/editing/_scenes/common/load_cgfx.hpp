/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#pragma once
#ifndef LOAD_CGFX_HPP
#define LOAD_CGFX_HPP

#include <common_features/items.h>
#include <common_features/graphics_funcs.h>

template<class T>
inline void loadCustomIcon(CustomDirManager &uLVL, T &t_item)
{
    t_item.icon = QPixmap(); // Enforce auto-generated icon for customized sprites if no custom given

    if(!t_item.setup.icon_n.isEmpty()) // Load custom icon in only when custom image is used
    {
        auto customIconFile = uLVL.getCustomFile(t_item.setup.icon_n, true);
        if(customIconFile.isEmpty())
        {
            t_item.icon = GraphicsHelps::loadPixmap(customIconFile);
            t_item.cur_icon = &t_item.icon;
        }
    }
}

template<class T, class D>
inline bool loadCustomImage(QImage &tempImg,
                            CustomDirManager &uLVL,
                            T &t_item,
                            D *itemD,
                            QList<QPixmap> &localImages,
                            bool *isInvalidImage,
                            QString *customImgPath = nullptr,
                            QSize *capturedSize = nullptr)
{
    bool isCustom = false;
    QString customImgFile;

    if(customImgPath)
    {
        customImgFile = uLVL.getCustomFile(*customImgPath, true);
        if(customImgFile.isEmpty())
            customImgFile = uLVL.getCustomFile(t_item.setup.image_n, true);
    }
    else
        customImgFile = uLVL.getCustomFile(t_item.setup.image_n, true);

    if(!customImgFile.isEmpty())
    {
        if(!customImgFile.endsWith(".png", Qt::CaseInsensitive))
        {
            QString customMask = uLVL.getCustomFile(t_item.setup.mask_n, false);
            GraphicsHelps::loadQImage(tempImg, customImgFile, customMask, &itemD->image);
        }
        else
            GraphicsHelps::loadQImage(tempImg, customImgFile);

        if(tempImg.isNull())
        {
            *isInvalidImage = true;
        }
        else
        {
            localImages.push_back(QPixmap::fromImage(tempImg));
            t_item.cur_image = &localImages.last();
            if(capturedSize) // Capture size of image
                *capturedSize = QSize(t_item.cur_image->width(), t_item.cur_image->height());
        }

        isCustom = true;

        loadCustomIcon(uLVL, t_item);
    }

    return isCustom;
}

#endif // LOAD_CGFX_HPP
