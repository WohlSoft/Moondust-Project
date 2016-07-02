#include <mainwindow.h>

#include <ui_mainwindow.h>
#include <common_features/logger.h>
#include <common_features/app_path.h>

void MainWindow::initPlugins()
{
    WriteToLog(PGE_LogLevel::Debug, "Init. Plugins");

    QMenu* pluginsMenu = ui->menuPlugins;
    pluginsMenu->clear();

    QString mainDir = AppPathManager::userAppDir();
    m_pluginManager.loadPluginsInDir(QDir(mainDir + "/editor-plugins"));


    // Add plugins to menu

    if(pluginsMenu->actions().size() == 0)
        pluginsMenu->addAction("<No Plugins installed>");

    WriteToLog(PGE_LogLevel::Debug, "Init. Plugin Menu Finished");
}


