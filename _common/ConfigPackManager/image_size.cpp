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

#include "image_size.h"
#include <QFile>

static bool tryGIF(QFile &file, int *w, int *h)
{
    file.seek(0);
    const char *GIF1 = "GIF87a";
    const char *GIF2 = "GIF89a";
    char magic[6];

    if(file.read(magic, 6) != 6)
        return false;

    bool found = false;

    if(strncmp(magic, GIF1, 6) == 0)
        found = true;

    if(strncmp(magic, GIF2, 6) == 0)
        found = true;

    if(!found)
        return false;

    unsigned char size[4];

    if(file.read(reinterpret_cast<char *>(size), 4) != 4)
        return false;

#define UINT(d) static_cast<unsigned int>(d)
    *w = static_cast<int>((UINT(size[0]) & 0x00FF) | ((UINT(size[1]) << 8) & 0xFF00));
    *h = static_cast<int>((UINT(size[2]) & 0x00FF) | ((UINT(size[3]) << 8) & 0xFF00));
#undef UINT

    if(*w < 0)
        return false;

    if(*h < 0)
        return false;

    return true;
}

static bool tryBMP(QFile &file, int *w, int *h)
{
    file.seek(0);
    const char *BMP = "BM";
    char magic[2];

    if(file.read(magic, 2) != 2)
        return false;

    if(strncmp(magic, BMP, 2) != 0)
        return false;

    unsigned char size[8];

    if(!file.seek(18))
        return false;

    if(file.read(reinterpret_cast<char *>(size), 8) != 8)
        return false;

#define UINT(d) static_cast<unsigned int>(d)
    *w = static_cast<int>((UINT(size[3]) & 0xFF) | ((UINT(size[2]) << 8) & 0xFF00) | ((UINT(size[1]) << 16) & 0xFF0000) | ((UINT(size[0]) << 24) & 0xFF000000));
    *h = static_cast<int>((UINT(size[7]) & 0xFF) | ((UINT(size[6]) << 8) & 0xFF00) | ((UINT(size[5]) << 16) & 0xFF0000) | ((UINT(size[4]) << 24) & 0xFF000000));
#undef UINT

    if(*w < 0)
        return false;

    if(*h < 0)
        return false;

    return true;
}

static bool tryPNG(QFile &file, int *w, int *h)
{
    file.seek(0);
    const char *PNG  = "\211PNG\r\n\032\n";
    const char *IHDR = "IHDR";
    char magic[8];

    if(file.read(magic, 8) != 8)
        return false;

    if(strncmp(magic, PNG, 8) != 0)
        return false;

    if(file.read(magic, 8) != 8)
        return false;

    if(strncmp(magic + 4, IHDR, 4) != 0)
        return false;

    unsigned char size[8];

    if(file.read(reinterpret_cast<char *>(size), 8) != 8)
        return false;

#define UINT(d) static_cast<unsigned int>(d)
    *w = static_cast<int>((UINT(size[3]) & 0xFF) | ((UINT(size[2]) << 8) & 0xFF00) | ((UINT(size[1]) << 16) & 0xFF0000) | ((UINT(size[0]) << 24) & 0xFF000000));
    *h = static_cast<int>((UINT(size[7]) & 0xFF) | ((UINT(size[6]) << 8) & 0xFF00) | ((UINT(size[5]) << 16) & 0xFF0000) | ((UINT(size[4]) << 24) & 0xFF000000));
#undef UINT

    if(*w < 0)
        return false;

    if(*h < 0)
        return false;

    return true;
}

bool PGE_ImageInfo::getImageSize(QString imagePath, int *w, int *h, int *errCode)
{
    QFile image(imagePath);

    if(errCode)
        *errCode = ERR_OK;

    if(!image.exists())
    {
        if(errCode)
            *errCode = ERR_NOT_EXISTS;

        return false;
    }

    if(!image.open(QIODevice::ReadOnly))
    {
        if(errCode)
            *errCode = ERR_CANT_OPEN;

        return false;
    }

    if(tryGIF(image, w, h))
        return true;

    if(tryPNG(image, w, h))
        return true;

    if(tryBMP(image, w, h))
        return true;

    if(errCode)
        *errCode = ERR_UNSUPPORTED_FILETYPE;

    return false;
}

QString PGE_ImageInfo::getMaskName(QString imageFileName)
{
    QString out_maskName = imageFileName;
    int i = out_maskName.size() - 1;

    for(; i > 0; i--)
    {
        if(out_maskName[i] == '.')
        {
            out_maskName.insert(i, 'm');
            break;
        }
    }

    if(i == 0)
        out_maskName = "";

    return out_maskName;
}
