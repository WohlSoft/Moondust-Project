#include "items.h"


QPixmap Items::getItemGFX(int itemType, unsigned long ItemID, bool whole, long  *confId)
{
    MainWindow *main = MainWinConnect::pMainWin;

    switch(itemType)
    {
    case ItemTypes::LVL_Block:
        {
            if(main->activeChildWindow()==1)
            {
                LvlScene *scn = main->activeLvlEditWin()->scene;
                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_blocks.size())
                {
                    j = scn->index_blocks[ItemID].i;
                    animator = scn->index_blocks[ItemID].ai;
                }

                if( confId != NULL)
                    confId = (long*)j;

                if(whole)
                    return scn->animates_Blocks[animator]->wholeImage();
                else
                    return scn->animates_Blocks[animator]->image(0);
            }
            else
            {
                QPixmap tmpI;
                long j=0;
                bool isIndex=false;
                //Check for index
                if(ItemID < (unsigned long)main->configs.index_blocks.size())
                {
                    if(ItemID == main->configs.main_block[main->configs.index_blocks[ItemID].i].id)
                    {
                        j = main->configs.index_blocks[ItemID].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < main->configs.main_block.size(); i++)
                    {
                        if(main->configs.main_block[i].id == ItemID)
                        {
                            j = i;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }

                if( confId != NULL)
                    confId = (long*)j;

                if((main->configs.main_block[j].animated)&&(!whole))
                    tmpI = main->configs.main_block[j].image.copy(0,
                                (int)round(main->configs.main_block[j].image.height() / main->configs.main_block[j].frames)*main->configs.main_block[j].display_frame,
                                main->configs.main_block[j].image.width(),
                                (int)round(main->configs.main_block[j].image.height() / main->configs.main_block[j].frames));
                else
                    tmpI = main->configs.main_block[j].image;

                return tmpI;

            }

            break;
        }
    case ItemTypes::LVL_BGO:
        {
            if(main->activeChildWindow()==1)
            {
                LvlScene *scn = main->activeLvlEditWin()->scene;
                long j=0;
                long animator=0;
                if(ItemID < (unsigned int)scn->index_bgo.size())
                {
                    j = scn->index_bgo[ItemID].i;
                    animator = scn->index_bgo[ItemID].ai;
                }
                if( confId != NULL)
                    confId = (long*)j;

                if(whole)
                    return scn->animates_BGO[animator]->wholeImage();
                else
                    return scn->animates_BGO[animator]->image(0);
            }
            else
            {
                QPixmap tmpI;
                long j=0;
                bool isIndex=false;
                //Check for index
                if(ItemID < (unsigned long)main->configs.index_bgo.size())
                {
                    if(ItemID == main->configs.main_bgo[main->configs.index_bgo[ItemID].i].id)
                    {
                        j = main->configs.index_bgo[ItemID].i;
                        isIndex=true;
                    }
                }
                //In index is false, fetch array
                if(!isIndex)
                {
                    for(int i=0; i < main->configs.main_bgo.size(); i++)
                    {
                        if(main->configs.main_bgo[i].id == ItemID)
                        {
                            j = i;
                            isIndex=true;
                            break;
                        }
                    }
                    if(!isIndex) j=0;
                }

                if( confId != NULL)
                    confId = (long*)j;

                if((main->configs.main_bgo[j].animated)&&(!whole))
                    tmpI = main->configs.main_bgo[j].image.copy(0,
                                (int)round(main->configs.main_bgo[j].image.height() / main->configs.main_bgo[j].frames)*main->configs.main_bgo[j].display_frame,
                                main->configs.main_bgo[j].image.width(),
                                (int)round(main->configs.main_bgo[j].image.height() / main->configs.main_bgo[j].frames));
                else
                    tmpI = main->configs.main_bgo[j].image;

                return tmpI;
            }
            break;
        }
    case ItemTypes::LVL_NPC:
        {
            if(main->activeChildWindow()==1)
            {
                LvlScene *scn = main->activeLvlEditWin()->scene;
                if(!whole)
                    return scn->getNPCimg(ItemID, -1);
                else
                {
                    int j;
                    bool noimage=true, found=false;
                    QPixmap tImg;
                    //Check Index exists
                    if(ItemID < (unsigned int)scn->index_npc.size())
                    {
                        j = scn->index_npc[ItemID].gi;

                        if(j<main->configs.main_npc.size())
                        {
                        if(main->configs.main_npc[j].id == ItemID)
                            found=true;
                        }
                    }

                    if(found)
                    {   //get neccesary element directly
                        if(scn->index_npc[ItemID].type==1)
                        {
                            if(scn->uNPCs[scn->index_npc[ItemID].i].withImg)
                            {
                                noimage=false;
                                tImg = scn->uNPCs[scn->index_npc[ItemID].i].image;
                            }
                        }

                        if(!noimage)
                        {
                            tImg = main->configs.main_npc[ scn->index_npc[ItemID].gi].image;
                            noimage=false;
                        }
                    }

                    if( confId != NULL)
                        confId = (long*)j;

                    if((noimage)||(tImg.isNull()))
                    {
                        tImg=QPixmap(QApplication::applicationDirPath() + "/" + "data/unknown_npc.png");
                    }

                    return tImg;

                }
            }
            else
            {

            }
            break;
        }
    default:
        break;
    }
    //return null if wrong item
    return QPixmap();
}
