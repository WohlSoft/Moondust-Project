/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2015 Vitaly Novichkov <admin@wohlnet.ru>
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

void MainWindow::setDefLang()
{
    /*
     * //Small test from https://qt-project.org/wiki/How_to_create_a_multi_language_application
     */
    WriteToLog(QtDebugMsg, QString("Lang->Translator loaging...."));

    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf('_'));

    QString inifile = AppPathManager::settingsFile();
    QSettings settings(inifile, QSettings::IniFormat);

    settings.beginGroup("Main");
            GlobalSettings::locale = settings.value("language", defaultLocale).toString();
    settings.endGroup();

    WriteToLog(QtDebugMsg, QString("Lang->config was loaded"));

    WriteToLog(QtDebugMsg, QString("Lang->Setting slot...."));
    connect(ui->menuLanguage, SIGNAL(triggered(QAction *)), this, SLOT(slotLanguageChanged(QAction *)));
    WriteToLog(QtDebugMsg, QString("Lang->set"));

       m_langPath = ApplicationPath;
       m_langPath.append("/languages");

       langListSync();

       m_currLang = GlobalSettings::locale;
       QLocale locale = QLocale(m_currLang);
       QLocale::setDefault(locale);

       bool ok = m_translator.load(m_langPath + QString("/editor_%1.qm").arg(m_currLang));
                WriteToLog(QtDebugMsg, QString("Translation: %1").arg((int)ok));
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

           langListSync();
       }
       qDebug() << "Common Translation: " << ok;

       ok = m_translatorQt.load(m_langPath + QString("/qt_%1.qm").arg(m_currLang));
                WriteToLog(QtDebugMsg, QString("Qt Translation: %1").arg((int)ok));
       if(ok)
        qApp->installTranslator(&m_translatorQt);

       qDebug() << "Qt Translation: " << ok;

       ui->retranslateUi(this);
}

void MainWindow::langListSync()
{
    // format systems language
    ui->menuLanguage->clear();

    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("editor_*.qm"));
    for (int i = 0; i < fileNames.size(); ++i)
        {
            // get locale extracted by filename
            QString locale;
            locale = fileNames[i];                  // "TranslationExample_de.qm"
            locale.truncate(locale.lastIndexOf('.'));   // "TranslationExample_de"
            locale.remove(0, locale.indexOf('_') + 1);   // "de"

            QString lang = QLocale::languageToString(QLocale(locale).language());
            QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

            QAction *action = new QAction(ico, lang, this);
            action->setCheckable(true);
            action->setData(locale);

            WriteToLog(QtDebugMsg, QString("Locale: %1 %2").arg(m_langPath).arg(locale));

            ui->menuLanguage->addAction(action);

            if (GlobalSettings::locale == locale)
            {
                action->setChecked(true);
            }
        }

    if(fileNames.size()==0)
    {
        QAction *action = ui->menuLanguage->addAction("[translations not found]");
        action->setCheckable(false);
        action->setDisabled(true);
    }

}

void MainWindow::slotLanguageChanged(QAction* action)
{
    WriteToLog(QtDebugMsg, QString("Translation->SlotStarted"));
    if(0 != action)
    {
        // load the language depending on the action content
        GlobalSettings::locale = m_currLang;

        loadLanguage(action->data().toString());

        DevConsole::retranslate();

        //Retranslate each opened sub-window / tab
        QList<QMdiSubWindow*> subWS = ui->centralWidget->subWindowList();
        foreach(QMdiSubWindow* w, subWS)
        {
            if(QString(w->widget()->metaObject()->className())==LEVEL_EDIT_CLASS)
            {
                QString backup = qobject_cast<LevelEdit*>(w->widget())->windowTitle();
                qobject_cast<LevelEdit*>(w->widget())->ui->retranslateUi(qobject_cast<LevelEdit*>(w->widget()));
                qobject_cast<LevelEdit*>(w->widget())->setWindowTitle(backup);
            }
            else
            if(QString(w->widget()->metaObject()->className())==WORLD_EDIT_CLASS)
            {
                QString backup = qobject_cast<WorldEdit*>(w->widget())->windowTitle();
                qobject_cast<WorldEdit*>(w->widget())->ui->retranslateUi(qobject_cast<WorldEdit*>(w->widget()));
                qobject_cast<WorldEdit*>(w->widget())->setWindowTitle(backup);
            }
            else
            if(QString(w->widget()->metaObject()->className())==NPC_EDIT_CLASS)
            {
                QString backup = qobject_cast<NpcEdit*>(w->widget())->windowTitle();
                qobject_cast<NpcEdit*>(w->widget())->ui->retranslateUi(qobject_cast<NpcEdit*>(w->widget()));
                qobject_cast<NpcEdit*>(w->widget())->setWindowTitle(backup);
            }
        }
    }
}

bool MainWindow::switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);
    // load the new translator
    bool ok = translator.load(filename);
    WriteToLog(QtDebugMsg, QString("Translation: %1 %2").arg((int)ok).arg(filename));

    if(ok)
        qApp->installTranslator(&translator);
    WriteToLog(QtDebugMsg, QString("Translation-> changed"));
    return ok;
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        GlobalSettings::locale = m_currLang;

        QLocale::setDefault(locale);

        QString languageName = QLocale::languageToString(locale.language());

        bool ok  = switchTranslator(m_translatorQt, m_langPath + QString("/qt_%1.qm").arg(m_currLang));
            if(ok)
                qApp->installTranslator(&m_translatorQt);
            qDebug() << "Qt Translation: " << ok;
             ok  = switchTranslator(m_translator, m_langPath + QString("/editor_%1.qm").arg(m_currLang));
            qDebug() << "Common Translation: " << ok;


        WriteToLog(QtDebugMsg, QString("Translation-> try to retranslate"));

        if(ok)
        {
            //Retranslate UI of Main Window
            ui->retranslateUi(this);

            //Retranslate dock widgets
            emit languageSwitched();

            WriteToLog(QtDebugMsg, QString("Translation-> done"));
        }
        else
            WriteToLog(QtDebugMsg, QString("Translation-> not changed (not okay)"));

        //Sync dynamic menus
        SyncRecentFiles();
        updateWindowMenu();
        langListSync();

        dock_LvlItemBox->clearFilter();

        ui->statusBar->showMessage(tr("Current Language changed to %1").arg(languageName), 2000);
    }
}


