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

#include <QDesktopWidget>
#include <QFile>

#include <common_features/graphics_funcs.h>
#include <common_features/app_path.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <DirManager/dirman.h>
#include <Utils/files.h>
#include <Graphics/bitmask2rgba.h>

#include <FreeImageLite.h>

static FIBITMAP *loadImage(const std::string &file, bool convertTo32bit = true)
{
    QFile fileMap(QString::fromStdString(file));

    FIBITMAP *img = nullptr;

    if(!fileMap.open(QIODevice::ReadOnly))
        return nullptr;

    unsigned int m_size = static_cast<unsigned int>(fileMap.size());
    unsigned char *m = fileMap.map(0, fileMap.size());

    if(m)
    {
        FIMEMORY *imgMEM = FreeImage_OpenMemory(m, m_size);
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileTypeFromMemory(imgMEM);

        if(formato  == FIF_UNKNOWN)
        {
            fileMap.unmap(m);
            return nullptr;
        }

        img = FreeImage_LoadFromMemory(formato, imgMEM, 0);
        FreeImage_CloseMemory(imgMEM);

        fileMap.unmap(m);
    }
    else
    {
        FREE_IMAGE_FORMAT formato = FreeImage_GetFileType(file.c_str(), 0);
        if(formato  == FIF_UNKNOWN)
            return nullptr;
        img = FreeImage_Load(formato, file.c_str());
    }

    if(!img)
        return nullptr;

    unsigned int bpp = FreeImage_GetBPP(img);

    if(convertTo32bit && bpp != 32)
    {
        FIBITMAP *temp = FreeImage_ConvertTo32Bits(img);
        FreeImage_Unload(img);
        if(!temp)
            return nullptr;
        img = temp;
    }

    return img;
}

static bool mergeBitBltToRGBA(FIBITMAP *image, const std::string &pathToMask)
{
    if(!image)
        return false;

    if(!Files::fileExists(pathToMask))
        return false; //Nothing to do.

    FIBITMAP *mask = loadImage(pathToMask);

    if(!mask)
        return false;//Nothing to do.

    bitmask_to_rgba(image, mask);

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

static void splitRGBAtoBitBlt(FIBITMAP *&image, FIBITMAP *&mask)
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
            Npix.rgbGreen = grey;
            Npix.rgbBlue = grey;
            Npix.rgbReserved = 255;

            Fpix.rgbRed  = subtractAlpha(Fpix.rgbRed, grey);
            Fpix.rgbGreen = subtractAlpha(Fpix.rgbGreen, grey);
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
        if((last == '/') || (last == '\\'))
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

static void doLazyFixer(std::string pathIn,  std::string imgFileIn,
                        std::string pathOut,
                        LazyFixTool_Setup &setup)
{
    if(Files::hasSuffix(imgFileIn, "m.gif"))
        return; //Skip mask files

    std::string imgPathIn = pathIn + "/" + imgFileIn;
    std::string maskPathIn;

    //std::cout << imgPathIn;
    //std::cout.flush();

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
            //std::cout << ".MKDIR.";
            //std::cout.flush();
            if(!backupDir.mkdir(""))
            {
                //std::cout << ".FAIL!.";
                //std::cout.flush();
                goto skipBackpDir;
            }
        }
        ret |= Files::copyFile(backupDir.absolutePath() + "/" + imgFileIn,  imgPathIn,  false);
        ret |= Files::copyFile(backupDir.absolutePath() + "/" + maskFileIn, maskPathIn, false);

        if(ret)
            setup.count_backups++;
skipBackpDir:
        ;
    }

    FIBITMAP *image = loadImage(imgPathIn);
    if(!image)
    {
        setup.count_failed++;
        //std::cout << "...CAN'T OPEN!\n";
        //std::cout.flush();
        return;
    }

    FIBITMAP *mask = nullptr;
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
                //std::cout << "...F-WRT FAILED!\n";
                isFail = true;
            }
            FreeImage_Unload(image8);
        }
        else
            isFail = true;
        FreeImage_Unload(image);

    }
    else
        isFail = true;

    if(mask)
    {
        std::string outPathB = pathOut + "/" + maskFileIn;
        FIBITMAP *mask8  = FreeImage_ColorQuantize(mask, FIQ_WUQUANT);
        if(mask8)
        {
            int ret = FreeImage_Save(FIF_GIF, mask8, outPathB.c_str());
            if(!ret)
            {
                //std::cout << "...B-WRT FAILED!\n";
                isFail = true;
            }
            FreeImage_Unload(mask8);
        }
        else
            isFail = true;
        FreeImage_Unload(mask);
    }
    else
        setup.count_nomask++;

    if(isFail)
    {
        setup.count_failed++;
        //std::cout << "...FAILED!\n";
    }
    else
    {
        setup.count_success++;
        //std::cout << "...done\n";
    }
}

void MainWindow::on_actionFixWrongMasks_triggered()
{
    bool doIt = false;
    QString pathIn;
    QString pathOut;

    QString pathIn_custom;
    QString pathOut_custom;

    if(activeChildWindow() == MainWindow::WND_Level)
    {
        LevelEdit *e = activeLvlEditWin();
        if(e->isUntitled())
        {
            QMessageBox::warning(this, tr("File is untitled"),
                                 tr("File doesn't use custom graphics.\n"));
            return;
        }
        pathIn = e->LvlData.meta.path + "/";
        pathIn_custom = e->LvlData.meta.path + "/" + e->LvlData.meta.filename + "/";

        pathOut = pathIn;
        pathOut_custom = pathIn_custom;
        doIt = true;
    }
    else if(activeChildWindow() == MainWindow::WND_World)
    {
        WorldEdit *e = activeWldEditWin();
        if(e->isUntitled())
        {
            QMessageBox::warning(this, tr("File is untitled"),
                                 tr("File doesn't use custom graphics.\n"));
            return;
        }
        pathIn = e->WldData.meta.path + "/";
        pathIn_custom = e->WldData.meta.path + "/" + e->WldData.meta.filename + "/";

        pathOut = pathIn;
        pathOut_custom = pathIn_custom;
        doIt = true;
    }

    if(doIt)
    {
        QStringList filters;
        filters << "*.gif" << "*.GIF";
        QDir imagesDir;
        QDir cImagesDir;

        QStringList fileList;
        QStringList fileList_custom;

        imagesDir.setPath(pathIn);
        cImagesDir.setPath(pathIn_custom);

        if(imagesDir.exists())
            fileList << imagesDir.entryList(filters);

        if(cImagesDir.exists())
            fileList_custom << cImagesDir.entryList(filters);

        //qDebug() << fileList.size()+fileList_custom.size();
        if(fileList.size() + fileList_custom.size() <= 0)
        {
            QMessageBox::warning(this, tr("Nothing to do."),
                                 tr("This file is not use GIF graphics with transparent masks or haven't custom graphics."));
            return;
        }

        LazyFixTool_Setup setup;

        QProgressDialog progress(tr("Fixing of masks..."), tr("Abort"), 0, fileList.size() + fileList_custom.size(), this);
        progress.setWindowTitle(tr("Please wait..."));
        progress.setWindowModality(Qt::WindowModal);
        progress.setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        progress.setFixedSize(progress.size());
        progress.setGeometry(util::alignToScreenCenter(progress.size()));
        //progress.setCancelButton();
        progress.setMinimumDuration(0);

        setup.pathIn = pathIn.toStdString();
        setup.pathOut = pathOut.toStdString();
        delEndSlash(setup.pathIn);
        delEndSlash(setup.pathOut);

        for(QString &q : fileList)
        {
            doLazyFixer(setup.pathIn, q.toStdString(), setup.pathOut, setup);
            progress.setValue(progress.value() + 1);
            qApp->processEvents();
            if(progress.wasCanceled())
                break;
        }

        setup.pathIn = pathIn_custom.toStdString();
        setup.pathOut = pathOut_custom.toStdString();
        delEndSlash(setup.pathIn);
        delEndSlash(setup.pathOut);

        if(!progress.wasCanceled())
            for(QString &q : fileList_custom)
            {
                doLazyFixer(setup.pathIn, q.toStdString(), setup.pathOut, setup);
                progress.setValue(progress.value() + 1);
                qApp->processEvents();
                if(progress.wasCanceled())
                    break;
            }

        if(!progress.wasCanceled())
        {
            progress.close();
            QMessageBox::information(this, tr("Done"),
                                     tr("Masks has been fixed!\nPlease reload current file to apply result."));
        }
    }

}
