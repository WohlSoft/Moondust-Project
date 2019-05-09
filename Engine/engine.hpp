/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2019 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This software is licensed under a dual license system (MIT or GPL version 3 or later).
 * This means you are free to choose with which of both licenses (MIT or GPL version 3 or later)
 * you want to use this software.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You can see text of MIT license in the LICENSE.mit file you can see in Engine folder,
 * or see https://mit-license.org/.
 *
 * You can see text of GPLv3 license in the LICENSE.gpl3 file you can see in Engine folder,
 * or see <http://www.gnu.org/licenses/>.
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

#include <graphics/gl_renderer.h>
#include <common_features/episode_state.h>

#define INITIAL_WINDOW_TITLE \
    "Moondust Engine - v" V_FILE_VERSION V_FILE_RELEASE \
    " build " V_BUILD_VER "-" V_BUILD_BRANCH " (Arch: " FILE_CPU ")"

enum Level_returnTo
{
    RETURN_TO_MAIN_MENU = 0,
    RETURN_TO_WORLDMAP,
    RETURN_TO_GAMEOVER_SCREEN,
    RETURN_TO_CREDITS_SCREEN,
    RETURN_TO_EXIT
};

struct PGEEngineCmdArgs
{
    PGEEngineCmdArgs();
    //! Debug mode is enabled
    bool debugMode;
    //! Audio subsystem is enabled
    bool audioEnabled;
    //! Do episode testing and exit
    bool testWorld;
    //! Do single level testing and exit
    bool testLevel;
    //! Number of players in the testing
    int  test_NumPlayers;
    //! Playable character ID per each player
    int  test_Characters[4];
    //! Character state ID per each player
    int  test_States[4];
    //! Renderer type (OpenGL or software)
    GlRenderer::RenderEngineType rendererType;
    /*!
     * \brief Apply test settings to the game state
     * \param state Reference to the current episode state
     */
    void applyTestSettings(EpisodeState &state);
};

//! Global flags states configured from the command line
extern PGEEngineCmdArgs     g_flags;
//! Target scene after exiting from the level scene
extern Level_returnTo       g_jumpOnLevelEndTo;
//! Path to config pack configured from the command line
extern std::string          g_configPackPath;
//! Path to the file to open (requested from a command line)
extern std::string          g_fileToOpen;
//! Global game state
extern EpisodeState         g_GameState;
//! Episode state meta data
extern PlayEpisodeResult    g_Episode;

class PGE_Translator;

/**
 * @brief Global manager of Moondust Engine's modules
 */
class PGEEngineApp
{
private:
    /**
     * @brief Flags of module states
     */
    enum LoadedModules
    {
        //! No modules loaded
        NOTHING         = 0x000,
        //! Config manager is initialized
        CONFIG_MANAGER  = 0x001,
        //! libSDL is initialized
        LIBSDL          = 0x002,
        //! libSDL_mixer is initialized
        LIBSDLMIXER     = 0x004,
        //! Audio subsystem is initialized
        AUDIO_ENGINE    = 0x008,
        //! Application settings are loaded
        SETTINGS_LOADED = 0x010,
        //! Interprocess subsystem is enabled
        INTERPROCESSOR  = 0x020,
        //! Font manager is initialized
        FONT_MANAGER    = 0x040,
        //! Game window is created
        PGE_WINDOW      = 0x080,
        //! Logger is initalized
        LOGGER          = 0x100,
        //! Translation subsystem is initialized
        TRANSLATOR      = 0x200,
        //! Joysticks subsystem is initialized
        JOYSTICKS       = 0x400
    } m_lib;

    /* Constructed instances */
    ////! Qt Application instance
    //PGE_Application    *m_qApp;
    //! List of command line arguments
    //QStringList         m_args;
    //! Translator instance
    PGE_Translator     *m_tr;
    //! Audio loading erro string
    std::string         m_audioError;
    /*************************/

    /**
     * @brief Is module enabled
     * @param lib ID of module
     * @return true if module is marked as enabled
     */
    inline bool enabled(LoadedModules lib)
    {
        return (0 != (m_lib & lib));
    }

    /**
     * @brief Change mark of module state to "enabled"
     * @param lib ID of module
     */
    inline void enable(LoadedModules lib)
    {
        m_lib = static_cast<LoadedModules>(m_lib | lib);
    }

    /**
     * @brief Change mark of module state to "disabled"
     * @param lib ID of module
     */
    inline void disable(LoadedModules lib)
    {
        m_lib = static_cast<LoadedModules>(m_lib & (~lib));
    }

public:

    PGEEngineApp();

    ~PGEEngineApp();

    /**
     * @brief Unload all loaded modules
     */
    void unloadAll();

    /**
     * @brief Initialize Qt Application instance
     * @param argc Count of command line arguments
     * @param argv Pointer to array of command line arguments
     * @return pointer to instance of Qt Application sub-system. Null if failed to initalize Qt Application
     */
    //PGE_Application *loadQApp(int &argc, char **argv);

    /**
     * @brief Command line arguments
     * @return Reference to list of command line arguments
     */
    //QStringList &args()
    //{
    //    return m_args;
    //}

    /**
     * @brief Initialize translation sub-system
     * @return Reference to loaded translator sub-system
     */
    PGE_Translator &loadTr();

    /**
     * @brief Initialize interprocessing subsystem
     */
    void initInterprocessor();

    /**
     * @brief Initialize SDL library
     * @return true if initializing of SDL was failed
     */
    bool initSDL();

    /**
     * @brief Initialize audio subsystem if needed
     * @param useAudio true if audio subsystem is needed. False will results continuation without audio.
     * @return true if error has occouped
     */
    bool initAudio(bool useAudio);

    /**
     * @brief Error message of audio subsystem initializing
     * @return string with the message text
     */
    std::string errorAudio();

    /**
     * @brief Create game window
     * @param title Initial title of game window
     * @param renderType renderer type to use
     * @return true if creating of window was failed
     */
    bool initWindow(std::string title, int renderType);

    /**
     * @brief Initialize basics of the font manager
     */
    void initFontBasics();

    /**
     * @brief Initialize complete font manager sub-system
     */
    void initFontFull();

    /**
     * @brief Mark config manager as enabled module
     */
    void enableConfigManager()
    {
        enable(CONFIG_MANAGER);
    }

    /**
     * @brief Load application settings
     */
    void loadSettings();

    /**
     * @brief Load joysticks and joystick settings
     */
    void loadJoysticks();

    /**
     * @brief Initialize logging sub-system
     */
    void loadLogger();

    /**
     * @brief Parse low arguments (before initializing of any sub-system)
     * @param argc Count of command line arguments
     * @param argv Pointer to array of command line arguments
     * @return true if exit from application is requested
     */
    static bool parseLowArgs(const std::vector<std::string> &args);

    /**
     * @brief Parse hight arguments (after initializing of some sub-systems)
     */
    void parseHighArgs(const std::vector<std::string> &args);

    /**
     * @brief Create configs diretory if that is not exists
     */
    void createConfigsDir();
};

#endif // ENGINE_HPP
