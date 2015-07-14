#include "settings.h"
#include "config_packs.h"
#include <common_features/app_path.h>
#include <QSettings>

void ManagerSettings::load()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("cpack-repos");
    cpacks_reposList.clear();
    int repos=setup.value("repos", 0).toInt();
    while(repos>0)
    {
        ConfigPackRepo repo;
        repo.url=setup.value(QString("repo-%1").arg(repos), "").toString();
        repo.enabled=setup.value(QString("repo-%1-enabled").arg(repos),true).toBool();
        cpacks_reposList.push_back(repo);
        repos--;
    }

    if(cpacks_reposList.isEmpty())
    {   ConfigPackRepo repo;
        repo.url=_DEF_CONFIG_REPO;
        repo.enabled=true;
        cpacks_reposList.push_back(repo);
    }
    setup.endGroup();
}

void ManagerSettings::save()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("cpack-repos");
    setup.setValue("repos", cpacks_reposList.size());
    for(int i=0;i<cpacks_reposList.size();i++)
    {
        setup.setValue(QString("repo-%1").arg(i+1), cpacks_reposList[i].url);
        setup.setValue(QString("repo-%1-enabled").arg(i+1), cpacks_reposList[i].enabled);
    }
    setup.endGroup();
}
