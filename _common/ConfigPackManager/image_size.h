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

#include "PGEString.h"

/**
 * @brief Basic image information misc. functions
 */
namespace PGE_ImageInfo
{

/**
 * @brief Error codes of image information retrieving
 */
enum ErrCodes
{
    //! No errors, everything is OK
    ERR_OK,
    //! Image file type is not supported
    ERR_UNSUPPORTED_FILETYPE,
    //! File not exists
    ERR_NOT_EXISTS,
    //! Can't open the file
    ERR_CANT_OPEN
};

/**
 * @brief Quickly get image size (width and height) from image file.
 * @param [in] imagePath Path to image file
 * @param [out] w Width of image
 * @param [out] h Height of image
 * @param [out] errCode Error code
 * @return true if successfully finished, false if error occouped
 */
bool getImageSize(PGEString imagePath, uint32_t *w, uint32_t *h, int *errCode=nullptr);

/**
 * @brief Returns filename of masked image
 * @param [in] imageFileName foreground image file-name
 * @return masked image filename
 */
PGEString getMaskName(PGEString imageFileName);

}//PGE_ImageInfo

#endif // IMAGESIZE_H
