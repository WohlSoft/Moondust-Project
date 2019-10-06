#include "pge_editorpluginmanager.h"
#include <common_features/logger.h>

PGE_EditorPluginManager::PGE_EditorPluginManager(QObject *parent) : QObject(parent)
{}

bool PGE_EditorPluginManager::hasPlugins() const
{
    return m_plugins.size() > 0;
}

unsigned int PGE_EditorPluginManager::getPluginCount() const
{
    return static_cast<uint>(m_plugins.size());
}

const PGE_EditorPluginItem *PGE_EditorPluginManager::getPluginInfo(unsigned int index) const
{
    return m_plugins[static_cast<int>(index)];
}

void PGE_EditorPluginManager::loadPluginsInDir(const QDir &dir)
{
    if(!dir.exists())
    {
        dir.mkdir(".");
        return;
    }

    for(const QString &subFolder : dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
    {
        LogDebug(QString("Found package %1! Looking for main.js...").arg(subFolder));
        QDir nextDir = dir;
        nextDir.cd(subFolder);

        if(nextDir.exists("main.js"))
        {
            LogDebug(QString("Found %1 and attempt to load main.js!").arg(subFolder));
            bool ok = false;
            const QVariantMap result = m_engine.loadFileByExpcetedResult<QVariantMap>(nextDir.canonicalPath() + "/main.js", &ok);

            if(!ok)
            {
                LogWarning("Error while loading main.js in " + subFolder + ", skipping...");
                m_plugins.push_back(new PGE_EditorPluginItem(this, subFolder,
                                    tr("%1 at line %2")
                                    .arg(m_engine.getLastError())
                                    .arg(m_engine.getLastErrorLine())));
                continue;
            }

            if(result.find("pluginName") == result.cend())
            {
                QString errMsg = "Failed to find plugin_name from the result of main.js in " + subFolder + ", skipping...";
                LogWarning(errMsg);
                m_plugins.push_back(new PGE_EditorPluginItem(this, subFolder, std::move(errMsg)));
                continue;
            }

            QString pluginName = result.value("pluginName").toString();
            QString authorName = result.value("authorName").toString();
            QString description = result.value("description").toString();
            int version = result.value("version").toInt();
            m_plugins.push_back(new PGE_EditorPluginItem(this,
                                subFolder,
                                std::move(pluginName),
                                std::move(authorName),
                                std::move(description),
                                version));
            LogDebug("Successfully loaded plugin\"" + pluginName + "\"!");
        }
        else
        {
            LogDebug("Found package, but not main.js!");
        }
    }
}
