#include "mainwindow.h"
#ifndef MUSPLAY_USE_WINAPI
#include <QApplication>
#else
#include <windows.h>
#include <commctrl.h>
#include "defines.h"
#endif

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>

#include "SingleApplication/singleapplication.h"
#include "SingleApplication/pge_application.h"

#ifndef MUSPLAY_USE_WINAPI
#include <QtDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#endif

static void error(QString msg)
{
    #ifndef MUSPLAY_USE_WINAPI
    QMessageBox::warning(nullptr, "SDL error", msg, QMessageBox::Ok);
    #else
    MessageBoxA(NULL, msg.c_str(), "SDL error", MB_OK|MB_ICONWARNING);
    #endif
}

#ifndef MUSPLAY_USE_WINAPI
int main(int argc, char *argv[])
{
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    (void)(hPrevInstance);
    (void)(lpCmdLine);
    INITCOMMONCONTROLSEX icc;
    // Initialise common controls.
    icc.dwSize = sizeof(icc);
    icc.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icc);
#endif

    #ifndef MUSPLAY_USE_WINAPI
    QApplication::addLibraryPath( "." );
    QApplication::addLibraryPath( QFileInfo(QString::fromUtf8(argv[0])).dir().path() );
    QApplication::addLibraryPath( QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path() );
    #endif

    #ifndef MUSPLAY_USE_WINAPI
    PGE_Application a(argc, argv);
    QStringList args=a.arguments();
    SingleApplication *as = new SingleApplication(args);
    if(!as->shouldContinue())
    {
        QTextStream(stdout) << "SDL2 Mixer X Player already runned!\n";
        delete as;
        return 0;
    }
    #ifdef Q_OS_LINUX
    a.setStyle("GTK");
    #endif
    #endif

    if(SDL_Init(SDL_INIT_AUDIO)==-1)
        error(SDL_GetError());

    if(Mix_Init(MIX_INIT_FLAC | MIX_INIT_MP3 | MIX_INIT_OGG | MIX_INIT_MODPLUG )==-1)
        error(Mix_GetError());

    #ifndef MUSPLAY_USE_WINAPI
    qDebug() << QString(a.applicationDirPath()+"/timidity/");
    MIX_Timidity_addToPathList(QString(a.applicationDirPath()+"/timidity/").toLocal8Bit().data());
    #else
    MIX_Timidity_addToPathList("./timidity/");
    #endif

    if(Mix_OpenAudio(44100, AUDIO_S16, 2, 4096)==-1)
        error(Mix_GetError());

    //Disallow auto-resetting MIDI properties (to allow manipulation with MIDI settings by functions)
    MIX_SetLockMIDIArgs(1);

    #ifndef MUSPLAY_USE_WINAPI
    MainWindow w;
    #else
    MainWindow w(hInstance, nCmdShow);
    #endif

#ifndef MUSPLAY_USE_WINAPI
    //Set acception of external file openings
    w.connect(as, SIGNAL(openFile(QString)), &w, SLOT(openMusicByArg(QString)));
#ifdef __APPLE__
    w.connect(&a, SIGNAL(openFileRequested(QString)), &w, SLOT(openMusicByArg(QString)));
    a.setConnected();
#endif

    w.show();
    if(a.arguments().size()>1)
        w.openMusicByArg(a.arguments()[1]);
    #ifdef __APPLE__
    {
        QStringList argx = a.getOpenFileChain();
        if(!argx.isEmpty())
            w.openMusicByArg(argx[0]);
    }
    #endif
    int result = a.exec();
    delete as;
#else
    int result = 0;
    w.exec();
#endif
    SDL_Quit();
    return result;
}
