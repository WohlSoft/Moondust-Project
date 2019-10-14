/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * and also, convert all BMPs into GIF
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2019 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <locale>
#include <iostream>
#include <stdio.h>

#include <FileMapper/file_mapper.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Utf8Main/utf8main.h>
#include <tclap/CmdLine.h>
#include "version.h"

#include <FreeImageLite.h>

static FIBITMAP *loadImage(const std::string &file, bool convertTo32bit = true)
{
#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
    FileMapper fileMap;
    if(!fileMap.open_file(file.c_str()))
        return NULL;

    FIMEMORY *imgMEM = FreeImage_OpenMemory(reinterpret_cast<unsigned char *>(fileMap.data()),
                                            (unsigned int)fileMap.size());
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);
    if(formato  == FIF_UNKNOWN)
        return NULL;
    FIBITMAP *img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
    FreeImage_CloseMemory(imgMEM);
    fileMap.close_file();
    if(!img)
        return NULL;
#else
    FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.c_str(), 0);
    if(formato  == FIF_UNKNOWN)
        return NULL;
    FIBITMAP *img = FreeImage_Load(formato, file.c_str());
    if(!img)
        return NULL;
#endif

    if(convertTo32bit)
    {
        FIBITMAP *temp;
        temp = FreeImage_ConvertTo32Bits(img);
        if(!temp)
            return NULL;
        FreeImage_Unload(img);
        img = temp;
    }
    return img;
}

static bool mergeBitBltToRGBA(FIBITMAP *image, const std::string &pathToMask)
{
    if(!image)
        return false;

    if(!Files::fileExists(pathToMask))
        return false; //Nothing to do

    FIBITMAP *mask = loadImage(pathToMask);

    if(!mask)
        return false;//Nothing to do

    unsigned int img_w  = FreeImage_GetWidth(image);
    unsigned int img_h  = FreeImage_GetHeight(image);
    unsigned int mask_w = FreeImage_GetWidth(mask);
    unsigned int mask_h = FreeImage_GetHeight(mask);

    RGBQUAD Fpix;
    RGBQUAD Bpix;
    RGBQUAD Npix = {0x0, 0x0, 0x0, 0xFF};

    for(unsigned int y = 0; (y < img_h) && (y < mask_h); y++)
    {
        for(unsigned int x = 0; (x < img_w) && (x < mask_w); x++)
        {

            FreeImage_GetPixelColor(image, x, y, &Fpix);
            FreeImage_GetPixelColor(mask, x, y, &Bpix);

            Npix.rgbRed     = ((0x7F & Bpix.rgbRed) | Fpix.rgbRed);
            Npix.rgbGreen   = ((0x7F & Bpix.rgbGreen) | Fpix.rgbGreen);
            Npix.rgbBlue    = ((0x7F & Bpix.rgbBlue) | Fpix.rgbBlue);
            int newAlpha = 255 -
                           ((int(Bpix.rgbRed) +
                             int(Bpix.rgbGreen) +
                             int(Bpix.rgbBlue)) / 3);
            if((Bpix.rgbRed > 240u) //is almost White
               && (Bpix.rgbGreen > 240u)
               && (Bpix.rgbBlue > 240u))
                newAlpha = 0;

            newAlpha = newAlpha + ((int(Fpix.rgbRed) +
                                    int(Fpix.rgbGreen) +
                                    int(Fpix.rgbBlue)) / 3);
            if(newAlpha > 255)
                newAlpha = 255;
            Npix.rgbReserved = static_cast<BYTE>(newAlpha);

            FreeImage_SetPixelColor(image, x, y, &Npix);
        }
    }
    FreeImage_Unload(mask);
    return true;
}

static inline uint8_t subtractAlpha(const uint8_t channel, const uint8_t alpha)
{
    int16_t ch = static_cast<int16_t>(channel) - static_cast<int16_t>(alpha);
    if(ch < 0)
        ch = 0;
    return static_cast<uint8_t>(ch);
}

static void splitRGBAtoBitBlt(FIBITMAP * &image, FIBITMAP *&mask)
{
    unsigned int img_w   = FreeImage_GetWidth(image);
    unsigned int img_h   = FreeImage_GetHeight(image);

    mask = FreeImage_AllocateT(FIT_BITMAP,
                               img_w, img_h,
                               FreeImage_GetBPP(image),
                               FreeImage_GetRedMask(image),
                               FreeImage_GetGreenMask(image),
                               FreeImage_GetBlueMask(image));

    RGBQUAD Fpix;
    RGBQUAD Npix = {0x0, 0x0, 0x0, 0xFF};

    for(unsigned int y = 0; (y < img_h); y++)
    {
        for(unsigned int x = 0; (x < img_w); x++)
        {
            FreeImage_GetPixelColor(image, x, y, &Fpix);

            uint8_t grey = (255 - Fpix.rgbReserved);
            Npix.rgbRed  = grey;
            Npix.rgbGreen= grey;
            Npix.rgbBlue = grey;
            Npix.rgbReserved = 255;

            Fpix.rgbRed  = subtractAlpha(Fpix.rgbRed, grey);
            Fpix.rgbGreen= subtractAlpha(Fpix.rgbGreen, grey);
            Fpix.rgbBlue = subtractAlpha(Fpix.rgbBlue, grey);
            Fpix.rgbReserved = 255;

            FreeImage_SetPixelColor(image, x, y, &Fpix);
            FreeImage_SetPixelColor(mask,  x, y, &Npix);
        }
    }
}

struct LazyFixTool_Setup
{
    std::string pathIn;
    bool listOfFiles        = false;

    std::string pathOut;
    bool pathOutSame        = false;

    bool noMakeBackup       = false;
    bool walkSubDirs        = false;
    unsigned int count_success  = 0;
    unsigned int count_backups  = 0;
    unsigned int count_nomask   = 0;
    unsigned int count_failed   = 0;
};

static inline void delEndSlash(std::string &dirPath)
{
    if(!dirPath.empty())
    {
        char last = dirPath[dirPath.size() - 1];
        if((last == '/')||(last == '\\'))
            dirPath.resize(dirPath.size() - 1);
    }
}

static inline void getGifMask(std::string &mask, const std::string &front)
{
    mask = front;
    //Make mask filename
    size_t dotPos = mask.find_last_of('.');
    if(dotPos == std::string::npos)
        mask.push_back('m');
    else
        mask.insert(mask.begin() + dotPos, 'm');
}

void doLazyFixer(std::string pathIn,  std::string imgFileIn,
                std::string pathOut,
                LazyFixTool_Setup &setup)
{
    if(Files::hasSuffix(imgFileIn, "m.gif"))
        return; //Skip mask files

    std::string imgPathIn = pathIn + "/" + imgFileIn;
    std::string maskPathIn;

    std::cout << imgPathIn;
    std::cout.flush();

    std::string maskFileIn;
    getGifMask(maskFileIn, imgFileIn);

    maskPathIn = pathIn + "/" + maskFileIn;

    //Create backup in case of source and target are same
    if(!setup.noMakeBackup && (pathIn == pathOut))
    {
        DirMan backupDir(pathIn + "/_backup");
        bool ret = false;
        if(!backupDir.exists())
        {
            std::cout << ".MKDIR.";
            std::cout.flush();
            if(!backupDir.mkdir(""))
            {
                std::cout << ".FAIL!.";
                std::cout.flush();
                goto skipBackpDir;
            }
        }
        ret |= Files::copyFile(backupDir.absolutePath() + "/" + imgFileIn,  imgPathIn,  false);
        ret |= Files::copyFile(backupDir.absolutePath() + "/" + maskFileIn, maskPathIn, false);

        if(ret)
            setup.count_backups++;
        skipBackpDir:;
    }

    FIBITMAP *image = loadImage(imgPathIn);
    if(!image)
    {
        setup.count_failed++;
        std::cout << "...CAN'T OPEN!\n";
        std::cout.flush();
        return;
    }

    FIBITMAP *mask = NULL;
    if(Files::fileExists(maskPathIn))
    {
        bool hasMask = mergeBitBltToRGBA(image, maskPathIn);
        if(hasMask) // Split in case is mask presented
            splitRGBAtoBitBlt(image, mask);
    }

    bool isFail = false;
    if(image)
    {
        std::string outPathF = pathOut + "/" + imgFileIn;
        FIBITMAP *image8 = FreeImage_ColorQuantize(image, FIQ_WUQUANT);
        if(image8)
        {
            int ret = FreeImage_Save(FIF_GIF, image8, outPathF.c_str());
            if(!ret)
            {
                std::cout << "...F-WRT FAILED!\n";
                isFail = true;
            }
            FreeImage_Unload(image8);
        }
        else
            isFail = true;
        FreeImage_Unload(image);

    }
    else
    {
        isFail = true;
    }

    if(mask)
    {
        std::string outPathB = pathOut + "/" + maskFileIn;
        FIBITMAP *mask8  = FreeImage_ColorQuantize(mask, FIQ_WUQUANT);
        if(mask8)
        {
            int ret = FreeImage_Save(FIF_GIF, mask8, outPathB.c_str());
            if(!ret)
            {
                std::cout << "...B-WRT FAILED!\n";
                isFail = true;
            }
            FreeImage_Unload(mask8);
        }
        else
            isFail = true;
        FreeImage_Unload(mask);
    } else {
        setup.count_nomask++;
    }

    if(isFail)
    {
        setup.count_failed++;
        std::cout << "...FAILED!\n";
    } else {
        setup.count_success++;
        std::cout << "...done\n";
    }

    std::cout.flush();
}


int main(int argc, char *argv[])
{
//    if(argc > 0)
//        g_ApplicationPath = Files::dirname(argv[0]);
//    g_ApplicationPath = DirMan(g_ApplicationPath).absolutePath();

    DirMan imagesDir;
    std::vector<std::string> fileList;
    FreeImage_Initialise();

    LazyFixTool_Setup setup;

    try
    {
        // Define the command line object.
        TCLAP::CmdLine  cmd(V_FILE_DESC "\n"
                            "Copyright (c) 2017-2019 Vitaly Novichkov <admin@wohlnet.ru>\n"
                            "This program is distributed under the GNU GPLv3+ license\n", ' ', V_FILE_VERSION V_FILE_RELEASE);

        TCLAP::SwitchArg switchNoBackups("n",       "no-backup", "Don't create backup", false);
        TCLAP::SwitchArg switchDigRecursive("d",    "dig-recursive", "Look for images in subdirectories", false);
        TCLAP::SwitchArg switchDigRecursiveDEP("w", "dig-recursive-old", "Look for images in subdirectories [deprecated]", false);

        TCLAP::ValueArg<std::string> outputDirectory("O", "output",
                "path to a directory where the fixed images will be saved",
                false, "", "/path/to/output/directory/");
        TCLAP::UnlabeledMultiArg<std::string> inputFileNames("filePath(s)",
                "Input GIF file(s)",
                true,
                "Input file path(s)");

        cmd.add(&switchNoBackups);
        cmd.add(&switchDigRecursive);
        cmd.add(&switchDigRecursiveDEP);
        cmd.add(&outputDirectory);
        cmd.add(&inputFileNames);

        cmd.parse(argc, argv);

        setup.noMakeBackup      = switchNoBackups.getValue();
        setup.walkSubDirs     = switchDigRecursive.getValue() | switchDigRecursiveDEP.getValue();
        //nopause         = switchNoPause.getValue();

        setup.pathOut     = outputDirectory.getValue();

        for(const std::string &fpath : inputFileNames.getValue())
        {
            if(Files::hasSuffix(fpath, "m.gif"))
                continue;
            else if(DirMan::exists(fpath))
                setup.pathIn = fpath;
            else
            {
                fileList.push_back(fpath);
                setup.listOfFiles = true;
            }
        }

        if((argc <= 1) || (setup.pathIn.empty() && !setup.listOfFiles))
        {
            fprintf(stderr, "\n"
                            "ERROR: Missing input files!\n"
                            "Type \"%s --help\" to display usage.\n\n", argv[0]);
            return 2;
        }
    }
    catch(TCLAP::ArgException &e)   // catch any exceptions
    {
        std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 2;
    }

    fprintf(stderr, "============================================================================\n"
                    "Lazily-made image masks fix tool by Wohlstand. Version " V_FILE_VERSION V_FILE_RELEASE "\n"
                    "============================================================================\n"
                    "This program is distributed under the GNU GPLv3 license \n"
                    "============================================================================\n");
    fflush(stderr);

    if(!setup.listOfFiles)
    {
        if(setup.pathIn.empty())
            goto WrongInputPath;
        if(!DirMan::exists(setup.pathIn))
            goto WrongInputPath;

        imagesDir.setPath(setup.pathIn);
        setup.pathIn = imagesDir.absolutePath();
    }

    if(!setup.pathOut.empty())
    {
        if(!DirMan::exists(setup.pathOut) && !DirMan::mkAbsPath(setup.pathOut))
            goto WrongOutputPath;

        setup.pathOut = DirMan(setup.pathOut).absolutePath();
        setup.pathOutSame   = false;
    }
    else
    {
        setup.pathOut       = setup.pathIn;
        setup.pathOutSame   = true;
    }

    delEndSlash(setup.pathIn);
    delEndSlash(setup.pathOut);

    printf("============================================================================\n"
           "Converting images...\n"
           "============================================================================\n");
    fflush(stdout);

    if(!setup.listOfFiles)
        std::cout << ("Input path:  " + setup.pathIn + "\n");

    std::cout << ("Output path: " + setup.pathOut + "\n");

    std::cout << "============================================================================\n";
    std::cout.flush();

    if(setup.listOfFiles)// Process a list of flies
    {
        for(std::string &file : fileList)
        {
            std::string fname   = Files::basename(file);
            setup.pathIn = DirMan(Files::dirname(file)).absolutePath();
            if(setup.pathOutSame)
                setup.pathOut = DirMan(Files::dirname(file)).absolutePath();
            doLazyFixer(setup.pathIn, fname , setup.pathOut, setup);
        }
    }
    else // Process directories with a source files
    {
        imagesDir.getListOfFiles(fileList, {".gif"});
        if(!setup.walkSubDirs) //By directories
        {
            for(std::string &fname : fileList)
                doLazyFixer(setup.pathIn, fname, setup.pathOut, setup);
        }
        else
        {
            imagesDir.beginWalking({".gif"});
            std::string curPath;
            while(imagesDir.fetchListFromWalker(curPath, fileList))
            {
                if(Files::hasSuffix(curPath, "/_backup"))
                    continue; //Skip backup directories
                for(std::string &file : fileList)
                {
                    if(setup.pathOutSame)
                        setup.pathOut = curPath;
                    doLazyFixer(curPath, file, setup.pathOut, setup);
                }
            }
        }
    }

    printf("============================================================================\n"
           "                      Conversion has been completed!\n"
           "============================================================================\n"
           "Successfully fixed:         %d\n"
           "Masks are not found:        %d\n"
           "Backups created:            %d\n"
           "Fixes failed:               %d\n"
           "\n",
           setup.count_success,
           setup.count_nomask,
           setup.count_backups,
           setup.count_failed);
    fflush(stdout);
    return (setup.count_failed == 0) ? 0 : 1;

WrongInputPath:
    std::cout.flush();
    std::cerr.flush();
    printf("============================================================================\n"
           "                           Wrong input path!\n"
           "============================================================================\n");
    fflush(stdout);
    return 2;

WrongOutputPath:
    std::cout.flush();
    std::cerr.flush();
    printf("============================================================================\n"
           "                          Wrong output path!\n"
           "============================================================================\n");
    fflush(stdout);
    return 2;
}
