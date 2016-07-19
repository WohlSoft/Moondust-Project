#include "historyelementaddlayer.h"

#include <editing/_scenes/level/lvl_scene.h>
#include <common_features/main_window_ptr.h>
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

    for(int i = 0; i < lvlScene->m_data->layers.size(); i++){
        if(lvlScene->m_data->layers[i].meta.array_id == (unsigned int)m_array_id){
            lvlScene->m_data->layers.removeAt(i);
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

    LevelLayer l;
    l.meta.array_id = m_array_id;
    l.name = m_name;
    l.hidden = false;
    lvlScene->m_data->layers.push_back(l);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(true);
    MainWinConnect::pMainWin->dock_LvlLayers->setLayersBox();
    MainWinConnect::pMainWin->dock_LvlLayers->setLayerToolsLocked(false);
}





