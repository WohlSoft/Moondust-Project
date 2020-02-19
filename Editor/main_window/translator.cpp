/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2020 Vitaly Novichkov <admin@wohlnet.ru>
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

#include <common_features/app_path.h>
#include <common_features/logger_sets.h>
#include <dev_console/devconsole.h>
#include <main_window/global_settings.h>

#include <ui_mainwindow.h>
#include <mainwindow.h>

#include <main_window/dock/toolboxes.h>

//#include <ui_lvl_item_properties.h>
//#include <ui_tileset_item_box.h>
//#include <ui_lvl_warp_props.h>
//#include <ui_lvl_sctc_props.h>
//#include <ui_lvl_item_toolbox.h>

#include <ui_leveledit.h>
#include <ui_world_edit.h>
#include <ui_npcedit.h>

static QString makeLangFilePath(const QString &langPath, const QString &lang)
{
    return langPath + QString("/editor_%1.qm").arg(lang);
}

static QString makeQtLangFilePath(const QString &langPath, const QString &lang)
{
    return langPath + QString("/qt_%1.qm").arg(lang);
}

static bool langFileExists(const QString &langPath, const QString &lang)
{
    QString p = makeLangFilePath(langPath, lang);
    return QFile::exists(p);
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

void MainWindow::setDefLang()
{
    /*
     * //Small test from https://qt-project.org/wiki/How_to_create_a_multi_language_application
     */
    LogDebug(QString("Lang->Translator loaging...."));

    QString defaultLocale = QLocale::system().name();

    defaultLocale = defaultLocale.replace('_', '-').toLower();
    if(!langFileExists(m_langPath, defaultLocale))
        defaultLocale.truncate(defaultLocale.lastIndexOf('-'));

    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
    {
        GlobalSettings::locale = settings.value("language", defaultLocale).toString();
    }
    settings.endGroup();

    LogDebug(QString("Lang->config was loaded"));

    LogDebug(QString("Lang->Setting slot...."));
    connect(ui->menuLanguage, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));
    LogDebug(QString("Lang->set"));

    m_langPath = AppPathManager::languagesDir();

    langListSync();

    m_currLang = GlobalSettings::locale;
    QLocale locale = QLocale(m_currLang);
    QLocale::setDefault(locale);

    bool ok = m_translator.load(makeLangFilePath(m_langPath, m_currLang));
    LogDebug(QString("Translation: %1").arg((int)ok));
    if(ok)
        qApp->installTranslator(&m_translator);
    else
    {
        m_currLang = "en"; //set to English if no other translations are found
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        ok = m_translator.load(makeLangFilePath(m_langPath, m_currLang));
        if(ok)
            qApp->installTranslator(&m_translator);

        langListSync();
    }
    qDebug() << "Common Translation: " << ok;

    ok = m_translatorQt.load(makeQtLangFilePath(m_langPath, m_currLang));
    LogDebug(QString("Qt Translation: %1").arg((int)ok));
    if(ok)
        qApp->installTranslator(&m_translatorQt);

    qDebug() << "Qt Translation: " << ok;

    qApp->setLayoutDirection(QObject::tr("LTR") == "RTL" ? Qt::RightToLeft : Qt::LeftToRight);

    ui->retranslateUi(this);
    refreshLunaLUAMenuItems();
    connect(this,
            SIGNAL(languageSwitched()),
            this,
            SLOT(refreshLunaLUAMenuItems()));
}

void MainWindow::langListSync()
{
    // format systems language
    ui->menuLanguage->clear();

    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("editor_*.qm"));
    for(int i = 0; i < fileNames.size(); ++i)
    {
        // get locale extracted by filename
        QString locale = getLangName(fileNames[i]);
        QString localeStd = getStdLangName(fileNames[i]);
        QString lang;
        if(locale == "en")
            lang = "English";
        else
        {
            QLocale loc(localeStd);
            lang = QString("%1 (%2)")
                    .arg(loc.nativeLanguageName())
                    .arg(loc.nativeCountryName());
            if(!lang.isEmpty())
                lang[0] = lang[0].toUpper();
        }
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

        LogDebug(QString("Locale: %1 %2").arg(m_langPath).arg(locale));

        ui->menuLanguage->addAction(action);

        if(GlobalSettings::locale == locale)
        {
            action->setChecked(true);
        }
    }

    if(fileNames.size() == 0)
    {
        QAction *action = ui->menuLanguage->addAction("[translations not found]");
        action->setCheckable(false);
        action->setDisabled(true);
    }
}

template<class T>
static void reTranslateWidget(QWidget *w)
{
    T *wnd = qobject_cast<T *>(w);
    Q_ASSERT(wnd);
    wnd->reTranslate();
}

void MainWindow::slotLanguageChanged(QAction *action)
{
    LogDebug(QString("Translation->SlotStarted"));
    if(0 != action)
    {
        // load the language depending on the action content
        GlobalSettings::locale = m_currLang;

        loadLanguage(action->data().toString());

        DevConsole::retranslate();

        //Retranslate each opened sub-window / tab
        QList<QMdiSubWindow *> subWS = ui->centralWidget->subWindowList();
        foreach(QMdiSubWindow *w, subWS)
        {
            QWidget *wd = w->widget();
            QString className = wd->metaObject()->className();
            if(className == LEVEL_EDIT_CLASS)
                reTranslateWidget<LevelEdit>(wd);
            else if(className == WORLD_EDIT_CLASS)
                reTranslateWidget<WorldEdit>(wd);
            else if(className == NPC_EDIT_CLASS)
                reTranslateWidget<NpcEdit>(wd);
        }
    }
}

bool MainWindow::switchTranslator(QTranslator &translator, const QString &filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);
    // load the new translator
    bool ok = translator.load(filename);
    LogDebug(QString("Translation: %1 %2").arg((int)ok).arg(filename));

    if(ok)
    {
        qApp->installTranslator(&translator);
        LogDebug(QString("Translation-> changed"));
    }
    else
    {
        LogDebug(QString("Translation-> wasn't loaded because of error"));
    }
    return ok;
}

void MainWindow::loadLanguage(const QString &rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        GlobalSettings::locale = m_currLang;

        QLocale::setDefault(locale);

        QString languageName = QLocale::languageToString(locale.language());

        bool ok  = switchTranslator(m_translatorQt, makeQtLangFilePath(m_langPath, m_currLang));
        if(ok)
            qApp->installTranslator(&m_translatorQt);
        qDebug() << "Qt Translation: " << ok;
        ok  = switchTranslator(m_translator, makeLangFilePath(m_langPath, m_currLang));
        qDebug() << "Common Translation: " << ok;

        LogDebug(QString("Translation-> try to retranslate"));

        if(ok)
        {
            //Retranslate UI of Main Window
            ui->retranslateUi(this);

            //Retranslate dock widgets
            emit languageSwitched();

            LogDebug(QString("Translation-> done"));
        }
        else
            LogDebug(QString("Translation-> not changed (not okay)"));

        //Sync dynamic menus
        SyncRecentFiles();
        updateWindowMenu();
        langListSync();

        dock_LvlItemBox->clearFilter();

        ui->statusBar->showMessage(tr("Current Language changed to %1").arg(languageName), 2000);
    }
}


