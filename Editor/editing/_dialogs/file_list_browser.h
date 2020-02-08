#pragma once
#ifndef FILE_LIST_BROWSER_H
#define FILE_LIST_BROWSER_H

#include <QDialog>
#include <QListWidgetItem>
#include <QFuture>

namespace Ui {
class FileListBrowser;
}

class FileListBrowser : public QDialog
{
    Q_OBJECT

    void buildFileList();
public:
    explicit FileListBrowser(QString searchDirectory, QString currentFile = QString(), QWidget *parent = nullptr);
    ~FileListBrowser();

    void setDescription(const QString &description);
    void setFilters(const QStringList &filters);
    void setIcon(const QIcon &icon);
    void startListBuilder();

    QString currentFile();

signals:
    void itemAdded(QString item);
    void digFinished();

private slots:
    void addItem(QString item);
    void finalizeDig();

    void on_FileList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

private:
    Ui::FileListBrowser *ui = nullptr;

    QFuture<void> fileWalker;
    QStringList m_filters;

    QString m_currentFile;
    QString m_parentDirectory;
    QString m_lastCurrentFile;
};

#endif // FILE_LIST_BROWSER_H
