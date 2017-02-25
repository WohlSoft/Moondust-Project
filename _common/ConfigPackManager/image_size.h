/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef IMAGESIZE_H
#define IMAGESIZE_H

#include <QString>

class PGE_ImageInfo
{
public:
    enum ErrCodes
    {
        ERR_OK,
        ERR_UNSUPPORTED_FILETYPE,
        ERR_NOT_EXISTS,
        ERR_CANT_OPEN
    };

    /**
     * @brief Quickly get image size (width and height) from image file.
     * @param imagePath Path to image file
     * @param [out] w Width of image
     * @param [out] h Height of image
     * @param [out] errCode Error code
     * @return true if successfully finished, false if error occouped
     */
    static bool getImageSize(QString imagePath, uint32_t*w, uint32_t *h, int *errCode=nullptr);
    static bool getImageSize(std::string imagePath, uint32_t *w, uint32_t *h, int *errCode=nullptr);

    /**
     * @brief Returns filename of masked image
     * @param imageFileName foreground image file-name
     * @return masked image filename
     */
    static QString getMaskName(QString imageFileName);
    static std::string getMaskName(std::string imageFileName);
};

#endif // IMAGESIZE_H
