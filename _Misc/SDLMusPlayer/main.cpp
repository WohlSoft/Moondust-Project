#include "mainwindow.h"
#include <QApplication>

#undef main
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#undef main

#include <QtDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MODPLUG );
    qDebug() << QString(a.applicationDirPath()+"/timidity/");
    MIX_Timidity_addToPathList(QString(a.applicationDirPath()+"/timidity/").toLocal8Bit().data());
    Mix_OpenAudio(44100, AUDIO_S16, 2, 4096);
    Mix_AllocateChannels(16);

    MainWindow w;
    w.show();
    if(a.arguments().size()>1)
        w.openMusicByArg(a.arguments()[1]);

    int result = a.exec();
    SDL_Quit();
    return result;
}
