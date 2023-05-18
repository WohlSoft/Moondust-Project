/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2023 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <Utils/files.h>
#include <Graphics/bitmask2rgba.h>
#include <FileMapper/file_mapper.h>

#include "graphics_funcs.h"
#include <Logger/logger.h>

#ifdef DEBUG_BUILD
#include <Utils/elapsed_timer.h>
#endif

#include <ConfigPackManager/image_size.h>

#include <common_features/engine_resources.h>

#ifdef _WIN32
#include <SDL2/SDL_syswm.h>
#endif
#include <FreeImageLite.h>

void GraphicsHelps::initFreeImage()
{
    FreeImage_Initialise();
}

void GraphicsHelps::closeFreeImage()
{
    FreeImage_DeInitialise();
}

FIBITMAP *GraphicsHelps::loadImage(std::string file, bool convertTo32bit)
{
#ifdef DEBUG_BUILD
    ElapsedTimer loadingTime;
    ElapsedTimer fReadTime;
    ElapsedTimer imgConvTime;
    loadingTime.start();
    fReadTime.start();
#endif

#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32) || defined(__HAIKU__)
    FileMapper fileMap;

    if(!fileMap.open_file(file.c_str()))
        return nullptr;

    FIMEMORY *imgMEM = FreeImage_OpenMemory(reinterpret_cast<unsigned char *>(fileMap.data()),
                                            static_cast<unsigned int>(fileMap.size()));
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato  == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);
    fileMap.close_file();

    if(!img)
        return nullptr;

#else
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.c_str(), 0);

    if(formato  == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_Load(formato, file.c_str());

    if(!img)
        return nullptr;

#endif
#ifdef DEBUG_BUILD
    long long fReadTimeElapsed = static_cast<long long>(fReadTime.nanoelapsed());
    long long imgConvertElapsed = 0;
#endif

    unsigned int bpp = FreeImage_GetBPP(img);

    if(convertTo32bit && bpp != 32)
    {
#ifdef DEBUG_BUILD
        imgConvTime.start();
#endif
        FIBITMAP *temp = FreeImage_ConvertTo32Bits(img);
        FreeImage_Unload(img);
        if(!temp)
            return nullptr;
        img = temp;
#ifdef DEBUG_BUILD
        imgConvertElapsed = static_cast<long long>(imgConvTime.nanoelapsed());
#endif
    }

#ifdef DEBUG_BUILD
    D_pLogDebug("File read of texture %s passed in %d nanoseconds", file.c_str(), static_cast<int>(fReadTimeElapsed));
    D_pLogDebug("Conv to 32-bit of %s passed in %d nanoseconds", file.c_str(), static_cast<int>(imgConvertElapsed));
    D_pLogDebug("Total Loading of image %s passed in %d nanoseconds", file.c_str(), static_cast<int>(loadingTime.nanoelapsed()));
#endif
    return img;
}

FIBITMAP *GraphicsHelps::loadImageRC(const char *file)
{
    unsigned char *memory = nullptr;
    size_t fileSize = 0;
    SDL_assert_release(RES_getMem(file, memory, fileSize));
    //{
    //pLogCritical("Resource file \"%s\" is not found!", file);
    //return nullptr;
    //}

    FIMEMORY *imgMEM = FreeImage_OpenMemory(memory, static_cast<FI_DWORD>(fileSize));
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

    if(formato == FIF_UNKNOWN)
        return nullptr;

    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);

    if(!img)
        return nullptr;

    FIBITMAP *temp;
    temp = FreeImage_ConvertTo32Bits(img);

    if(!temp)
        return nullptr;

    FreeImage_Unload(img);
    img = temp;
    return img;
}

void GraphicsHelps::closeImage(FIBITMAP *img)
{
    FreeImage_Unload(img);
}

SDL_Surface *GraphicsHelps::fi2sdl(FIBITMAP *img)
{
    int h = static_cast<int>(FreeImage_GetHeight(img));
    int w = static_cast<int>(FreeImage_GetWidth(img));
    FreeImage_FlipVertical(img);
    SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(FreeImage_GetBits(img),
                        w, h, 32, w * 4, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, FI_RGBA_ALPHA_MASK);
    return surf;
}

void GraphicsHelps::mergeWithMask(FIBITMAP *image, std::string pathToMask, std::string pathToMaskFallback)
{
    if(!image)
        return;

    if(!Files::fileExists(pathToMask) && pathToMaskFallback.empty())
        return; //Nothing to do

    FIBITMAP *mask = pathToMask.empty() ? nullptr : loadImage(pathToMask, true);
    if(!mask && !pathToMaskFallback.empty())
    {
        FIBITMAP *front = loadImage(pathToMaskFallback, true);
        bitmask_get_mask_from_rgba(front, &mask);
        closeImage(front);
    }

    if(!mask)
        return;//Nothing to do

    bitmask_to_rgba(image, mask);

    FreeImage_Unload(mask);
}

bool GraphicsHelps::getImageMetrics(std::string imageFile, PGE_Size *imgSize)
{
    if(!imgSize)
        return false;

    int errorCode;
    uint32_t w, h;

    if(!PGE_ImageInfo::getImageSize(imageFile, &w, &h, &errorCode))
        return false;

    imgSize->setSize(int(w), int(h));
    return true;
}

void GraphicsHelps::getMaskedImageInfo(std::string rootDir, std::string in_imgName, std::string &out_maskName, std::string &out_errStr, PGE_Size *imgSize)
{
    if(in_imgName.empty())
    {
        out_errStr = "Image filename isn't defined";
        return;
    }

    int errorCode;
    uint32_t w, h;

    if(!PGE_ImageInfo::getImageSize(rootDir + in_imgName, &w, &h, &errorCode))
    {
        switch(errorCode)
        {
        case PGE_ImageInfo::ERR_UNSUPPORTED_FILETYPE:
            out_errStr = "Unsupported or corrupted file format: " + rootDir + in_imgName;
            break;

        case PGE_ImageInfo::ERR_NOT_EXISTS:
            out_errStr = "image file is not exist: " + rootDir + in_imgName;
            break;

        case PGE_ImageInfo::ERR_CANT_OPEN:
            out_errStr = "Can't open image file: " + rootDir + in_imgName;
            break;
        }

        return;
    }

    out_maskName = PGE_ImageInfo::getMaskName(in_imgName);
    out_errStr.clear();

    if(imgSize)
    {
        imgSize->setWidth(int(w));
        imgSize->setHeight(int(h));
    }
}

bool GraphicsHelps::setWindowIcon(SDL_Window *window, FIBITMAP *img, int iconSize)
{
#ifdef _WIN32
    struct SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);

    if(-1 == SDL_GetWindowWMInfo(window, &wmInfo))
        return false;

    if(wmInfo.subsystem != SDL_SYSWM_WINDOWS)
        return false;

    HWND windowH = wmInfo.info.win.window;
    HICON hicon = NULL;
    BYTE *icon_bmp;
    unsigned int icon_len, y;
    SDL_RWops *dst;
    unsigned int w = FreeImage_GetWidth(img);
    unsigned int h = FreeImage_GetWidth(img);
    Uint8 *bits = (Uint8 *)FreeImage_GetBits(img);
    unsigned int pitch = FreeImage_GetPitch(img);
    /* Create temporary bitmap buffer */
    icon_len = 40 + h * w * 4;
    icon_bmp = SDL_stack_alloc(BYTE, icon_len);
    dst = SDL_RWFromMem(icon_bmp, icon_len);

    if(!dst)
    {
        SDL_stack_free(icon_bmp);
        return false;
    }

    /* Write the BITMAPINFO header */
    SDL_WriteLE32(dst, 40);
    SDL_WriteLE32(dst, w);
    SDL_WriteLE32(dst, h * 2);
    SDL_WriteLE16(dst, 1);
    SDL_WriteLE16(dst, 32);
    SDL_WriteLE32(dst, BI_RGB);
    SDL_WriteLE32(dst, h * w * 4);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    SDL_WriteLE32(dst, 0);
    y = 0;

    do
    {
        Uint8 *src = bits + y * pitch;
        SDL_RWwrite(dst, src, pitch, 1);
    }
    while(++y < h);

    hicon = CreateIconFromResource(icon_bmp, icon_len, TRUE, 0x00030000);
    SDL_RWclose(dst);
    SDL_stack_free(icon_bmp);
    /* Set the icon for the window */
    SendMessage(windowH, WM_SETICON, (iconSize < 32) ? ICON_SMALL : ICON_BIG, (LPARAM) hicon);
#else
    (void)iconSize;
    SDL_Surface *sicon = GraphicsHelps::fi2sdl(img);
    SDL_SetWindowIcon(window, sicon);
    SDL_FreeSurface(sicon);
    const char *error = SDL_GetError();

    if(*error != '\0')
        return false;

#endif
    return true;
}


/*********************Code from Qt**end****************/
