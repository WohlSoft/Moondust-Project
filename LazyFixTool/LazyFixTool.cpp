/*
 * LazyFixTool, a free tool for fix lazily-made image masks
 * This is a part of Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <iostream>

#include <QCoreApplication>
#include <QImage>
#include <QDir>
#include <QString>
#include <QTextStream>
#include <QtDebug>
#include <QFileInfo>

QImage setAlphaMask(QImage image, QImage mask)
{
    if(mask.isNull())
        return image;

    if(image.isNull())
        return image;

    QImage target = image;
    QImage newmask = mask;

    if(target.size()!= newmask.size())
    {
        newmask = newmask.copy(0,0, target.width(), target.height());
    }

    newmask.invertPixels();

    target.setAlphaChannel(newmask);

    return target;
}

int main(int argc, char *argv[])
{
    QCoreApplication::addLibraryPath(".");
    QCoreApplication a(argc, argv);

    QStringList filters;
    QDir musicDir;
    QString path;
    QString OPath;
    QStringList fileList;
    QRegExp isMask = QRegExp("*m.gif");
    isMask.setPatternSyntax(QRegExp::Wildcard);

    std::cout<<"============================================================================\n";
    std::cout<<"Lazily-made image masks fix tool by Wohlstand\n";
    std::cout<<"============================================================================\n";
    std::cout<<"This software distributed under GNU GPLv3\n";
    std::cout<<"============================================================================\n";

    if(a.arguments().size()==1)
    {
        goto DisplayHelp;
    }

    if(a.arguments().filter("--help", Qt::CaseInsensitive).size()>0)
    {
        goto DisplayHelp;
    }

    musicDir.setPath(a.arguments().at(1));
    filters << "*.gif" << "*.GIF";
    musicDir.setSorting(QDir::Name);
    musicDir.setNameFilters(filters);

    path = musicDir.absolutePath() + "/";

    if(a.arguments().filter("-O", Qt::CaseSensitive).size()>0)
    {
        OPath = a.arguments().filter("-O", Qt::CaseSensitive).first().remove(0,2);
        if(!QFileInfo(OPath).isDir())
            goto WrongOutputPath;

        OPath = QDir(OPath).absolutePath() + "/";
    }
    else
        OPath=path;

    std::cout<<"============================================================================\n";
    std::cout<<"Converting images...\n";
    std::cout<<"============================================================================\n";

    std::cout<< QString("Input path:  "+path+"\n").toStdString();
    std::cout<< QString("Output path: "+OPath+"\n").toStdString();
    std::cout<<"============================================================================\n";
    fileList << musicDir.entryList(filters);

    foreach(QString q, fileList)
    {

        if(isMask.exactMatch(q))
            continue;

        QImage target;
        QString imgFileM;
        QStringList tmp = q.split(".", QString::SkipEmptyParts);
        if(tmp.size()==2)
            imgFileM = tmp[0] + "m." + tmp[1];
        else
            continue;

        QImage image = QImage(path+q);
        QImage mask = QImage(path+imgFileM);

        if(mask.isNull()) //Skip null masks
            continue;

        target = setAlphaMask(image, mask);

        if(!target.isNull())
        {
            //Save before fix
            target.save(OPath+tmp[0]+"_before.png");
            mask.save(OPath+tmp[0]+"_mask_before.png");
            qDebug() << path+q;


        //fix
        if(image.size()!= mask.size())
            mask = mask.copy(0,0, image.width(), image.height());
        target = mask;

        for(int w=0; w< target.width(); w++)
            for(int h=0; h < target.height(); h++)
            {
                if(target.pixel(w,h)==image.pixel(w,h))
                    target.setPixel(w,h, qRgb(0,0,0));
                else
                if(target.pixel(w,h) > qRgb(0xEF,0xEF,0xEF))
                    target.setPixel(w,h, qRgb(0xFF,0xFF,0xFF));
            }

        mask = target;

        target = setAlphaMask(image, mask);

        //Save after fix
        target.save(OPath+tmp[0]+"_after.png");
        mask.save(OPath+tmp[0]+"_mask_after.png");

        }
        else
        qDebug() << path+q+" - WRONG!";
    }

    std::cout<<"============================================================================\n";
    std::cout<<"Done!\n\n";

    getchar();

    exit(0);
    return a.exec();

DisplayHelp:
    std::cout<<"============================================================================\n";
    std::cout<<"This utility will fix lazily-made image masks:\n";
    std::cout<<"============================================================================\n";
    std::cout<<"Syntax:\n\n";
    std::cout<<"   LazyFixTool [--help] /path/to/folder [-O/path/to/out]\n\n";
    std::cout<<" --help              - Display this help\n";
    std::cout<<" /path/to/folder     - path to directory with pair of GIF files\n";
    std::cout<<" -O/path/to/out      - path to directory where will be saved PNG images\n";
    std::cout<<"\n\n";

    getchar();

    exit(0);
    return a.exec();
WrongOutputPath:
    std::cout<<"============================================================================\n";
    std::cout<<"Wrong output path!\n";
    goto DisplayHelp;
}
