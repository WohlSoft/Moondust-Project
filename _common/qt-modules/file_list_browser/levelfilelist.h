#pragma once
#ifndef LEVELFILELIST_H
#define LEVELFILELIST_H

#include "file_list_browser.h"

class LevelFileList : public FileListBrowser
{
    Q_OBJECT

public:
    explicit LevelFileList(QString searchDirectory, QString currentFile = QString(), QWidget *parent = nullptr);
    ~LevelFileList();
};

#endif // LEVELFILELIST_H
