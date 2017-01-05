#pragma once
#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <vector>
#include <set>

extern std::string g_ApplicationPath;

class ConfigPackMiniManager
{
public:
    typedef std::vector<std::string> StringList;
    ConfigPackMiniManager();
    ~ConfigPackMiniManager() {}

    void setConfigDir(const std::string &config_dir);
    bool isUsing();
    void addIntoDirList(std::string dir);
    void appendDirList(const std::string &dir);

    std::string getFile(const std::string &file, std::string customPath);

private:
    bool                    m_is_using;
    std::string             m_cp_root_path;
    std::string             m_custom_path;
    std::set<std::string>   m_dir_list;
};

#endif // CONFIGMANAGER_H
