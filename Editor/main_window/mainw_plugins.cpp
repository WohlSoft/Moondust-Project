#include <mainwindow.h>

#include <ui_mainwindow.h>
#include <common_features/logger.h>
#include <common_features/app_path.h>

#include "plugins/pge_editorplugininfo.h"

void MainWindow::initPlugins()
{
    WriteToLog(PGE_LogLevel::Debug, "Init. Plugins");

    QMenu* pluginsMenu = ui->menuPlugins;
    pluginsMenu->clear();

    QString mainDir = AppPathManager::userAppDir();
    m_pluginManager.loadPluginsInDir(QDir(mainDir + "/editor-plugins"));
    m_pluginManager.loadPluginsInDir(QDir(configs.config_dir + "editor-plugins"));

    // Add plugins to menu
    for(unsigned int i = 0; i < m_pluginManager.getPluginCount(); ++i) {
        const PGE_EditorPluginItem* item = m_pluginManager.getPluginInfo(i);
        if(item->isValid())
            pluginsMenu->addAction(item->pluginName());
    }

    if(pluginsMenu->actions().size() == 0)
        pluginsMenu->addAction("<No Plugins installed>");

    pluginsMenu->addSeparator();
    QAction* pluginManager = pluginsMenu->addAction("Plugin Manager");
    connect(pluginManager, &QAction::triggered, [this](bool checked) {
        Q_UNUSED(checked);
        PGE_EditorPluginInfo infoWindow(&m_pluginManager, this);
        infoWindow.exec();
    });

    WriteToLog(PGE_LogLevel::Debug, "Init. Plugin Menu Finished");
}


