/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
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

#include <ctime>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#endif

#include "engine.hpp"

#include <version.h>
#include <graphics/window.h>

#include <audio/pge_audio.h>

#include <Utils/files.h>
#include <DirManager/dirman.h>

#include <common_features/crash_handler.h>
#include <common_features/logger.h>
#include <common_features/translator.h>
#include <common_features/number_limiter.h>

#include <data_configs/config_manager.h>

#include <fontman/font_manager.h>

#include <settings/global_settings.h>
#include <settings/debugger.h>

#include <networking/intproc.h>

PGEEngineCmdArgs    g_flags;

Level_returnTo      g_jumpOnLevelEndTo = RETURN_TO_EXIT;

std::string         g_configPackPath  = "";
std::string         g_fileToOpen  = "";
EpisodeState        g_GameState;
PlayEpisodeResult   g_Episode;

PGEEngineCmdArgs::PGEEngineCmdArgs() :
    debugMode(false),
    audioEnabled(true),
    testWorld(false),
    testLevel(false),
    test_NumPlayers(1),
    test_Characters{ -1, -1, -1, -1 },
    test_States{ 1, 1, 1, 1 },
    rendererType(GlRenderer::RENDER_AUTO)
{}

void PGEEngineCmdArgs::applyTestSettings(EpisodeState &state)
{
    state.numOfPlayers = test_NumPlayers;

    for(int i = 0; i < 4; i++)
    {
        if(test_Characters[i] == -1)
            continue;
        PlayerState st = state.getPlayerState(i + 1);
        st.characterID      = static_cast<uint32_t>(test_Characters[i]);
        st.stateID          = static_cast<uint32_t>(test_States[i]);
        st._chsetup.id      = static_cast<unsigned long>(test_Characters[i]);
        st._chsetup.state   = static_cast<unsigned long>(test_States[i]);
        state.setPlayerState(i + 1, st);
    }
}



PGEEngineApp::PGEEngineApp() :
    m_lib(NOTHING),
    m_tr(nullptr)
{
    CrashHandler::initSigs();
    srand(static_cast<unsigned int>(std::time(NULL)));
}

PGEEngineApp::~PGEEngineApp()
{
    try
    {
        unloadAll();
    }
    catch(...)
    {}
}

void PGEEngineApp::unloadAll()
{
    if(enabled(CONFIG_MANAGER))
    {
        ConfigManager::unluadAll();
        disable(CONFIG_MANAGER);
    }

    if(enabled(AUDIO_ENGINE))
    {
        PGE_Audio::quit();
        disable(AUDIO_ENGINE);
    }

    if(enabled(SETTINGS_LOADED))
    {
        g_AppSettings.save();
        disable(SETTINGS_LOADED);
    }

    if(enabled(JOYSTICKS))
    {
        g_AppSettings.closeJoysticks();
        disable(JOYSTICKS);
    }

    if(enabled(INTERPROCESSOR))
    {
        if(IntProc::isEnabled())
            IntProc::quit();

        disable(INTERPROCESSOR);
    }

    if(enabled(FONT_MANAGER))
    {
        FontManager::quit();
        disable(FONT_MANAGER);
    }

    if(enabled(PGE_WINDOW))
    {
        PGE_Window::uninit();
        disable(PGE_WINDOW);
    }

    if(enabled(LIBSDLMIXER))
    {
        Mix_Quit();
        disable(LIBSDLMIXER);
    }

    if(enabled(LIBSDL))
    {
        SDL_Quit();
        disable(LIBSDL);
    }

    if(enabled(TRANSLATOR))
    {
        delete m_tr;
        m_tr = nullptr;
        disable(TRANSLATOR);
    }

    pLogDebug("<Application closed>");

    if(enabled(LOGGER))
    {
        CloseLog();
        disable(LOGGER);
    }
}

//PGE_Application *PGEEngineApp::loadQApp(int &argc, char **argv)
//{
//    PGE_Application::addLibraryPath(".");
//    //PGE_Application::addLibraryPath(QFileInfo(QString::fromUtf8(argv[0])).dir().path());
//    //PGE_Application::addLibraryPath(QFileInfo(QString::fromLocal8Bit(argv[0])).dir().path());
//    #if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
//    PGE_Application::setAttribute(Qt::AA_EnableHighDpiScaling, false);
//    PGE_Application::setAttribute(Qt::AA_DisableHighDpiScaling, true);
//    #endif
//    PGE_Application::setAttribute(Qt::AA_Use96Dpi);
//    m_qApp = new PGE_Application(argc, argv);

//    if(!m_qApp)
//    {
//        pLogFatal("Can't construct application class!");
//        return nullptr;
//    }

//    m_args = m_qApp->arguments();
//    //! Because Qt Application removes some flags from original argv, need to update the size value
//    argc   = m_args.size();
//    //Generating application path
//    //Init system paths
//    AppPathManager::initAppPath(argv[0]);
//    enable(QAPP);
//    return m_qApp;
//}

PGE_Translator &PGEEngineApp::loadTr()
{
    pLogDebug("Constructing translator...");
    m_tr = new PGE_Translator();
    pLogDebug("Initializing translator...");
    m_tr->init();
    enable(TRANSLATOR);
    pLogDebug("Translator successfully built...");
    return *m_tr;
}

void PGEEngineApp::initInterprocessor()
{
    IntProc::init();
    enable(INTERPROCESSOR);
}

bool PGEEngineApp::initSDL()
{
    bool res = false;
    pLogDebug("Initialization of SDL...");
    Uint32 sdlInitFlags = 0;
    int sdlMixerInitFlags = 0;
    // Prepare flags for SDL initialization
    sdlInitFlags |= SDL_INIT_TIMER;
    sdlInitFlags |= SDL_INIT_AUDIO;
    sdlInitFlags |= SDL_INIT_VIDEO;
    sdlInitFlags |= SDL_INIT_EVENTS;
    sdlInitFlags |= SDL_INIT_JOYSTICK;
    //(Cool thing, but is not needed yet)
    //sdlInitFlags |= SDL_INIT_HAPTIC;
    sdlInitFlags |= SDL_INIT_GAMECONTROLLER;

    sdlMixerInitFlags |= MIX_INIT_FLAC;
    sdlMixerInitFlags |= MIX_INIT_MOD;
    sdlMixerInitFlags |= MIX_INIT_MP3;
    sdlMixerInitFlags |= MIX_INIT_OGG;
    sdlMixerInitFlags |= MIX_INIT_MID;

    // Initialize SDL
    res = (SDL_Init(sdlInitFlags) < 0);

    if(!res)
        enable(LIBSDL);

    const char *error = SDL_GetError();
    if(*error != '\0')
        pLogWarning("Error while SDL Initialization: %s", error);
    SDL_ClearError();

    // Initialize SDL Mixer
    res = (Mix_Init(sdlMixerInitFlags) < 0);
    if(!res)
        enable(LIBSDLMIXER);

    error = SDL_GetError();
    if(*error != '\0')
        pLogWarning("Error while SDL Mixer Initialization: %s", error);
    SDL_ClearError();

    return res;
}

bool PGEEngineApp::initAudio(bool useAudio)
{
    bool ret = false;

    if(!useAudio)
        return true;

    pLogDebug("Initialization of Audio subsystem...");
    ret = (PGE_Audio::init(44100, 32, 4096) == -1);

    if(!ret)
        enable(AUDIO_ENGINE);
    else
        m_audioError = Mix_GetError();

    return ret;
}

std::string PGEEngineApp::errorAudio()
{
    return m_audioError;
}

bool PGEEngineApp::initWindow(std::string title, int renderType)
{
    bool ret = true;
    pLogDebug("Init main window...");
    ret = PGE_Window::init(title, renderType);

    if(ret)
        enable(PGE_WINDOW);

    return !ret;
}

void PGEEngineApp::initFontBasics()
{
    pLogDebug("Init basic font manager...");
    FontManager::initBasic();
    enable(FONT_MANAGER);
}

void PGEEngineApp::initFontFull()
{
    pLogDebug("Init full font manager...");
    FontManager::initFull();
    enable(FONT_MANAGER);
}

void PGEEngineApp::loadSettings()
{
    g_AppSettings.load();
    g_AppSettings.apply();
    enable(SETTINGS_LOADED);
}

void PGEEngineApp::loadJoysticks()
{
    pLogDebug("Init joystics...");
    g_AppSettings.initJoysticks();
    g_AppSettings.loadJoystickSettings();
    enable(JOYSTICKS);
}

void PGEEngineApp::loadLogger()
{
    LoadLogSettings();
    //Write into log the application start event
    pLogDebug("<Application started>");
    enable(LOGGER);
}

static void printUsage(const char *arg0)
{
    std::string arg0s(arg0);
    const char *logo =
        "=================================================\n"
        INITIAL_WINDOW_TITLE "\n"
        "=================================================\n"
        "\n";

    #ifndef _WIN32
#define OPTIONAL_BREAK "\n"
    #else
#define OPTIONAL_BREAK " "
    #endif

    std::string msg(
        "Command line syntax:\n"
        "\n"
        "Start game with a specific config pack:\n"
        "   " + arg0s + " [options] --config=\"./configs/Config Pack Name/\"\n"
        "\n"
        "Play single level or episode:\n"
        "   " + arg0s + " [options] <path to level or world map file>\n"
        "\n"
        #ifndef _WIN32
        "Show application version:\n"
        "   " + arg0s + " --version\n"
        "\n"
        #endif
        "Copy settings into "
        #if defined(_WIN32)
        "%UserProfile%/.PGE_Project/"
        #elif defined(__APPLE__)
        "/Library/Application Support/PGE_Project/"
        #else
        "~/.PGE_Project/"
        #endif
        " folder and use it as" OPTIONAL_BREAK
        "placement of config packs, episodes, for screenshots store, etc.:\n"
        "   " + arg0s + " --install\n"
        "\n"
        #ifndef _WIN32
        "Show this help:\n"
        "   " + arg0s + " --help\n"
        "\n\n"
        #endif
        "Options:\n\n"
        "  --config=\"{path}\"          - Use a specific configuration package\n"
        #if defined(__APPLE__)
        "  --render-[auto|sw|gl2] - Choose a graphical sub-system\n"
        #else
        "  --render-[auto|sw|gl2|gl3] - Choose a graphical sub-system\n"
        #endif
        "             auto - Automatically detect it (DEFAULT)\n"
        #if !defined(__APPLE__)
        "             gl3  - Use OpenGL 3.1 renderer\n"
        #endif
        "             gl2  - Use OpenGL 2.1 renderer\n"
        "             sw   - Use software renderer (may overload CPU)\n"
        "  --render-vsync             - Toggle on VSync if supported on your hardware\n"
        "  --lang=xx                  - Use a specific language (default is locale)\n"
        "            (where xx - a code of language. I.e., en, ru, es, nl, pl, etc.)\n"
        "  --num-players=X            - Start game with X number of players\n"
        "  --pXc=Y                    - Set character Y for player X\n"
        "            (for example --p1c=2 will set character with ID 2 to first player)\n"
        "  --pXs=Y                    - Set character state Y for player X\n"
        "            (for example --p2s=4 will set state with ID 4 to second player)\n"
        "  --debug                    - Enable debug mode\n"
        "  --debug-physics            - Enable debug rendering of the physics\n"
        "  --debug-print=[yes|no]     - Enable/Disable debug information printing\n"
        "  --debug-pagan-god          - Enable god mode\n"
        "  --debug-superman           - Enable unlimited flying up\n"
        "  --debug-chucknorris        - Allow to playable character destroy any objects\n"
        "  --debug-worldfreedom       - Allow to walk everywhere on the world map\n"
        "\n"
        "More detailed information can be found here:\n"
        "http://wohlsoft.ru/pgewiki/PGE_Engine#Command_line_arguments\n"
        OPTIONAL_BREAK);
    /*
    #ifdef _WIN32
    if(AttachConsole(ATTACH_PARENT_PROCESS))
    {
        HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
        int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
        FILE* hf_out = _fdopen(hCrt, "w");
        setvbuf(hf_out, NULL, _IONBF, 1);
        *stdout = *hf_out;

        HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
        hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
        FILE* hf_in = _fdopen(hCrt, "r");
        setvbuf(hf_in, NULL, _IONBF, 128);
        *stdin = *hf_in;
    }
    #endif
    */
    fprintf(stdout, "%s%s", logo, msg.c_str());
}

bool PGEEngineApp::parseLowArgs(const std::vector<std::string> &args)
{
    if(args.size() > 1)
    {
        //Check only first argument
        const std::string &arg = args[1];

        if(arg.compare("--version") == 0)
        {
            std::cout << V_INTERNAL_NAME " " V_FILE_VERSION << V_FILE_RELEASE "-" V_BUILD_VER << "-" << V_BUILD_BRANCH << std::endl;
            std::cout.flush();
            return false;
        }
        else if(arg.compare("--install") == 0)
        {
            //PGEEngineApp  lib;
            //lib.loadQApp(argc, argv);
            //FIXME: Implement installing on STL-only!
            AppPathManager::install();
            AppPathManager::initAppPath();
            return false;
        }
        else if(arg.compare("--help") == 0)
        {
            printUsage(args[0].c_str());
            return false;
        }
    }

    #ifdef _WIN32
    //Close console which is no more needed
    FreeConsole();
    #endif

    return true;
}

static void removeQuotes(char *&s, size_t &len)
{
    if(len > 0)
    {
        if(s[0] == '\"')
        {
            s++;
            len--;
        }

        if((len > 0) && (s[len - 1] == '\"'))
        {
            s[len - 1] = '\0';
            len--;
        }
    }
}

static void findEqual(char *&s, size_t &len)
{
    while(len > 0)
    {
        len--;
        if(*s++ == '=')
            break;
    }
}

static std::string takeStrFromArg(std::string &arg, bool &ok)
{
    std::string target = arg;
    size_t      len = arg.size();
    char        *s = &target[0];

    findEqual(s, len);
    removeQuotes(s, len);
    ok = (len > 0);

    return std::string(s, len);
}

static int takeIntFromArg(std::string &arg, bool &ok)
{
    std::string target = arg;
    size_t  len = arg.size();
    char    *s  = &target[0];

    findEqual(s, len);
    removeQuotes(s, len);

    ok = (len > 0);

    return atoi(s);
}

void PGEEngineApp::parseHighArgs(const std::vector<std::string> &args)
{
    /* Set defaults to global properties */
    g_Episode.character = 0;
    g_Episode.savefile  = "save1.savx";
    g_Episode.worldfile = "";
    g_AppSettings.debugMode         = false; //enable debug mode
    g_AppSettings.interprocessing   = false; //enable interprocessing

    for(size_t pi = 1; pi < args.size(); pi++)
    {
        std::string param_s = args[pi];
        pLogDebug("Argument: [%s]", param_s.c_str());
        int  i = 0;
        char characterParam[8] = "\0";
        char stateParam[8] = "\0";

        for(i = 0; i < 4; i++)
        {
            sprintf(characterParam, "--p%dc=", i + 1);
            sprintf(stateParam, "--p%ds=", i + 1);

            if(param_s.compare(0, 6, characterParam) == 0)
            {
                int tmp;
                bool ok = false;
                tmp = takeIntFromArg(param_s, ok);

                if(ok) g_flags.test_Characters[i] = tmp;

                break;
            }
            else if(param_s.compare(0, 6, stateParam) == 0)
            {
                int tmp;
                bool ok = false;
                tmp = takeIntFromArg(param_s, ok);

                if(ok)
                {
                    g_flags.test_States[i] = tmp;

                    if(g_flags.test_Characters[i] == -1)
                        g_flags.test_Characters[i] = 1;
                }

                break;
            }
        }

        if(i < 4) //If one of pXc/pYs parameters has been detected
            continue;

        if(param_s.compare(0, 9, "--config=") == 0)
        {
            std::string tmp;
            bool ok = false;
            tmp = takeStrFromArg(param_s, ok);
            if(ok)
                g_configPackPath = tmp;
        }
        else if(param_s.compare(0, 14, "--num-players=") == 0)
        {
            int tmp;
            bool ok = false;
            tmp = takeIntFromArg(param_s, ok);

            if(ok) g_flags.test_NumPlayers = tmp;

            //1 or 2 players until 4-players mode will be implemented!
            NumberLimiter::applyD(g_flags.test_NumPlayers, 1, 1, 2);
        }
        else if(param_s.compare("--debug") == 0)
        {
            g_AppSettings.debugMode = true;
            g_flags.debugMode = true;
        }
        else if(param_s.compare("--debug-pagan-god") == 0)
        {
            if(g_flags.debugMode)
                PGE_Debugger::cheat_pagangod = true;
        }
        else if(param_s.compare("--debug-superman") == 0)
        {
            if(g_flags.debugMode)
                PGE_Debugger::cheat_superman = true;
        }
        else if(param_s.compare("--debug-chucknorris") == 0)
        {
            if(g_flags.debugMode)
                PGE_Debugger::cheat_chucknorris = true;
        }
        else if(param_s.compare("--debug-worldfreedom") == 0)
        {
            if(g_flags.debugMode)
                PGE_Debugger::cheat_worldfreedom = true;
        }
        else if(param_s.compare("--debug-physics") == 0)
        {
            if(g_flags.debugMode)
                PGE_Window::showPhysicsDebug = true;
        }
        else if(param_s.compare("--debug-print=yes") == 0)
        {
            if(g_flags.debugMode)
            {
                g_AppSettings.showDebugInfo = false;
                PGE_Window::showDebugInfo = true;
            }
        }
        else if(param_s.compare("--debug-print=no") == 0)
        {
            PGE_Window::showDebugInfo = false;
            g_AppSettings.showDebugInfo = false;
        }
        else if(param_s.compare("--interprocessing") == 0)
        {
            initInterprocessor();
            g_AppSettings.interprocessing = true;
        }
        else if(param_s.compare(0, 7, "--lang=") == 0)
        {
            std::string tmp;
            bool ok = false;
            tmp = takeStrFromArg(param_s, ok);
            ok &= (tmp.size() == 2);
            if(ok)
                m_tr->toggleLanguage(tmp);
        }
        else if(param_s.compare("--render-auto") == 0)
            g_flags.rendererType = GlRenderer::RENDER_AUTO;
        else if(param_s.compare("--render-gl2") == 0)
            g_flags.rendererType = GlRenderer::RENDER_OPENGL_2_1;
        else if(param_s.compare("--render-gl3") == 0)
            g_flags.rendererType = GlRenderer::RENDER_OPENGL_3_1;
        else if(param_s.compare("--render-sw") == 0)
            g_flags.rendererType = GlRenderer::RENDER_SW_SDL;
        else if(param_s.compare("--render-vsync") == 0)
        {
            g_AppSettings.vsync = true;
            PGE_Window::vsync = true;
        }
        else
        {
            char *str = &param_s[0];
            size_t len = param_s.size();
            removeQuotes(str, len);
            if(Files::fileExists(str))
            {
                //QString param = QString::fromUtf8(str, (int)len);
                g_fileToOpen = str;
                pLogDebug("Got file path: [%s]", str);
            }
            else
                pLogWarning("Invalid argument or file path: [%s]", str);
        }
    }
}

void PGEEngineApp::createConfigsDir()
{
    std::string configPath = AppPathManager::userAppDirSTD() + "/" +  "configs";
    DirMan configDir(configPath);
    //Create empty config directory if not exists
    if(!configDir.exists())
        configDir.mkdir(configPath);
}
