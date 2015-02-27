#include "historyelementaddlayer.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <common_features/mainwinconnect.h>
#include <main_window/dock/lvl_layers_box.h>

HistoryElementAddLayer::HistoryElementAddLayer(int array_id, QString name, QObject *parent) :
    QObject(parent),
    m_array_id(array_id),
    m_name(name)
{}

HistoryElementAddLayer::~HistoryElementAddLayer()
{}

QString HistoryElementAddLayer::getHistoryName()
{
    return tr("Add Layer");
}

void HistoryElementAddLayer::undo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    for(int i = 0; i < lvlScene->LvlData->layers.size(); i++){
        if(lvlScene->LvlData->layers[i].array_id == (unsigned int)m_array_id){
            lvlScene->LvlData->layers.removeAt(i);
        }
    }
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);

}

void HistoryElementAddLayer::redo()
{
    if(!m_scene)
        return;

    LvlScene* lvlScene;
    if(!(lvlScene = qobject_cast<LvlScene*>(m_scene)))
        return;

    LevelLayers l;
    l.array_id = m_array_id;
    l.name = m_name;
    l.hidden = false;
    lvlScene->LvlData->layers.push_back(l);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}





