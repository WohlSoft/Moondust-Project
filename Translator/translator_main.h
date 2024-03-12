/*
 * Moondust Translator, a free tool for internationalisation of levels and episodes
 * This is a part of the Moondust Project, a free platform for game making
 * Copyright (c) 2023-2024 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef TRANSLATOR_MAIN_H
#define TRANSLATOR_MAIN_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include <QSharedPointer>
#include <QItemSelection>

#include "textdata/textdata.h"

class FilesListModel;
class FilesStringsModel;
class TranslateField;
class DialoguesListModel;
class DialogueItem;
class LangsListModel;
class SimpleHighlighter;

namespace Ui {
class TranslatorMain;
}

class TranslatorMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit TranslatorMain(QWidget *parent = nullptr);
    ~TranslatorMain();

protected:
    void changeEvent(QEvent *e);
    void closeEvent(QCloseEvent *e);
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void on_actionOpenProject_triggered();
    void on_actionOpenSingleLevel_triggered();
    void on_actionRescan_triggered();
    void on_actionSaveTranslations_triggered();
    void on_actionCloseProject_triggered();
    void on_actionQuit_triggered();

    void on_actionClearRecentProjects_triggered();

    void on_actionAbout_triggered();

    void on_languagesAdd_clicked();

private:
    Ui::TranslatorMain *ui;

    //! Currently opened project data
    TranslateProject m_project;

    // Models
    FilesListModel *m_filesListModel = nullptr;
    FilesStringsModel *m_filesStringsModel = nullptr;
    DialoguesListModel *m_dialoguesListModel = nullptr;
    LangsListModel *m_langsListModel = nullptr;

    // Misc. data
    QSettings m_setup;
    SimpleHighlighter *m_highLighter = nullptr;
    QString m_recentPath;
    QVector<QString> m_recentProjects;
    QString m_currentPath;
    QString m_currentLevel;
    bool    m_isSingleLevel = false;
    QString m_curTranslation;
    QString m_recentLang = "metadata";
    QString *m_sourceNotePtr = nullptr;
    QMap<QString, QSharedPointer<TranslateField>> m_translateFields;
    QVector<QSharedPointer<DialogueItem>> m_dialogueItems;

    void loadSetup();
    void saveSetup();

    void openProject(const QString &d, bool singleLevel = false);

    void resetTranslationFields();
    void syncTranslationFieldsContent();
    void syncTranslationFieldsContent(const QItemSelection &selected, const QItemSelection &);
    void syncDialoguesListLang();
    void syncStringsListStatus();

    void syncDialoguesList(const QItemSelection &selected, const QItemSelection &);

    void updateTranslationFields(const QModelIndex &s);
    void updateTranslationFields(int group,
                                 const QString &root,
                                 int type,
                                 int key,
                                 const QString &text,
                                 const QString &tr_note);

    void updateActions();
    void updateRecent();
    void updateTranslateFields();
};

#endif // TRANSLATOR_MAIN_H
