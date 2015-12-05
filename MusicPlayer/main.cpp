#include "mainwindow.h"
#include <QApplication>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>

#include "SingleApplication/singleapplication.h"

#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

static void error(QString msg)
{
    QMessageBox::warning(nullptr, "SDL error", msg, QMessageBox::Ok);
}

int main(int argc, char *argv[])
{
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    QApplication a(argc, argv);
    QStringList args=a.arguments();
    SingleApplication *as = new SingleApplication(args);
    if(!as->shouldContinue())
    {
        QTextStream(stdout) << "SDL2 Mixer X Player already runned!\n";
        delete as;
        return 0;
    }

    if(SDL_Init(SDL_INIT_AUDIO)==-1)
        error(SDL_GetError());

    if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_MOD | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MODPLUG )==-1)
        error(Mix_GetError());

    qDebug() << QString(a.applicationDirPath()+"/timidity/");
    MIX_Timidity_addToPathList(QString(a.applicationDirPath()+"/timidity/").toLocal8Bit().data());

    if(Mix_OpenAudio(44100, AUDIO_S16, 2, 4096)==-1)
        error(Mix_GetError());

    MainWindow w;

    //Set acception of external file openings
    w.connect(as, SIGNAL(openFile(QString)), &w, SLOT(openMusicByArg(QString)));

    w.show();
    if(a.arguments().size()>1)
        w.openMusicByArg(a.arguments()[1]);

    int result = a.exec();
    delete as;
    SDL_Quit();
    return result;
}
