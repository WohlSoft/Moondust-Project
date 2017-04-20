#pragma once
#ifndef LEVELFILELIST_H
#define LEVELFILELIST_H

#include <QDialog>
#include <QListWidgetItem>
#include <QFuture>

namespace Ui {
class LevelFileList;
}

class LevelFileList : public QDialog
{
    Q_OBJECT

public:
    explicit LevelFileList(QString Folder, QString current="", QWidget *parent = 0);
    ~LevelFileList();
    QString SelectedFile;

    void buildLevelList();
    QString parentFolder;
    QString lastCurrentFile;

private slots:
    void addItem(QString item);
    void finalizeDig();

    void on_FileList_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();
    void on_buttonBox_rejected();


signals:
    void itemAdded(QString item);
    void digFinished();

private:
    QFuture<void> fileWalker;
    Ui::LevelFileList *ui;
};

#endif // LEVELFILELIST_H
