/*
 * Moondust, a free game engine for platform game making
 * Copyright (c) 2014-2018 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef PGE_TRANSLATOR_H
#define PGE_TRANSLATOR_H

#include "QTranslatorX/qm_translator.h"

class PGE_Translator
{
public:
    PGE_Translator();
    void init();
    void toggleLanguage(std::string lang);
private:
    friend std::string qtTrId(const char* string);

    bool            m_isInit;
    //QTranslator     m_translator;   /**< contains the translations for this application */
    //QTranslator     m_translatorQt; /**< contains the translations for qt */
    QmTranslatorX   m_translator;   /**< contains the translations for this application */
    std::string     m_currLang;     /**< contains the currently loaded language */
    std::string     m_langPath;     /**< Path of language files. This is always fixed to /languages. */
};

#endif // PGE_TRANSLATOR_H
