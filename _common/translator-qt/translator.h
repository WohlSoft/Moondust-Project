/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2025 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef TRANSLATOR_H
#define TRANSLATOR_H

#include <QObject>
#include <QTranslator>

class QMenu;
class QAction;

class Translator : public QObject
{
    Q_OBJECT
public:
    explicit Translator(QObject *parent = nullptr);
    virtual ~Translator();

    void setSettings(QMenu *menu,
                    const QString &fileNamePrefix,
                    const QString &languagesDir,
                    const QString &settingsFile);

    /*!
     * \brief Init settings of the translator and initialize default language
     */
    void initTranslator();

private:
    /*!
     * \brief Load language by two-letter code (en, ru, de, it, es, etc.)
     * \param rLanguage Two-letter language name (en, ru, de, it, es, etc.)
     */
    void loadLanguage(const QString& rLanguage);
    /*!
     * \brief Switch translator module
     * \param translator Translator module
     * \param filename Path to the translation file
     * \return true if successfully loaded, false on any error
     */
    bool switchTranslator(QTranslator &translator, const QString &filename);
    /*!
     * \brief Syncronize state of the languages menu
     */
    void reloadMenu();

    void loadSettings();
    void saveSettings();

private slots:
    /*!
     * \brief Change language to selected in the language menu
     * \param action Menuitem onject pointer
     */
    void slotLanguageChanged(QAction *action);

private:
    //! Translation filename prefix
    QString         m_trFilePrefix = "app";
    //! Path to settings file to remember language settings
    QString         m_settingsFile;
    //! Menu which should contain a list of languages to toggle
    QMenu          *m_langsMenu = nullptr;
    //! contains the currently loaded language setting
    QString         m_currLang;
    //! contains the translations for this application
    QTranslator     m_trApp;
    //! contains the translations for qt
    QTranslator     m_trQt;
    //! Path of language files. This is always fixed to /languages
    QString         m_langPath;

signals:
    //! Is editor language was switched
    void languageSwitched();
};

#endif // TRANSLATOR_H
