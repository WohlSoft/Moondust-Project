/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * WARNING!
 * This is a piece of code for change layer of selected items by context menu.
 *
 * This piece made only for this files:
 *    item_bgo.cpp
 *    item_block.cpp
 *    item_npc.cpp
 *    item_water.cpp
 *    item_door.cpp
 *
 * This file can't be used for others without necessary predefined values.
 */

bool itemIsFound=false;
QString lName;
if(selected==newLayer)
{
    scene->contextMenuOpened = false;
    ToNewLayerBox * layerBox = new ToNewLayerBox(scene->LvlData);
    layerBox->setWindowFlags (Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
    layerBox->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, layerBox->size(), qApp->desktop()->availableGeometry()));
    if(layerBox->exec()==QDialog::Accepted)
    {
        itemIsFound=true;
        lName = layerBox->lName;

        //Store new layer into array
        LevelLayers nLayer;
        nLayer.name = lName;
        nLayer.hidden = layerBox->lHidden;
        scene->LvlData->layers_array_id++;
        nLayer.array_id = scene->LvlData->layers_array_id;
        scene->LvlData->layers.push_back(nLayer);
        //scene->SyncLayerList=true; //Refresh layer list
        MainWinConnect::pMainWin->setLayerToolsLocked(true);
        MainWinConnect::pMainWin->setLayersBox();
        MainWinConnect::pMainWin->setLayerToolsLocked(false);
    }
    delete layerBox;
}
else
foreach(QAction * lItem, layerItems)
{
    if(selected==lItem)
    {
        itemIsFound=true;
        lName = lItem->data().toString();
        //FOUND!!!
     break;
    }//Find selected layer's item
}

if(itemIsFound)
{
    LevelData modData;
    foreach(LevelLayers lr, scene->LvlData->layers)
    { //Find layer's settings
        if(lr.name==lName)
        {
            foreach(QGraphicsItem * SelItem, scene->selectedItems() )
            {
                if(SelItem->data(0).toString()=="Block")
                {
                    modData.blocks.push_back(((ItemBlock*) SelItem)->blockData);
                    ((ItemBlock *) SelItem)->blockData.layer = lr.name;
                    ((ItemBlock *) SelItem)->setVisible(!lr.hidden);
                    ((ItemBlock *) SelItem)->arrayApply();
                }
                else
                if(SelItem->data(0).toString()=="BGO")
                {
                    modData.bgo.push_back(((ItemBGO*) SelItem)->bgoData);
                    ((ItemBGO *) SelItem)->bgoData.layer = lr.name;
                    ((ItemBGO *) SelItem)->setVisible(!lr.hidden);
                    ((ItemBGO *) SelItem)->arrayApply();
                }
                else
                if(SelItem->data(0).toString()=="NPC")
                {
                    modData.npc.push_back(((ItemNPC*) SelItem)->npcData);
                    ((ItemNPC *) SelItem)->npcData.layer = lr.name;
                    ((ItemNPC *) SelItem)->setVisible(!lr.hidden);
                    ((ItemNPC *) SelItem)->arrayApply();
                }
                else
                if(SelItem->data(0).toString()=="Water")
                {
                    modData.water.push_back(((ItemWater*) SelItem)->waterData);
                    ((ItemWater *) SelItem)->waterData.layer = lr.name;
                    ((ItemWater *) SelItem)->setVisible(!lr.hidden);
                    ((ItemWater *) SelItem)->arrayApply();
                }
                else
                if((SelItem->data(0).toString()=="Door_exit")  ||
                        (SelItem->data(0).toString()=="Door_enter"))
                {
                    if(SelItem->data(0).toString()=="Door_exit"){
                        LevelDoors tDoor = ((ItemDoor *) SelItem)->doorData;
                        tDoor.isSetOut = true;
                        tDoor.isSetIn = false;
                        modData.doors.push_back(tDoor);
                    }
                    else
                    if(SelItem->data(0).toString()=="Door_enter"){
                        LevelDoors tDoor = ((ItemDoor *) SelItem)->doorData;
                        tDoor.isSetOut = false;
                        tDoor.isSetIn = true;
                        modData.doors.push_back(tDoor);
                    }
                    ((ItemDoor *) SelItem)->doorData.layer = lr.name;
                    ((ItemDoor *) SelItem)->setVisible(!lr.hidden);
                    ((ItemDoor *) SelItem)->arrayApply();
                }
            }
            if(selected==newLayer){
                scene->addChangedNewLayerHistory(modData, lr);
            }
            break;
        }
    }//Find layer's settings
    if(selected!=newLayer){
        scene->addChangedLayerHistory(modData, lName);
    }
    scene->contextMenuOpened = false;
}
