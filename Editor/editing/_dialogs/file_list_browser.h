#pragma once
#ifndef FILE_LIST_BROWSER_H
#define FILE_LIST_BROWSER_H

#include <QFileDialog>

class QToolButton;

class FileListBrowser : public QObject
{
    Q_OBJECT

    void buildFileList();
public:
    explicit FileListBrowser(QString searchDirectory, QString currentFile = QString(), QWidget *parent = nullptr);
    ~FileListBrowser();

    void setDirectoryRoot(const QString &root);
    void setDirectoryRelation(const QString &rel);

    void setFilters(const QString &filterName, const QStringList &filters);
    void setIcon(const QIcon &icon);
    void setWindowTitle(const QString &title);

    QString currentFile();

    int exec();

private slots:
    void directoryEntered(const QString& directory);

private:
    QString m_currentFile;
    QString m_currentRoot;
    QString m_currentRelation;
    QFileDialog m_dialog;
    QToolButton *m_upButton;
    bool m_sfxMode = false;
};

#endif // FILE_LIST_BROWSER_H
