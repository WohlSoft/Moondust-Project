#pragma once
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QString>
#include <QStringList>

class ConfigPackMiniManager
{
public:
    ConfigPackMiniManager();
    ~ConfigPackMiniManager() {}

    void setConfigDir(QString config_dir);
    bool isUsing();
    void appendDirList(QString &dir);

    QString getFile(QString file, QString customPath);

private:
    bool        m_is_using;
    QString     m_cp_root_path;
    QString     m_custom_path;
    QStringList m_dir_list;
};

#endif // CONFIGMANAGER_H
