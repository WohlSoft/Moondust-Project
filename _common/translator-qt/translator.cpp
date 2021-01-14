/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2021 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <QMdiSubWindow>
#include <QApplication>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QDebug>
#include <QMenu>

#include "translator.h"

Translator::Translator(QObject *parent) :
    QObject(parent)
{}

void Translator::setSettings(QMenu *menu,
                             const QString &fileNamePrefix,
                             const QString &languagesDir,
                             const QString &settingsFile)
{
    m_langPath = languagesDir;
    m_trFilePrefix = fileNamePrefix;
    m_settingsFile = settingsFile;
    m_langsMenu = menu;
    QObject::connect(m_langsMenu, SIGNAL(triggered(QAction *)),
                     this, SLOT(slotLanguageChanged(QAction *)));
}

Translator::~Translator()
{
    if(m_langsMenu)
    {
        QObject::disconnect(m_langsMenu, SIGNAL(triggered(QAction *)),
                            this, SLOT(slotLanguageChanged(QAction *)));
        m_langsMenu = nullptr;
    }
}


static QString makeLangFilePath(const QString &langPath, const QString &prefix, const QString &lang)
{
    return langPath + QString("/%1_%2.qm").arg(prefix).arg(lang);
}

static QString makeQtLangFilePath(const QString &langPath, const QString &lang)
{
    return langPath + QString("/qt_%1.qm").arg(lang);
}

static bool langFileExists(const QString &langPath, const QString &prefix, const QString &lang)
{
    QString p = makeLangFilePath(langPath, prefix, lang);
    return QFile::exists(p);
}

void Translator::initTranslator()
{
    Q_ASSERT(m_langsMenu);  // Menu is required for translator's work

    loadSettings();
    reloadMenu();

    bool ok = m_trApp.load(makeLangFilePath(m_langPath, m_trFilePrefix, m_currLang));
    if(ok)
        qApp->installTranslator(&m_trApp);
    else
    {
        m_currLang = "en"; //set to English if no other translations are found
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        ok = m_trApp.load(makeLangFilePath(m_langPath, m_trFilePrefix, m_currLang));
        if(ok)
            qApp->installTranslator(&m_trApp);
        reloadMenu();
    }

    ok = m_trQt.load(makeQtLangFilePath(m_langPath, m_currLang));
    if(ok)
        qApp->installTranslator(&m_trQt);

    qApp->setLayoutDirection(QObject::tr("LTR") == "RTL" ? Qt::RightToLeft : Qt::LeftToRight);

    emit languageSwitched();
}


static QString getLangName(const QString &fname)
{
    QString locale;
    locale = fname;                             // "TranslationExample_de.qm"
    locale.truncate(locale.lastIndexOf('.'));   // "TranslationExample_de"
    locale.remove(0, locale.indexOf('_') + 1);  // "de"
    return locale;
}

static QString getStdLangName(const QString &fname)
{
    QString locale;
    locale = fname;                             // "TranslationExample_de.qm"
    locale.truncate(locale.lastIndexOf('.'));   // "TranslationExample_de"
    locale.remove(0, locale.indexOf('_') + 1);  // "de"
    locale = locale.replace('-', '_');
    return locale;
}

void Translator::reloadMenu()
{
    // format systems language
    m_langsMenu->clear();

    QDir dir(m_langPath);
    QStringList fileNamesFilter(QString("%1_*.qm").arg(m_trFilePrefix));
    QStringList fileNames = dir.entryList(fileNamesFilter);

    for(QString &f : fileNames)
    {
        // get locale extracted by filename
        QString locale = getLangName(f);
        QString localeStd = getStdLangName(f);
        QString lang;

        if(locale == "en")
        {
            lang = "English";
        }
        else
        {
            QLocale loc(localeStd);
            lang = QString("%1 (%2)")
                    .arg(loc.nativeLanguageName())
                    .arg(loc.nativeCountryName());
            if(!lang.isEmpty())
                lang[0] = lang[0].toUpper();
        }

#ifdef __APPLE__
        QAction *action = new QAction(lang, this);
#else
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));
        QAction *action = new QAction(ico, lang, this);
#endif

        action->setCheckable(true);
        action->setData(locale);

        m_langsMenu->addAction(action);

        if(m_currLang == locale)
        {
            action->setChecked(true);
        }
    }

    if(fileNames.size() == 0)
    {
        QAction *action = m_langsMenu->addAction("[translations not found]");
        action->setCheckable(false);
        action->setDisabled(true);
    }
}

void Translator::loadSettings()
{
    // Small test from https://qt-project.org/wiki/How_to_create_a_multi_language_application
    QString defaultLocale = QLocale::system().name();

    defaultLocale = defaultLocale.replace('_', '-').toLower();
    if(!langFileExists(m_langPath, m_trFilePrefix, defaultLocale))
        defaultLocale.truncate(defaultLocale.lastIndexOf('-'));

    QSettings settings(m_settingsFile, QSettings::IniFormat);

    settings.beginGroup("Main");
    {
        m_currLang = settings.value("language", defaultLocale).toString();
    }
    settings.endGroup();

    QLocale locale = QLocale(m_currLang);
    QLocale::setDefault(locale);
}

void Translator::saveSettings()
{
    QSettings settings(m_settingsFile, QSettings::IniFormat);
    settings.beginGroup("Main");
    {
        settings.setValue("language", m_currLang);
    }
    settings.endGroup();
    settings.sync();
}

void Translator::slotLanguageChanged(QAction *action)
{
    if(nullptr != action)
    {
        // load the language depending on the action content
        QString lang = action->data().toString();
        if(m_currLang != lang)
        {
            loadLanguage(lang);
            saveSettings();
        }
    }
}

bool Translator::switchTranslator(QTranslator &translator, const QString &filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);
    // load the new translator
    bool ok = translator.load(filename);
    if(ok)
        qApp->installTranslator(&translator);
    return ok;
}

void Translator::loadLanguage(const QString &rLanguage)
{
    QLocale locale = QLocale(rLanguage);
    QLocale::setDefault(locale);
    QString languageName = QLocale::languageToString(locale.language());

    bool ok  = switchTranslator(m_trQt, makeQtLangFilePath(m_langPath, rLanguage));
    if(ok)
        qApp->installTranslator(&m_trQt);
    ok  = switchTranslator(m_trApp, makeLangFilePath(m_langPath, m_trFilePrefix, rLanguage));
    if(ok)
    {
        m_currLang = rLanguage;
        qApp->setLayoutDirection(QObject::tr("LTR") == "RTL" ? Qt::RightToLeft : Qt::LeftToRight);
        emit languageSwitched();
    }

    reloadMenu();
}
