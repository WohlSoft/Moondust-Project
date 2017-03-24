/*
 * A small set of additional math functions and templates
 *
 * Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this
 * software and associated documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the rights to use, copy, modify,
 * merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
 * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "maths.h"
#include <cmath>
#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#else
#include <stdio.h>
#include <assert.h>
#endif

template<typename T>
static T osRandom()
{
    T dst = 0;
    #ifdef _WIN32
    HCRYPTPROV hCryptProv = 0;
    BOOL res =  CryptGenRandom(hCryptProv, sizeof(T), &dst);
    assert(res != 0);
    #else
    FILE *d = fopen("/dev/urandom", "rb");
    assert(d);
    fread(&dst, 1, sizeof(T), d);
    fclose(d);
    #endif
    return dst;
}

int8_t Maths::rand()
{
    return osRandom<int8_t>();
}

uint8_t Maths::urand8()
{
    return osRandom<uint8_t>();
}

int16_t Maths::rand16()
{
    return osRandom<int16_t>();
}

uint16_t Maths::urand16()
{
    return osRandom<uint16_t>();
}

int32_t Maths::rand32()
{
    return osRandom<int32_t>();
}

uint32_t Maths::urand32()
{
    return osRandom<uint32_t>();
}

int64_t Maths::rand64()
{
    return osRandom<int64_t>();
}

uint64_t Maths::urand64()
{
    return osRandom<uint64_t>();
}


long Maths::roundTo(long src, long gridSize)
{
    long gridX;

    if(gridSize > 0)
    {
        //ATTACH TO GRID
        gridX = (src - src % gridSize);

        if(src < 0)
        {
            if(src < gridX - static_cast<long>(gridSize / 2.0))
                gridX -= gridSize;
        }
        else
        {
            if(src > gridX + static_cast<long>(gridSize / 2.0))
                gridX += gridSize;
        }

        return gridX;
    }
    else
        return src;
}

double Maths::roundTo(double src, double gridSize)
{
    double gridX;
    src = floor(src);

    if(gridSize > 0)
    {
        //ATTACH TO GRID
        gridX = src - std::fmod(src, gridSize);

        if(src < 0)
        {
            if(src < gridX - floor(gridSize / 2.0))
                gridX -= gridSize;
        }
        else
        {
            if(src > gridX + floor(gridSize / 2.0))
                gridX += gridSize;
        }

        return gridX;
    }
    else
        return src;
}
