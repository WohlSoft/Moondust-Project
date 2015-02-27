#include "historyelementrenamelayer.h"
#include <editing/_scenes/level/lvl_scene.h>

#include <common_features/mainwinconnect.h>
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

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == (unsigned int)m_array_id){
            lvlScene->LvlData->layers[i].name = m_oldName;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->ModifyLayer(m_newName, m_oldName);
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

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == (unsigned int)m_array_id){
            lvlScene->LvlData->layers[i].name = m_newName;
        }
    }

    //just in case
    MainWinConnect::pMainWin->dock_LvlWarpProps->setDoorData(-2);

    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->ModifyLayer(m_oldName, m_newName);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}
