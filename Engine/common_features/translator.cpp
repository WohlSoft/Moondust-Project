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

#include <locale>

#include "translator.h"
#include "app_path.h"
#include "logger.h"

#ifdef __ANDROID__
#   include <SDL2/SDL_rwops.h>
#   include <SDL2/SDL_assert.h>
#   include <android/log.h>
#endif

#ifdef _WIN32
#   include <windows.h>
#endif

#ifdef __APPLE__
#   include <CoreFoundation/CoreFoundation.h>
#   include <CoreServices/CoreServices.h>
#endif

#include "fmt_format_ne.h"

static PGE_Translator *g_translator = nullptr;

PGE_Translator::PGE_Translator() :
    m_isInit(false),
    m_currLang("en"),
    m_langPath("./languages/")
{
    g_translator = this;
}

void PGE_Translator::init()
{
    if(m_isInit)
        return;
    std::string defaultLocale = "en";
#if defined(_WIN32)
    // Win32 way
    LCID locale = GetSystemDefaultLCID();
    char langStr[21];
    memset(langStr, 0, 21);
    GetLocaleInfoA(locale, LOCALE_SISO639LANGNAME, langStr, 20);
    defaultLocale.clear();
    defaultLocale.append(langStr);
#elif defined(__APPLE__)
    // Cocoa way
    CFLocaleRef cflocale = CFLocaleCopyCurrent();
    CFStringRef value = (CFStringRef)CFLocaleGetValue(cflocale, kCFLocaleLanguageCode);
    CFIndex length  = CFStringGetLength(value);
    CFIndex maxSize = CFStringGetMaximumSizeForEncoding(length, kCFStringEncodingUTF8) + 1;
    char *buffer = (char *)malloc(maxSize);
    if(CFStringGetCString(value, buffer, maxSize, kCFStringEncodingUTF8))
    {
        defaultLocale = std::string(buffer);
    } else {
        pLogWarning("COCOA: Failed to retreive language code");
    }
    free(buffer);
    CFRelease(cflocale);
#elif defined(__EMSCRIPTEN__)
    defaultLocale = "en";
#else
    // Generic way
    try
    {
        std::locale the_global_locale("");
        defaultLocale = the_global_locale.name();
        if(defaultLocale.size() > 2)
            defaultLocale.erase(defaultLocale.begin() + defaultLocale.find_last_of('_'), defaultLocale.end());
        else if(defaultLocale == "C")
            defaultLocale = "en";
    }
    catch(const std::runtime_error &err)
    {
        pLogCritical("Can't recognize locale by std::locale: %s", err.what());
        defaultLocale = "en";
    }
    catch(...)
    {
        pLogCritical("Can't recognize locale by std::locale: Unknown error");
        defaultLocale = "en";
    }
#endif

    m_langPath = AppPathManager::languagesDir();
    pLogDebug("Initializing translator in the path: %s", m_langPath.c_str());
    toggleLanguage(defaultLocale);
    pLogDebug("Locale detected: %s", m_currLang.c_str());
#ifdef __EMSCRIPTEN__
    printf("Using English language file %s\n", m_langPath.c_str());
    fflush(stdout);
#endif
}

static uint8_t *rwDumpFile(const char *path, size_t &size)
{
    SDL_RWops *op = SDL_RWFromFile(path, "rb");
    if(op)
    {
        SDL_RWseek(op, 0, RW_SEEK_END);
        size = static_cast<size_t>(SDL_RWtell(op));
        SDL_RWseek(op, 0, RW_SEEK_SET);
        uint8_t *data = (uint8_t *) SDL_malloc(size);
        SDL_assert_release(data);
        SDL_RWread(op, data, 1, size);
        SDL_RWclose(op);
        return data;
    }
    return nullptr;
}

void PGE_Translator::toggleLanguage(std::string lang)
{
    if(!m_isInit || (m_currLang != lang))
    {
        if(m_isInit)
            m_translator.close();

        m_currLang = lang;

        std::string langFilePath = m_langPath + fmt::format_ne("/engine_{0}.qm", m_currLang);
#ifdef __ANDROID__
        bool ok = false;
        size_t size = 0;
        uint8_t *array = rwDumpFile(langFilePath.c_str(), size);
        if(array)
        {
            ok = m_translator.loadData(array, size,
                                       reinterpret_cast<unsigned char *>(&m_langPath[0]));
            SDL_free(array);
            if(!ok)
            {
                __android_log_print(ANDROID_LOG_WARN, "TRACKERS",
                        "Failed to open translation file %s!",
                        langFilePath.c_str());
            }
        }
        else
        {
            __android_log_print(ANDROID_LOG_WARN, "TRACKERS", "Can't open translation file %s!", langFilePath.c_str());
        }
#else
        bool ok = m_translator.loadFile(langFilePath.c_str(),
                                        reinterpret_cast<unsigned char *>(&m_langPath[0]));
#endif
        if(!ok)
        {
            m_currLang = "en"; //set to English if no other translations are found
            langFilePath = m_langPath + fmt::format_ne("/engine_{0}.qm", m_currLang);

#ifdef __ANDROID__
            bool enOk = false;
            size_t enSize = 0;
            uint8_t *enData = rwDumpFile(langFilePath.c_str(), enSize);
            if(enData)
            {
                enOk = m_translator.loadData(enData, enSize,
                                           reinterpret_cast<unsigned char *>(&m_langPath[0]));
                SDL_free(enData);
                if(!enOk)
                {
                    __android_log_print(ANDROID_LOG_WARN, "TRACKERS",
                                        "Failed to open English translation file %s!",
                                        langFilePath.c_str());
                }
            }
#else
            m_translator.loadFile(langFilePath.c_str(),
                                  reinterpret_cast<unsigned char *>(&m_langPath[0]));
#endif
            #ifdef __EMSCRIPTEN__
                printf("Loading language file %s\n", langFilePath.c_str());
                fflush(stdout);
            #endif
        }
        m_isInit = true;
    }
}

std::string qtTrId(const char* string)
{
    if(!g_translator)
        return string;
    std::string out = g_translator->m_translator.do_translate8(nullptr, string, nullptr, -1);
    if(out.empty())
        return std::string(string);
    else
        return out;
}
