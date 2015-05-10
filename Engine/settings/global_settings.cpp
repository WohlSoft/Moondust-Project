#include "global_settings.h"
#include <common_features/app_path.h>
#include <graphics/window.h>

#include <QSettings>

GlobalSettings AppSettings;

GlobalSettings::GlobalSettings()
{
    debugMode       =false;
    interprocessing =false;

    ScreenWidth=800;
    ScreenHeight=600;

    MaxFPS=250;
    PhysStep=65;

    showDebugInfo=false;
}

GlobalSettings::~GlobalSettings()
{

}

void GlobalSettings::load()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
    MaxFPS=setup.value("max-fps", MaxFPS).toUInt();
        if(MaxFPS<65) MaxFPS=65;
    PhysStep=setup.value("phys-step", PhysStep).toUInt();
        if(PhysStep<65) PhysStep=65;
    showDebugInfo=setup.value("show-debug-info", showDebugInfo).toBool();
    setup.endGroup();
}

void GlobalSettings::save()
{
    QSettings setup(AppPathManager::settingsFile(), QSettings::IniFormat);
    setup.beginGroup("Main");
    setup.setValue("max-fps", MaxFPS);
    setup.setValue("phys-step", PhysStep);
    setup.setValue("show-debug-info", showDebugInfo);
    setup.endGroup();
}

void GlobalSettings::resetDefaults()
{

}

void GlobalSettings::apply()
{
    PGE_Window::PhysStep=PhysStep;
    PGE_Window::MaxFPS=MaxFPS;
    PGE_Window::Width =ScreenWidth;
    PGE_Window::Height=ScreenHeight;
    PGE_Window::showDebugInfo=showDebugInfo;
}

