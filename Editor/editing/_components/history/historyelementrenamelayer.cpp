#include "historyelementrenamelayer.h"
#include <editing/_scenes/level/lvl_scene.h>

#include <common_features/main_window_ptr.h>
#include <main_window/dock/lvl_warp_props.h>
#include <main_window/dock/lvl_layers_box.h>
HistoryElementRenameLayer::HistoryElementRenameLayer(const int array_id, const QString oldName, const QString newName, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_oldName(oldName),
    m_newName(newName)
{}

HistoryElementRenameLayer::~HistoryElementRenameLayer()
{}

QString HistoryElementRenameLayer::getHistoryName()
{
    return tr("Rename Layer");
}

void HistoryElementRenameLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(int i = 0; i < lvlScene->m_data->layers.size(); i++){
        if(lvlScene->m_data->layers[i].meta.array_id == (unsigned int)m_array_id){
            lvlScene->m_data->layers[i].name = m_oldName;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->modifyLayer(m_newName, m_oldName);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

void HistoryElementRenameLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(int i = 0; i < lvlScene->m_data->layers.size(); i++)
    {
        if(lvlScene->m_data->layers[i].meta.array_id == (unsigned int)m_array_id)
        {
            lvlScene->m_data->layers[i].name = m_newName;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->modifyLayer(m_oldName, m_newName);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

HistoryElementChangeLayerVisibility::HistoryElementChangeLayerVisibility(const int array_id, QString name, bool visibility, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_name(name),
    m_visibility(visibility)
{}

HistoryElementChangeLayerVisibility::~HistoryElementChangeLayerVisibility()
{}

QString HistoryElementChangeLayerVisibility::getHistoryName()
{
    return tr("Toggle the visibility of a layer");
}

void HistoryElementChangeLayerVisibility::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(auto &l : lvlScene->m_data->layers)
    {
        if(l.meta.array_id == (unsigned int)m_array_id)
        {
            l.hidden = !m_visibility;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->modifyLayer(m_name, m_visibility);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}

void HistoryElementChangeLayerVisibility::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(auto &l : lvlScene->m_data->layers)
    {
        if(l.meta.array_id == (unsigned int)m_array_id)
        {
            l.hidden = m_visibility;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->modifyLayer(m_name, !m_visibility);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}
