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

#include <fmt/fmt_format.h>

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

    std::locale the_global_locale("");
    std::string defaultLocale = the_global_locale.name();
    defaultLocale.erase(defaultLocale.begin() + defaultLocale.find_last_of('_'), defaultLocale.end());

    m_langPath = ApplicationPathSTD;
    m_langPath.append("/languages");
    pLogDebug("Initializing translator in the path: %s", m_langPath.c_str());
    toggleLanguage(defaultLocale);
    pLogDebug("Locale detected: %s", m_currLang.c_str());
}

void PGE_Translator::toggleLanguage(std::string lang)
{
    if(!m_isInit || (m_currLang != lang))
    {
        if(m_isInit)
            m_translator.close();

        m_currLang = lang;

        bool ok = m_translator.loadFile((m_langPath + fmt::format("/engine_{0}.qm", m_currLang)).c_str(),
                                        reinterpret_cast<unsigned char *>(&m_langPath[0]));
        if(!ok)
        {
            m_currLang = "en"; //set to English if no other translations are found
            ok = m_translator.loadFile((m_langPath + fmt::format("/engine_{0}.qm", m_currLang)).c_str(),
                                       reinterpret_cast<unsigned char *>(&m_langPath[0]));
        }
        m_isInit = true;
    }
}

std::string qsTrId(const char* string)
{
    if(!g_translator)
        return string;
    std::string out = g_translator->m_translator.do_translate8(0, string, 0, -1);
    if(out.empty())
        return std::string(string);
    else
        return out;
}
