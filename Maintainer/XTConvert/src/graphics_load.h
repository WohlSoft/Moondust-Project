/*
 * XTConvert - a tool to package asset packs and episodes for TheXTech game engine.
 * Copyright (c) 2024 ds-sloth
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

#include <QString>

struct FIBITMAP;
class GraphicsLoad
{
public:
    // ensures that FreeImage is inited while alive
    struct FreeImage_Sentinel
    {
        FreeImage_Sentinel();
        ~FreeImage_Sentinel();
    };

    static FIBITMAP* loadImage(const QString& path);

    static void RGBAToMask(FIBITMAP* mask);

    static void mergeWithMask(FIBITMAP *image, FIBITMAP *mask);

    static FIBITMAP *fast2xScaleDown(FIBITMAP *image);

    static bool validateBitmaskRequired(FIBITMAP *image, FIBITMAP *mask);

    static bool validateForDepthTest(FIBITMAP *image);
};
