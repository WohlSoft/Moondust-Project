#include "pge_editorpluginmanager.h"
#include <common_features/logger.h>

PGE_EditorPluginManager::PGE_EditorPluginManager(QObject *parent) : QObject(parent)
{}

bool PGE_EditorPluginManager::hasPlugins() const
{
    return m_loadedPlugins.size() > 0;
}

unsigned int PGE_EditorPluginManager::getPluginCount() const
{
    return m_loadedPlugins.size();
}

const PGE_EditorPluginItem &PGE_EditorPluginManager::getPluginInfo(int index) const
{
    return m_loadedPlugins[index];
}

void PGE_EditorPluginManager::loadPluginsInDir(const QDir &dir)
{
    if(!dir.exists())
    {
        dir.mkdir(".");
        return;
    }
    for(QString subFolder : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        WriteToLog(PGE_LogLevel::Debug, QString("Found ") + subFolder + " and attempt to load main.js!");
        QDir nextDir = dir;
        nextDir.cd(subFolder);

        if(!nextDir.exists("main.js")){
            WriteToLog(PGE_LogLevel::Debug, QString("Found ") + subFolder + " and attempt to load main.js!");
            bool ok = false;
            const QVariantMap result = m_engine.loadFileByExpcetedResult<QVariantMap>(nextDir.canonicalPath() + "/main.js", &ok);
            if(!ok)
            {
                WriteToLog(PGE_LogLevel::Warning, "Error while loading main.js in " + subFolder + ", skipping...");
                continue;
            }

            if(result.find("plugin_name") == result.cend()){
                WriteToLog(PGE_LogLevel::Warning, "Failed to find plugin_name from the result of main.js in " + subFolder + ", skipping...");
                continue;
            }

            QString pluginName = result.value("pluginName").toString();
            QString authorName = result.value("authorName").toString();
            int version = result.value("version").toInt();
            m_loadedPlugins.push_back(PGE_EditorPluginItem(std::move(pluginName), std::move(authorName), version));

            WriteToLog(PGE_LogLevel::Debug, "Successfully loaded plugin\"" + pluginName + "\"!");
        }
    }
}
