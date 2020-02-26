/*
 * PaletteFilter, a free tool for pallete adjuction of images into the reference palette picture
 *
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <cmath>
#include <unordered_map>
#include <stdio.h>

#include <FileMapper/file_mapper.h>
#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Utf8Main/utf8main.h>
#include <tclap/CmdLine.h>
#include "version.h"

#include <FreeImageLite.h>

typedef std::unordered_map<uint32_t, RGBQUAD> Pallete_t;

static uint32_t rgbToInt(const RGBQUAD &q) // without alpha-channel
{
    return (uint32_t(q.rgbRed << 0) & 0xFF) |
           (uint32_t(q.rgbGreen << 8) & 0xFF00) |
           (uint32_t(q.rgbBlue << 16) & 0xFF0000);
}

static FIBITMAP *loadImage(const std::string &file, bool convertTo32bit = true)
{
#if  defined(__unix__) || defined(__APPLE__) || defined(_WIN32)
    FileMapper fileMap;
    if(!fileMap.open_file(file.c_str()))
        return nullptr;

    FIMEMORY *imgMEM = FreeImage_OpenMemory(reinterpret_cast<unsigned char *>(fileMap.data()),
                                            (unsigned int)fileMap.size());
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
            return nullptr;
        FreeImage_Unload(img);
        img = temp;
    }
    return img;
}

static bool buildPallete(FIBITMAP *image, Pallete_t &dst)
{
    if(!image)
        return false;
    unsigned int img_w  = FreeImage_GetWidth(image);
    unsigned int img_h  = FreeImage_GetHeight(image);

    RGBQUAD pix = {0x0, 0x0, 0x0, 0xFF};

    dst.clear();
    for(unsigned int y = 0; y < img_h; y++)
    {
        for(unsigned int x = 0; x < img_w; x++)
        {
            FreeImage_GetPixelColor(image, x, y, &pix);
            uint32_t pix_i = rgbToInt(pix);
            if(dst.find(pix_i) == dst.end())
                dst.insert({pix_i, pix});
        }
    }

    return true;
}

static int colorCmp(const RGBQUAD &q1, const RGBQUAD &q2)
{
    return std::abs(int(q1.rgbRed) - int(q2.rgbRed)) +
           std::abs(int(q1.rgbGreen) - int(q2.rgbGreen)) +
           std::abs(int(q1.rgbBlue) - int(q2.rgbBlue));
}

static bool filterImage(FIBITMAP *image, const Pallete_t &dst)
{
    if(!image)
        return false;

    if(dst.empty())
        return false; // Impossible tp process with a blank pallete;

    unsigned int img_w  = FreeImage_GetWidth(image);
    unsigned int img_h  = FreeImage_GetHeight(image);

    RGBQUAD pix = {0x0, 0x0, 0x0, 0xFF};

    for(unsigned int y = 0; y < img_h; y++)
    {
        for(unsigned int x = 0; x < img_w; x++)
        {
            FreeImage_GetPixelColor(image, x, y, &pix);
            int minDiff = 0;
            RGBQUAD theBest = {0x0, 0x0, 0x0, 0xFF};
            bool first = true;
            for(auto &c : dst)
            {
                const RGBQUAD &q = c.second;
                int diff = colorCmp(q, pix);
                if(first || (std::abs(minDiff) > std::abs(diff)))
                {
                    minDiff = diff;
                    theBest = q;
                    theBest.rgbReserved = pix.rgbReserved; // keep an original alpha-channel
                    first = false;
                }
            }
            FreeImage_SetPixelColor(image, x, y, &theBest);
        }
    }

    return true;
}

struct PalleteFilter_Setup
{
    std::string palleteFile;
    Pallete_t   pallete;

    std::string pathIn;
    bool listOfFiles        = false;

    std::string pathOut;
    bool pathOutSame        = false;

    bool noMakeBackup       = false;
    bool walkSubDirs        = false;
    unsigned int count_success  = 0;
    unsigned int count_backups  = 0;
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

void doPalleteFilter(std::string pathIn,  std::string imgFileIn,
                     std::string pathOut,
                     PalleteFilter_Setup &setup)
{
    if(Files::hasSuffix(imgFileIn, ".gif"))
        return; //Skip any GIF files, unsupported!

    std::string imgPathIn = pathIn + "/" + imgFileIn;

    std::cout << imgPathIn;
    std::cout.flush();

    //Create backup in case of source and target are same
    if(!setup.noMakeBackup && (pathIn == pathOut))
    {
        DirMan backupDir(pathIn + "/_backup");
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

        if(Files::copyFile(backupDir.absolutePath() + "/" + imgFileIn,  imgPathIn,  false))
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

    bool isFail = false;
    if(image)
    {
        filterImage(image, setup.pallete);

        std::string outPathF = pathOut + "/" + imgFileIn;
        int ret = FreeImage_Save(FIF_PNG, image, outPathF.c_str());
        if(!ret)
        {
            std::cout << "...F-WRT FAILED!\n";
            isFail = true;
        }
        FreeImage_Unload(image);
    }
    else
    {
        isFail = true;
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

    PalleteFilter_Setup setup;

    try
    {
        // Define the command line object.
        TCLAP::CmdLine  cmd(V_FILE_DESC "\n"
                            "Copyright (c) 2017-2020 Vitaly Novichkov <admin@wohlnet.ru>\n"
                            "This program is distributed under the GNU GPLv3+ license\n", ' ', V_FILE_VERSION V_FILE_RELEASE);

        TCLAP::SwitchArg switchNoBackups("n",       "no-backup", "Don't create backup", false);
        TCLAP::SwitchArg switchDigRecursive("d",    "dig-recursive", "Look for images in subdirectories", false);
        TCLAP::SwitchArg switchDigRecursiveDEP("w", "dig-recursive-old", "Look for images in subdirectories [deprecated]", false);

        TCLAP::ValueArg<std::string> outputDirectory("O", "output",
                "path to a directory where the fixed images will be saved",
                false, "", "/path/to/output/directory/");
        TCLAP::UnlabeledMultiArg<std::string> inputFileNames("filePath(s)",
                "Input PNG file(s) (GIFs are not supported by this tool)",
                true,
                "Input file path(s)");
        TCLAP::ValueArg<std::string> palleteFile("P", "pallete",
                "Path to the reference PNG or GIF image file for a role of a pallete",
                true, "", "pallete.png");

        cmd.add(&switchNoBackups);
        cmd.add(&switchDigRecursive);
        cmd.add(&switchDigRecursiveDEP);
        cmd.add(&palleteFile);
        cmd.add(&outputDirectory);
        cmd.add(&inputFileNames);

        cmd.parse(argc, argv);

        setup.noMakeBackup      = switchNoBackups.getValue();
        setup.walkSubDirs     = switchDigRecursive.getValue() | switchDigRecursiveDEP.getValue();
        //nopause         = switchNoPause.getValue();

        setup.pathOut     = outputDirectory.getValue();

        setup.palleteFile = palleteFile.getValue();

        for(const std::string &fpath : inputFileNames.getValue())
        {
            if(Files::hasSuffix(fpath, "m.png"))
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

        if(setup.palleteFile.empty() || !Files::fileExists(setup.palleteFile))
        {
            fprintf(stderr, "\n"
                            "ERROR: Missing pallete file (file not exist)!\n");
            return 2;
        }
    }
    catch(TCLAP::ArgException &e)   // catch any exceptions
    {
        std::cerr << "Error: " << e.error() << " for arg " << e.argId() << std::endl;
        return 2;
    }

    fprintf(stderr, "============================================================================\n"
                    "Pallete filter tool by Wohlstand. Version " V_FILE_VERSION V_FILE_RELEASE "\n"
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
           "Pallete initializing (%s) ...\n"
           "============================================================================\n",
           setup.palleteFile.c_str());
    fflush(stdout);

    {
        FIBITMAP *image = loadImage(setup.palleteFile);
        if(!image)
        {
            setup.count_failed++;
            std::cout << "FATAL ERROR: CAN'T OPEN PALETTE FILE!\n";
            std::cout.flush();
            return 1;
        }
        bool isValid = buildPallete(image, setup.pallete);
        FreeImage_Unload(image);
        if(!isValid)
        {
            std::cout << "FATAL ERROR: INVALID PALETTE FILE!\n";
            std::cout.flush();
            return 1;
        }
        printf("Done, %zu colors in a pallete\n"
               "============================================================================\n", setup.pallete.size());
        fflush(stdout);
    }

    printf("============================================================================\n"
           "Filtering images...\n"
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
            doPalleteFilter(setup.pathIn, fname , setup.pathOut, setup);
        }
    }
    else // Process directories with a source files
    {
        imagesDir.getListOfFiles(fileList, {".png"});
        if(!setup.walkSubDirs) //By directories
        {
            for(std::string &fname : fileList)
                doPalleteFilter(setup.pathIn, fname, setup.pathOut, setup);
        }
        else
        {
            imagesDir.beginWalking({".png"});
            std::string curPath;
            while(imagesDir.fetchListFromWalker(curPath, fileList))
            {
                if(Files::hasSuffix(curPath, "/_backup"))
                    continue; //Skip backup directories
                for(std::string &file : fileList)
                {
                    if(setup.pathOutSame)
                        setup.pathOut = curPath;
                    doPalleteFilter(curPath, file, setup.pathOut, setup);
                }
            }
        }
    }

    printf("============================================================================\n"
           "                      Conversion has been completed!\n"
           "============================================================================\n"
           "Successfully fixed:         %d\n"
           "Backups created:            %d\n"
           "Fixes failed:               %d\n"
           "\n",
           setup.count_success,
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
