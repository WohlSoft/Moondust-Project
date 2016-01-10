#include "config_manager.h"

#include <QCoreApplication>
#include <QSettings>
#include <QDir>

ConfigPackMiniManager::ConfigPackMiniManager() : m_is_using(false)
{}

void ConfigPackMiniManager::setConfigDir(QString config_dir)
{
    if(config_dir.isEmpty()) return;

    if(!QDir(config_dir).exists())
        return;
    if(!QFileInfo(config_dir+"/main.ini").exists())
        return;

    m_cp_root_path = QDir(config_dir).absolutePath()+"/";

    QString main_ini = m_cp_root_path + "main.ini";
    QSettings mainset(main_ini, QSettings::IniFormat);
    mainset.setIniCodec("UTF-8");

    QString customAppPath = QCoreApplication::applicationDirPath()+"/";

    m_dir_list.clear();
    m_dir_list.push_back(m_cp_root_path);

    mainset.beginGroup("main");
    customAppPath = mainset.value("application-path", customAppPath).toString();
    customAppPath.replace('\\', '/');
    m_cp_root_path = (mainset.value("application-dir", false).toBool() ?
                          customAppPath + "/" : m_cp_root_path + "/data/" );
    m_dir_list.push_back( m_cp_root_path + mainset.value("graphics-level", "data/graphics/level").toString() + "/");
    appendDirList(m_dir_list.last());
    m_dir_list.push_back( m_cp_root_path + mainset.value("graphics-worldmap", "data/graphics/worldmap").toString() + "/");
    appendDirList(m_dir_list.last());
    m_dir_list.push_back( m_cp_root_path + mainset.value("graphics-characters", "data/graphics/characters").toString() + "/");
    appendDirList(m_dir_list.last());
    m_dir_list.push_back( m_cp_root_path + mainset.value("custom-data", "data-custom").toString() + "/");
    appendDirList(m_dir_list.last());
    mainset.endGroup();

    for(int i=0; i< m_dir_list.size(); i++)
    {
        m_dir_list[i].replace('\\', '/');
        m_dir_list[i].remove("//");
        if( !m_dir_list[i].endsWith('/') && !m_dir_list[i].endsWith('\\') )
            m_dir_list[i].push_back('/');
    }

    m_is_using=true;
}

bool ConfigPackMiniManager::isUsing()
{
    return m_is_using;
}

void ConfigPackMiniManager::appendDirList(QString &dir)
{
    QDir dirs(dir);
    QStringList folders = dirs.entryList(QDir::NoDotAndDotDot|QDir::Dirs);
    foreach(QString f, folders)
    {
        QString newpath=QString(dirs.absolutePath()+"/"+f).remove("//");
        if(!m_dir_list.contains(newpath)) //Disallow duplicate entries
            m_dir_list.push_back(newpath);
    }
}

QString ConfigPackMiniManager::getFile(QString file, QString customPath)
{
    if( !customPath.endsWith('/') && !customPath.endsWith('\\') )
        customPath.push_back('/');

    if(!m_is_using) return customPath+file;

    if(QFileInfo(customPath+file).exists())
        return customPath+file;

    foreach(QString path, m_dir_list)
    {
        if(QFileInfo(path+file).exists())
            return path+file;
    }
    return customPath+file;
}
