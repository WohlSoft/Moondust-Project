#pragma once
#ifndef FILE_LIST_BROWSER_H
#define FILE_LIST_BROWSER_H

#include <QDialog>
#include <QListWidgetItem>
#include <QFuture>

class FileListModel;

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

    void setExtraWidgetLayout(QLayout *layout);

    virtual QString currentFile();
    QString currentSelectedFile();

signals:
    void itemAdded(const QString &item);
    void itemSelected(const QString &item);
    void digFinished();

private slots:
    void addItem(const QString &item);
    void finalizeDig();

    void fileListItem_doubleClicked(const QModelIndex &item);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void filterUpdated(const QString &filter);

protected:
    QString directoryPath() const;

private:
    Ui::FileListBrowser *ui = nullptr;

    QFuture<void> fileWalker;
    QStringList m_filters;

    FileListModel *m_listModel = nullptr;

    QString m_currentFile;
    QString m_parentDirectory;
    QString m_lastCurrentFile;
};

#endif // FILE_LIST_BROWSER_H
