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

private slots:
    void on_actionOpen_project_triggered();
    void on_actionRescan_triggered();
    void on_actionSaveTranslations_triggered();
    void on_actionCloseProject_triggered();
    void on_actionQuit_triggered();

private:
    Ui::TranslatorMain *ui;
    TranslateProject m_project;
    FilesListModel *m_filesListModel = nullptr;
    FilesStringsModel *m_filesStringsModel = nullptr;
    DialoguesListModel *m_dialoguesListModel = nullptr;
    QSettings m_setup;
    QString m_recentPath;
    QString m_currentPath;
    QString m_curTranslation;
    QMap<QString, QSharedPointer<TranslateField>> m_translateFields;
    QVector<QSharedPointer<DialogueItem>> m_dialogueItems;

    void loadSetup();
    void saveSetup();

    void updateActions();
    void updateTranslateFields();
};

#endif // TRANSLATOR_MAIN_H
