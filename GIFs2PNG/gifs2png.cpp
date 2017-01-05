/*
 * GIFs2PNG, a free tool for merge GIF images with his masks and save into PNG
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <locale>
#include <iostream>
#include <stdio.h>

#include <FileMapper/file_mapper.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>

#include <Utils/files.h>
#include <tclap/CmdLine.h>
#include "version.h"

#ifdef _WIN32
#define FREEIMAGE_LIB 1
#endif
#include <FreeImageLite.h>

#include "common_features/config_manager.h"

static FIBITMAP *loadImage(std::string file, bool convertTo32bit = true)
{
    #if  defined(__unix__) || defined(_WIN32)
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

static void mergeBitBltToRGBA(FIBITMAP *image, std::string pathToMask)
{
    if(!image)
        return;

    if(!Files::fileExists(pathToMask))
        return; //Nothing to do

    FIBITMAP *mask = loadImage(pathToMask);

    if(!mask)
        return;//Nothing to do

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
            Npix.rgbReserved = newAlpha;

            FreeImage_SetPixelColor(image, x, y, &Npix);
        }
    }
    FreeImage_Unload(mask);
}

static bool endsWithCI(const std::string &str, const std::string &what)
{
    if(what.size() > str.size())
        return false;

    std::locale loc;
    std::string f;
    f.reserve(str.size());
    for(const char &c : str)
        f.push_back(std::tolower(c, loc));

    return (f.compare(f.size() - what.size(), what.size(), what) == 0);
}

void doGifs2PNG(std::string pathIn, std::string imgFileIn, std::string pathOut, bool removeMode, ConfigPackMiniManager &cnf)
{
    if(endsWithCI(pathIn, "/_backup/"))
        return; //Skip backup directories

    if(endsWithCI(imgFileIn, "m.gif"))
        return; //Skip mask files

    std::string maskFileIn = imgFileIn;
    {
        //Make mask name
        size_t dotPos = maskFileIn.find_last_of('.');
        if(dotPos == std::string::npos)
            maskFileIn.push_back('m');
        else
            maskFileIn.insert(maskFileIn.begin() + dotPos, 'm');
    }

    std::string maskPath = cnf.getFile(maskFileIn, pathIn);

    FIBITMAP *image = loadImage(pathIn + imgFileIn);
    if(!image)
    {
        std::cerr << pathIn + imgFileIn + " - CAN'T OPEN!\n";
        std::cerr.flush();
        return;
    }

    if(Files::fileExists(maskPath))
        mergeBitBltToRGBA(image, maskPath);

    if(image)
    {
        std::cout << pathIn + imgFileIn << "\n";
        std::cout.flush();

        std::string outPath = pathOut + "/" + Files::changeSuffix(imgFileIn, ".png");

        if(FreeImage_Save(FIF_PNG, image, outPath.c_str()))
        {
            if(removeMode) // Detele old files
            {
                Files::deleteFile(pathIn + imgFileIn);
                Files::deleteFile(pathIn + maskFileIn);
            }
            std::cout << outPath << "\n";
        }
        FreeImage_Unload(image);
    }
    else
        std::cerr << pathIn + imgFileIn + " - WRONG!\n";
}

int main(int argc, char *argv[])
{
    if(argc > 0)
        g_ApplicationPath = Files::dirname(argv[0]);
    g_ApplicationPath = DirMan(g_ApplicationPath).absolutePath();

    DirMan imagesDir;
    std::vector<std::string> fileList;
    FreeImage_Initialise();
    ConfigPackMiniManager config;

    std::string pathIn;
    bool listOfFiles        = false;
    std::string pathOut;
    bool pathOutSame        = false;
    std::string configPath;
    bool removeMode         = false;
    bool walkSubDirs        = false;
    bool skipBackground2    = false;

    try
    {
        // Define the command line object.
        TCLAP::CmdLine cmd(_FILE_DESC "\n"
                           "Copyright (c) 2017 Vitaly Novichkov <admin@wohlnet.ru>\n"
                           "This program is distributed under the GNU GPLv3+ license\n", ' ', _FILE_VERSION _FILE_RELEASE);

        TCLAP::SwitchArg switchRemove("r", "remove", "Remove source images after a succesful conversion", false);
        cmd.add(switchRemove);

        TCLAP::SwitchArg switchSkipBG("b", "ingnore-bg", "Skip all \"background2-*.gif\" sprites (due a bug in the LunaLUA)", false);
        cmd.add(switchSkipBG);

        TCLAP::SwitchArg switchDigRecursive("d", "dig-recursive", "Look for images in subdirectories", false);
        cmd.add(switchDigRecursive);

        TCLAP::SwitchArg switchDigRecursiveDEP("w", "dig-recursive-old", "Look for images in subdirectories [deprecated]", false);
        cmd.add(switchDigRecursiveDEP);

        TCLAP::ValueArg<std::string> outputDirectory("O", "output",
                "path to a directory where the PNG images will be saved",
                false, "", "/path/to/output/directory/");
        cmd.add(outputDirectory);

        TCLAP::ValueArg<std::string> configDirectory("C", "config",
                "Allow usage of default masks from specific PGE config pack "
                "(Useful for cases where the GFX designer didn't make a mask image)",
                false, "", "/path/to/config/pack");
        cmd.add(configDirectory);

        TCLAP::UnlabeledMultiArg<std::string> inputFileNames("filePath(s)",
                "Input GIF file(s)",
                true,
                "Input file path(s)");
        cmd.add(inputFileNames);

        cmd.parse(argc, argv);

        removeMode      = switchRemove.getValue();
        skipBackground2 = switchSkipBG.getValue();
        walkSubDirs     = switchDigRecursive.getValue() | switchDigRecursiveDEP.getValue();
        //nopause         = switchNoPause.getValue();

        pathOut     = outputDirectory.getValue();
        configPath  = configDirectory.getValue();

        for(const std::string &fpath : inputFileNames.getValue())
        {
            if(endsWithCI(fpath, "m.gif"))
                continue;
            else if(DirMan::exists(fpath))
                pathIn = fpath;
            else
            {
                fileList.push_back(fpath);
                listOfFiles = true;
            }
        }

        if((argc <= 1) || (pathIn.empty() && !listOfFiles))
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
                    "Pair of GIFs to PNG converter tool by Wohlstand. Version " _FILE_VERSION _FILE_RELEASE "\n"
                    "============================================================================\n"
                    "This program is distributed under the GNU GPLv3 license \n"
                    "============================================================================\n");
    fflush(stderr);

    if(!listOfFiles)
    {
        if(pathIn.empty())
            goto WrongInputPath;
        if(!DirMan::exists(pathIn))
            goto WrongInputPath;

        imagesDir.setPath(pathIn);
        pathIn = imagesDir.absolutePath() + "/";
    }

    if(!pathOut.empty())
    {
        if(!DirMan::exists(pathOut))
            goto WrongOutputPath;

        pathOut = DirMan(pathOut).absolutePath();
    }
    else
    {
        pathOut         = pathIn;
        pathOutSame     = true;
    }

    std::cout << "============================================================================\n";
    std::cout << "Converting images...\n";
    std::cout << "============================================================================\n";
    std::cout.flush();

    if(!listOfFiles)
        std::cout << ("Input path:  " + pathIn + "\n");

    std::cout << ("Output path: " + pathOut + "\n");

    std::cout << "============================================================================\n";
    std::cout.flush();

    config.setConfigDir(configPath);

    if(config.isUsing())
    {
        std::cout << "============================================================================\n";
        std::cout << ("Used config pack: " + configPath + "\n");
        std::cout << "============================================================================\n";
        std::cout.flush();
    }

    if(listOfFiles)// Process a list of flies
    {
        for(std::string &file : fileList)
        {
            pathIn = Files::dirname(file);
            std::string fname = Files::basename(file);
            if(skipBackground2 && (fname.compare(0, 11, "background2", 11) == 0))
                continue;
            if(pathOutSame)
                pathOut = pathIn;
            doGifs2PNG(pathIn + "/", fname , pathOut, removeMode, config);
        }
    }
    else // Process directories with a source files
    {
        imagesDir.getListOfFiles(fileList, {".gif"});
        if(!walkSubDirs) //By directories
        {
            for(std::string &fname : fileList)
            {
                if(skipBackground2 && (fname.compare(0, 11, "background2", 11) == 0))
                    continue;
                doGifs2PNG(pathIn, fname, pathOut, removeMode, config);
            }
        }
        else
        {
            imagesDir.beginWalking({".gif"});
            std::string curPath;
            while(imagesDir.fetchListFromWalker(curPath, fileList))
            {
                for(std::string &file : fileList)
                {
                    if(skipBackground2 && (file.compare(0, 11, "background2", 11) == 0))
                        continue;
                    if(pathOutSame)
                        pathOut = curPath;
                    //if(cOpath)
                    //pathOut = QFileInfo(dirsList.filePath()).dir().absolutePath();
                    doGifs2PNG(pathOut + "/", file, curPath, removeMode, config);
                }
            }
        }
    }

    std::cout << "============================================================================\n";
    std::cout << "Done!\n\n";
    std::cout.flush();
    return 0;

#if 0
DisplayHelp:
    //If we are running on windows, we want the "help" screen to display the arg options in the Windows style
    //to be consistent with native Windows applications (using '/' instead of '-' before single-letter args)

    std::cerr << "============================================================================\n";
    std::cerr << "This utility will merge GIF images and their masks into solid PNG images:\n";
    std::cerr << "============================================================================\n";
    std::cerr << "Syntax:\n\n";
    #ifdef Q_OS_WIN
#define ARGSIGN "/"
    #else
#define ARGSIGN "-"
    #endif
    std::cerr << "   GIFs2PNG [--help] [" ARGSIGN "R] file1.gif [file2.gif] [...] [" ARGSIGN "O path/to/output]\n";
    std::cerr << "   GIFs2PNG [--help] [" ARGSIGN "D] [" ARGSIGN "R] path/to/input [" ARGSIGN "O path/to/output]\n\n";
    std::cerr << " --help              - Display this help\n";
    std::cerr << " path/to/input       - path to a directory with pairs of GIF files\n";
    std::cerr << " " ARGSIGN "O path/to/output   - path to a directory where the PNG images will be saved\n";
    std::cerr << " " ARGSIGN "R                  - Remove source images after a succesful conversion\n";
    std::cerr << " " ARGSIGN "D                  - Look for images in subdirectories\n";
    std::cerr << " " ARGSIGN "B                  - Skip all \"background2-*.gif\" sprites (due a bug in the LunaLUA)\n";

    std::cerr << "\n";
    std::cerr << " --config /path/to/config/pack\n";
    std::cerr << "                     - Allow usage of default masks from specific PGE config pack\n";
    std::cerr << "                       (Useful for cases where the GFX designer didn't make a mask image)\n";
    std::cerr << " --nopause           - Don't pause application after processing finishes (useful for script integration)\n";
    std::cerr << "\n\n";
    std::cerr.flush();
    return 1;
#endif

WrongInputPath:
    std::cerr << "============================================================================\n";
    std::cerr << "Wrong input path!\n";
    std::cerr.flush();
    return 2;

WrongOutputPath:
    std::cerr << "============================================================================\n";
    std::cerr << "Wrong output path!\n";
    std::cerr.flush();
    return 2;
}
