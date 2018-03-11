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

#include <locale>

#include "translator.h"
#include "app_path.h"
#include "logger.h"
#ifdef _WIN32
#include <windows.h>
#endif
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#endif

#include "fmt_format_ne.h"

static PGE_Translator *g_translator = NULL;

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
    	pLogCritical("Can't recogonize locale by std::locale: %s", err.what());
    	defaultLocale = "en";
    }
    catch(...)
    {
    	pLogCritical("Can't recogonize locale by std::locale: Unknown error");
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

void PGE_Translator::toggleLanguage(std::string lang)
{
    if(!m_isInit || (m_currLang != lang))
    {
        if(m_isInit)
            m_translator.close();

        m_currLang = lang;

        std::string langFilePath = m_langPath + fmt::format_ne("/engine_{0}.qm", m_currLang);

        bool ok = m_translator.loadFile(langFilePath.c_str(),
                                        reinterpret_cast<unsigned char *>(&m_langPath[0]));
        if(!ok)
        {
            m_currLang = "en"; //set to English if no other translations are found
            langFilePath = m_langPath + fmt::format_ne("/engine_{0}.qm", m_currLang);
            m_translator.loadFile(langFilePath.c_str(),
                                  reinterpret_cast<unsigned char *>(&m_langPath[0]));
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
    std::string out = g_translator->m_translator.do_translate8(0, string, 0, -1);
    if(out.empty())
        return std::string(string);
    else
        return out;
}
