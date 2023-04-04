#ifndef TRANSLATOR_MAIN_H
#define TRANSLATOR_MAIN_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>

#include "textdata/textdata.h"

class FilesListModel;
class FilesStringsModel;


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
    FilesStringsModel * m_filesStringsModel = nullptr;
    QSettings m_setup;
    QString m_recentPath;
    QString m_currentPath;
    QString m_curTranslation;

    void loadSetup();
    void saveSetup();

    void updateActions();
};

#endif // TRANSLATOR_MAIN_H
