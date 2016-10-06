/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QLocale>
#include "translator.h"
#include "app_path.h"
#include "logger.h"

PGE_Translator::PGE_Translator() :
    m_isInit(false)
{}

void PGE_Translator::init()
{
    if(m_isInit)
        return;

    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    m_langPath = ApplicationPath;
    m_langPath.append("/languages");
    LogDebug( "Initializing translator in the path: " + m_langPath );
    LogDebug( "Locale detected: " + m_currLang );
    toggleLanguage(defaultLocale);
}

void PGE_Translator::toggleLanguage(QString lang)
{
    if(!m_isInit || (m_currLang != lang))
    {
        if(m_isInit)
        {
            qApp->removeTranslator(&m_translator);
            qApp->removeTranslator(&m_translatorQt);
        }

        m_currLang = lang;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);

        bool ok = m_translator.load(m_langPath + QString("/engine_%1.qm").arg(m_currLang));

        if(ok)
           qApp->installTranslator(&m_translator);
        else
        {
            m_currLang="en"; //set to English if no other translations are found
            QLocale locale = QLocale(m_currLang);
            QLocale::setDefault(locale);
            ok = m_translator.load(m_langPath + QString("/editor_%1.qm").arg(m_currLang));
            if(ok)
               qApp->installTranslator(&m_translator);
        }

        ok = m_translatorQt.load(m_langPath + QString("/qt_%1.qm").arg(m_currLang));
        if(ok)
            qApp->installTranslator(&m_translatorQt);

        m_isInit = true;
    }
}

