#ifndef TRANSLATOR_MAIN_H
#define TRANSLATOR_MAIN_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include <QSharedPointer>

#include "textdata/textdata.h"

class FilesListModel;
class FilesStringsModel;
class TranslateField;
class DialoguesListModel;
class DialogueItem;

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
    void on_actionOpen_project_triggered();
    void on_actionRescan_triggered();
    void on_actionSaveTranslations_triggered();
    void on_actionCloseProject_triggered();
    void on_actionQuit_triggered();

    void on_actionClearRecentProjects_triggered();

private:
    Ui::TranslatorMain *ui;
    TranslateProject m_project;
    FilesListModel *m_filesListModel = nullptr;
    FilesStringsModel *m_filesStringsModel = nullptr;
    DialoguesListModel *m_dialoguesListModel = nullptr;
    QSettings m_setup;
    QString m_recentPath;
    QVector<QString> m_recentProjects;
    QString m_currentPath;
    QString m_curTranslation;
    QString m_recentLang = "origin";
    QString *m_sourceNotePtr = nullptr;
    QMap<QString, QSharedPointer<TranslateField>> m_translateFields;
    QVector<QSharedPointer<DialogueItem>> m_dialogueItems;

    void loadSetup();
    void saveSetup();

    void openProject(const QString &d);

    void resetTranslationFields();
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
