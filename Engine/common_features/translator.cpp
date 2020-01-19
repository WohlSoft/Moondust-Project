/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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
    std::string defaultRegion = "";
    // TODO: Make also recognize the "region"
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
    toggleLanguage(defaultLocale, defaultRegion);
    pLogDebug("Locale detected: %s", m_currLang.c_str());
#ifdef __EMSCRIPTEN__
    printf("Using English language file %s\n", m_langPath.c_str());
    fflush(stdout);
#endif
}

#ifdef __ANDROID__
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

static bool loadTranslationFile(QmTranslatorX &tr, const std::string &path, unsigned char *dirPath)
{
    size_t size = 0;
    uint8_t *array = nullptr;
    bool ret = false;

    array = rwDumpFile(path.c_str(), size);
    if(array)
    {
        ret = tr.loadData(array, size, dirPath);
        SDL_free(array);
    }

    return ret;
}

#else
static bool loadTranslationFile(QmTranslatorX &tr, const std::string &path, unsigned char *dirPath)
{
    return tr.loadFile(path.c_str(), dirPath);
}
#endif

void PGE_Translator::toggleLanguage(std::string lang, std::string region)
{
    if(!m_isInit || (m_currLang != lang) || (m_currRegion != region))
    {
        if(m_isInit)
        {
            m_translator.close();
            m_translatorEn.close();
        }

        m_currLang = lang;
        m_currRegion = region;

        std::string langFileEnPath  = m_langPath + "/engine_en.qm";
        std::string langFilePath    = m_langPath + fmt::format_ne("/engine_{0}.qm", m_currLang);
        std::string langFileRegPath = m_langPath + fmt::format_ne("/engine_{0}-{1}.qm", m_currLang, m_currRegion);
        bool isEnglish = (langFilePath == langFileEnPath);
        unsigned char *dirPath = reinterpret_cast<unsigned char *>(&m_langPath[0]);
        bool ok = false, okEn = false;

        // Loading English separately as a fallback
        okEn = loadTranslationFile(m_translatorEn, langFileEnPath, dirPath);
        if(!okEn)
            pLogWarning("Failed to open English translation file %s!", langFileEnPath.c_str());

        if(!isEnglish)
        {
            // Try "lang-region"
            ok = loadTranslationFile(m_translator, langFileRegPath, dirPath);
            if(!ok)// Try "lang"
                ok = loadTranslationFile(m_translator, langFilePath, dirPath);
            if(!ok)
                pLogWarning("Can't open one of translation files (%s or %s)!", langFilePath.c_str(), langFileRegPath.c_str());
        }

        m_isInit = true;
    }
}

std::string qtTrId(const char* string)
{
    if(!g_translator)
        return string;

    std::string out;

    if(!g_translator->m_translator.isEmpty())
    {
        out = g_translator->m_translator.do_translate8(nullptr, string, nullptr, -1);
        if(!out.empty())
            return out;
    }

    if(g_translator->m_translatorEn.isEmpty())
    {
        out = g_translator->m_translatorEn.do_translate8(nullptr, string, nullptr, -1);
        if(!out.empty())
            return out;
    }

    return std::string(string);
}
