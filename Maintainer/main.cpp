#include "main_window.h"
#include <QApplication>
#include <common_features/logger.h>
#include <common_features/app_path.h>
#include <Utf8Main/utf8main.h>

// Windows available for open
#include <Music/audiocvt_sox_gui.h>
#include <Graphics/gifs2png.h>
#include <Graphics/png2gifs.h>
#include <Graphics/lazyfix.h>
#include <EpisodeCvt/episode_converter.h>
#include <EpisodeCvt/case_fixer.h>

const Qt::WindowFlags g_dialogFlags =
        Qt::Window |
        Qt::WindowSystemMenuHint |
        Qt::WindowTitleHint |
        Qt::WindowCloseButtonHint |
        Qt::WindowMinimizeButtonHint;

#include <QFileInfo>
#include <QDir>

#include <QtDebug>
#include <QStyleFactory>

enum OpenWindow
{
    OPEN_MAIN = 0,
    OPEN_GIFs2PNG,
    OPEN_PNG2GIFs,
    OPEN_LAZYFIX,
    OPEN_MUSIC_CONVERTER,
    OPEN_FILE_CONVERTER,
    OPEN_CASE_FIXER
};

int main(int argc, char *argv[])
{
    OpenWindow open = OPEN_MAIN;
    QString configPackPath;
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    QStringList args;

    QApplication a(argc, argv);
#ifdef _WIN32
    for(int i = 0; i < argc; i++)
        args.push_back(QString::fromUtf8(argv[i]));
#else
    args    = a.arguments();
#endif

    AppPathManager::initAppPath(argv[0]);
    LoadLogSettings();

    if(args.size() > 1)
    {
        if(args[1] == "gifs2png")
        {
            if(args.size() >= 3)
                configPackPath = args[2];
            open = OPEN_GIFs2PNG;
        }
        else if(args[1] == "png2gifs")
            open = OPEN_PNG2GIFs;
        else if(args[1] == "lazyfix")
            open = OPEN_LAZYFIX;
        else if(args[1] == "filecvt")
            open = OPEN_FILE_CONVERTER;
        else if(args[1] == "casefixer")
            open = OPEN_CASE_FIXER;
        else if(args[1] == "muscvt")
            open = OPEN_MUSIC_CONVERTER;
    }

    MaintainerMain w;

    switch(open)
    {
    default:
    case OPEN_MAIN:
        w.show();
        break;

    case OPEN_MUSIC_CONVERTER:
        {
            AudioCvt_Sox_gui mus(nullptr);
            mus.setWindowFlags(g_dialogFlags);
            mus.setWindowModality(Qt::NonModal);
            mus.show();
            return a.exec();
        }
        break;

    case OPEN_FILE_CONVERTER:
        {
            EpisodeConverter eps(nullptr);
            eps.setWindowFlags(g_dialogFlags);
            eps.setWindowModality(Qt::NonModal);
            eps.show();
            return a.exec();
        }
        break;

    case OPEN_GIFs2PNG:
        {
            GIFs2PNG g2p(nullptr);
            g2p.setWindowFlags(g_dialogFlags);
            g2p.setWindowModality(Qt::NonModal);
            if(!configPackPath.isEmpty())
                g2p.setConfigPackPath(configPackPath);
            g2p.show();
            return a.exec();
        }
        break;

    case OPEN_PNG2GIFs:
        {
            PNG2GIFs p2g(nullptr);
            p2g.setWindowFlags(g_dialogFlags);
            p2g.setWindowModality(Qt::NonModal);
            p2g.show();
            return a.exec();
        }
        break;

    case OPEN_LAZYFIX:
        {
            LazyFixTool lzf(nullptr);
            lzf.setWindowFlags(g_dialogFlags);
            lzf.setWindowModality(Qt::NonModal);
            lzf.show();
            return a.exec();
        }
        break;

    case OPEN_CASE_FIXER:
        {
            CaseFixer casefixer(nullptr);
            casefixer.setWindowFlags(g_dialogFlags);
            casefixer.setWindowModality(Qt::NonModal);
            casefixer.show();
            return a.exec();
        }
        break;
    }

    return a.exec();
}
