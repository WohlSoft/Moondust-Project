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

#include <common_features/app_path.h>
#include <editing/edit_level/level_edit.h>
#include <file_formats/file_formats.h>

#include "lvl_scene.h"

// /////////////////////////Init unused Section space as empty section///////////////////////////////
void LvlScene::InitSection(int sect)
{
    long leftA, leftB;
    long rightA, rightB;
    long topA, topB;
    long bottomA, bottomB;
    int i=0;
    bool collided=true;

    if( (sect >= LvlData->sections.size()) && (sect<0) )
    {
        //Expand sections
        int needToAdd = (LvlData->sections.size()-1) - sect;
        while(needToAdd > 0)
        {
            LevelSection dummySct = FileFormats::dummyLvlSection();
            dummySct.id = LvlData->sections.size();
            LvlData->sections.push_back(dummySct);
            needToAdd--;
        }
    }

    long x,y,h,w;

    if(
        (LvlData->sections[sect].size_left==0) &&
        (LvlData->sections[sect].size_top==0) &&
        (LvlData->sections[sect].size_bottom==0) &&
        (LvlData->sections[sect].size_right==0)
      )
    {
       //set section size/position values
       x = -200000 + 20000*sect;//left
       y = -200600 + 20000*sect;//top

       h = -200000 + 20000*sect;//bottom

       w = -199200 + 20000*sect;//right


       bool found=false;

       #ifdef _DEBUG_
       WriteToLog(QtDebugMsg, QString("InitSection -> Checking for collisions with other sections"));
       #endif

       //Looking for optimal section position
       while(collided)
       {
           leftA = x;
           topA = y;
           rightA = w;
           bottomA = h;

           found=false;

           //check crossing with other sections
           for(i=0;i<LvlData->sections.size(); i++)
           {
               if(i == sect) continue;

               leftB = LvlData->sections[i].size_left;
               rightB = LvlData->sections[i].size_right;
               topB = LvlData->sections[i].size_top;
               bottomB = LvlData->sections[i].size_bottom;

               if( bottomA <= topB )
               { continue; }
               if( topA >= bottomB )
               { continue; }
               if( rightA <= leftB )
               { continue; }
               if( leftA >= rightB )
               { continue; }

               #ifdef _DEBUG_
               WriteToLog(QtDebugMsg, QString("InitSection -> Collision found!"));
               #endif
               found=true;
           }

        if(found) //If collision found, move section right and try check agan
        {
            x += 20000; // move section right
            w += 20000; // move section right
        }
            else
        { //end loop
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("InitSection -> Collision pass"));
            #endif
            collided=false;
        }

       }

       //Apply section's data values
       LvlData->sections[sect].size_left = x;
       LvlData->sections[sect].size_top = y;
       LvlData->sections[sect].size_right = w;
       LvlData->sections[sect].size_bottom = h;

       //set position to initialized section
       LvlData->sections[sect].PositionY = y-10;
       LvlData->sections[sect].PositionX = x-10;

       LvlData->sections[sect].bgcolor = 16291944;

       #ifdef _DEBUG_
       WriteToLog(QtDebugMsg, QString("InitSection -> Data was applyed X%1 Y%2 W%3 H%4")
                  .arg(x).arg(y).arg(w).arg(h));
       #endif
    }
    setSectionBG( LvlData->sections[sect] );
}




void LvlScene::ChangeSectionBG(int BG_Id, int SectionID, bool forceTiled)
{
    int sctID=0;
    if(SectionID<0)
        sctID = LvlData->CurSection;
    else
        sctID = SectionID;

    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(0).toString()== QString("BackGround%1").arg(LvlData->sections[sctID].id) )
        {
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("Remove items "+findBG->data(0).toString()+" by id="+QString::number(sctID)+" by SctID="+QString::number(LvlData->sections[sctID].id)) );
            #endif
            removeItem(findBG);
            if(findBG) delete findBG;
        }
    }

    if((BG_Id>=0) && (BG_Id <= pConfigs->main_bg.size() )) // Deny unexist ID
            LvlData->sections[sctID].background = BG_Id;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    #endif
    setSectionBG(LvlData->sections[sctID], forceTiled);
}




// ////////////////////////Apply section background/////////////////////////////
void LvlScene::setSectionBG(LevelSection section, bool forceTiled)
{
    //QGraphicsPixmapItem * item=NULL;
    QGraphicsRectItem * itemRect=NULL;
    QBrush brush;
    QPen pen;
    QPixmap image = QPixmap(ApplicationPath + "/" + "data/nobg.gif");
    QPixmap img;
    QPixmap img2; //Second image buffer
    //need a BGitem

    bool isUser1=false, isUser2=false, noimage=false;
    long x,y,h,w, j;

    if(
        (section.size_left!=0) ||
        (section.size_top!=0)||
        (section.size_bottom!=0)||
        (section.size_right!=0)
      )
    { //Don't draw on unallocated section entry
        x=section.size_left;
        y=section.size_top;
        w=section.size_right;
        h=section.size_bottom;

        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "SetSectionBG-> Check for user images");
        #endif

        isUser1=false; // user's images are exist
        isUser2=false; // user's images are exist
        noimage=true;
        j = 0;
        if(section.background != 0 )
        {
            //Find user image
            for(j=0;j<uBGs.size();j++)
            {
                if(uBGs[j].id==section.background)
                {
                    noimage=false;
                    if((uBGs[j].q==0)||(uBGs[j].q==2)) //set first image
                        {img = uBGs[j].image; isUser1=true;}
                    if((uBGs[j].q>=1)){ // set Second image
                        img2 = uBGs[j].second_image; isUser2=true;}
                    break;
                }
            } //If not exist, will be used default

            j=pConfigs->getBgI(section.background);
            if(j>=0)
            {
                noimage=false;
                  if(!isUser1)
                      img = pConfigs->main_bg[j].image;
                  if(!isUser2)
                      img2 = pConfigs->main_bg[j].second_image;
            }

            if((noimage)&&(!isUser1))
            {
                #ifdef _DEBUG_
                WriteToLog(QtWarningMsg, "SetSectionBG-> Image not found");
                #endif
                img=image;
            }
        }
        else noimage=true;

        //pConfigs->main_bg[j].type;

        brush = QBrush(QColor(0, 0, 0));
        //QBrush brush(QColor(255, 255, 255));
        pen = QPen(Qt::NoPen);
        //for (int i = 0; i < 11; i++) {

        //item = addRect(QRectF(x, y, , ), pen, brush);

        if((!noimage)&&(!img.isNull()))
        {
            //item = addPixmap(image);
            //item = new QGraphicsPixmapItem;
            DrawBG(x, y, w, h, section.id, img, img2, pConfigs->main_bg[j], forceTiled);
            //BgItem[section.id]->setParentItem(item);
            //addItem(item);
            //item->setData(0, "BackGround"+QString::number(section.id) );
            //item->setPos(x, y);
        }
        else
        {
            itemRect = new QGraphicsRectItem;
            itemRect->setPen(pen);
            itemRect->setBrush(brush);
            itemRect->setRect(x, y, (long)fabs(x-w), (long)fabs(y-h));
            addItem(itemRect);
        }

        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("SetSectionBG-> Item placed to x=%1 y=%2 h=%3 w=%4").arg(x).arg(y)
                   .arg((long)fabs(x-w)).arg((long)fabs(y-h)));
        #endif
        //

        if(itemRect!=NULL)
        {
            itemRect->setData(0, QString("BackGround%1").arg(section.id) );
            itemRect->setZValue(Z_backImage);
        }

    } //Don't draw on reserved section entry

}


bool LvlScene::isInSection(long x, long y, int sectionIndex, int padding)
{
    return (x >= LvlData->sections[sectionIndex].size_left-padding) &&
        (x <= LvlData->sections[sectionIndex].size_right+padding) &&
        (y >= LvlData->sections[sectionIndex].size_top-padding) &&
             (y <= LvlData->sections[sectionIndex].size_bottom+padding);
}


bool LvlScene::isInSection(long x, long y, int width, int height, int sectionIndex, int padding)
{
    return isInSection(x, y, sectionIndex, padding) && isInSection(x+width, y+height, sectionIndex, padding);
}


// ////////////////////////////////////Draw BG image/////////////////////////////////////////////////
void LvlScene::DrawBG(int x, int y, int w, int h, int sctID,
                      QPixmap &srcimg, QPixmap &srcimg2, obj_BG &bgsetup, bool forceTiled)
{
    int si_attach, attach;
    QGraphicsItem * item;
    QColor FillColor; //Fill undrawed space with color

    long sctW,//Section Width
         sctH,//Section Height
         R1W, //Img Width  (Row1)
         R1H, //Img Height (Row1)
         R1Hc=0, //Crop height from bottom
         R1Ho=0, //Offset from top
         R2W, //Img Width  (Row2)
         R2H, //Img Height (Row2)
         R2Hc=0, //Crop height from bottom
         R2Ho=0, //Offset from top
         RectPlus=0,
         toY; //Placing position Y 0 - top

    sctW = (long)fabs(x-w);
    sctH = (long)fabs(y-h);

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "Draw BG -> Draw BG Image");
    #endif

    attach = bgsetup.attached;

// ///////////////////SingleRow BG///////////////////////////
    if((bgsetup.type==0)&&(!bgsetup.editing_tiled)&&(!forceTiled))
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Draw BG -> Style: SingleRow BG");
        #endif

        R1W = srcimg.width();
        R1H = srcimg.height();

        if(attach==0) // Get Pixel color (0 - bottom, 1 - top)
            FillColor = QColor( srcimg.toImage().pixel(0,0) ); // from top
        else
            FillColor = QColor( srcimg.toImage().pixel(0,(R1H-1)) ); //from bottom

        if(attach==0)
        { // 0 - bottom
            toY = (sctH>R1H)? sctH-R1H : 0;
            R1Hc = ((R1H>sctH) ? R1H-sctH : 0); //Crop height from bottom
            R1Ho = R1Hc; //Offset from top
            RectPlus=0;
        }
        else
        { // 1 - top
            toY = 0;
            R1Hc = ((R1H>sctH) ? R1H-sctH : 0); //Crop height from bottom
            R1Ho = 0; //Offset from top
            RectPlus=R1H;
        }

        // /////////////////////Draw row//////////////////

        item = addRect(0, 0, sctW, R1H-R1Hc, Qt::NoPen, QBrush(srcimg.copy(0, R1Ho, R1W, R1H-R1Hc)));
        item->setData(0, QString("BackGround%1").arg(sctID) );
        item->setPos(x, y+toY);
        item->setZValue(Z_backImage);

        if(R1H < sctH)
        {
            item = addRect(0, 0, sctW, sctH-R1H, Qt::NoPen, QBrush(FillColor));
            item->setData(0, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y+RectPlus);
            item->setZValue(Z_backImage);
        }
    }
    else

// ///////////////////DoubleRow BG////////////////////////
    if((bgsetup.type==1)&&(!bgsetup.editing_tiled)&&(!forceTiled))
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Draw BG -> Style: DoubleRow BG");
        #endif

        si_attach = bgsetup.second_attached; // Second image attach

        R1W = srcimg.width();
        R1H = srcimg.height();

        //Fill empty space
        if((!srcimg2.isNull()) && (si_attach==0))
            FillColor = QColor( srcimg2.toImage().pixel(0,0) );
        else
            FillColor = QColor( srcimg.toImage().pixel(0,0) );

        toY = (sctH>R1H)? sctH-R1H : 0;
        R1Hc = ((R1H>sctH) ? R1H-sctH : 0); //Crop height from bottom
        R1Ho = R1Hc; //Offset from top
        RectPlus=0;

        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("Draw BG -> Draw first row, params: "));
        #endif

        // /////////////////////Draw first row//////////////////
        item = addRect(0, 0, sctW, R1H-R1Hc, Qt::NoPen, QBrush(srcimg.copy(0, R1Ho, R1W, R1H-R1Hc)));
        item->setData(0, QString("BackGround%1").arg(sctID) );
        item->setPos(x, y+toY);
        item->setZValue(Z_backImage);
        // /////////////////////Draw first row//////////////////

        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Draw BG -> Draw second row");
        #endif

        R2W = srcimg2.width();
        R2H = srcimg2.height();

        if(si_attach==0) // over first
        {
            toY = (sctH-R1H > R2H)? sctH-R2H-R1H : 0;
            R2Hc = ((R2H+R1H>sctH) ? R2H-(sctH-R1H) : 0); //Crop height from bottom
            R2Ho = R2Hc; //Offset from top
            RectPlus=R2H;
        }
        else
        if(si_attach==1) // bottom
        {
            toY = (sctH > R2H)? sctH-R2H : 0;
            R2Hc = ((R2H>sctH) ? R2H-sctH : 0); //Crop height from bottom
            R2Ho = R2Hc; //Offset from top
            RectPlus=0;
        }

        if((!srcimg2.isNull()) && ((sctH > R1H)||(si_attach==1)))
        {

            // /////////////////////Draw second row//////////////////
            item = addRect(0, 0, sctW, R2H-R2Hc, Qt::NoPen, QBrush( srcimg2.copy(0, R2Ho, R2W, R2H-R2Hc) ));
            item->setData(0, QString("BackGround%1").arg(sctID) );
            item->setPos(x, y+toY);
            item->setZValue(Z_backImage+0.0000000001);
            // /////////////////////Draw second row//////////////////

        } else
            if(srcimg2.isNull())
                WriteToLog(QtWarningMsg, "Draw BG -> second image is Null");


        if( R1H+RectPlus < sctH )
        {
            item = addRect(0, 0, sctW, sctH-R1H-RectPlus, Qt::NoPen, QBrush(FillColor));
            item->setData(0, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y);
            item->setZValue(Z_backImage);
        }

    }
    else

// ///////////////////////////////Tiled BG///////////////////////////////
    {

     #ifdef _DEBUG_
     WriteToLog(QtDebugMsg, "Draw BG -> Style: Tiled");
     #endif

     R1W = srcimg.width();
     R1H = srcimg.height();
        if(attach==0)
        {
            //Attached to Bottom
            RectPlus = sctH % R1H;

            toY = (sctH>R1H)? sctH-R1H : 0;

            //R1Hc = R1H-RectPlus; // Crop height from bottom/Offset from top

            item = addRect(0, 0, sctW, RectPlus, Qt::NoPen,
                           QBrush(srcimg.copy(0, R1H-RectPlus, R1W, RectPlus))
                           );
            item->setData(0, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y);
            item->setZValue(Z_backImage);

            if(sctH >= R1H)
            {
                item = addRect(0, 0, sctW, sctH-RectPlus, Qt::NoPen, QBrush(srcimg));
                item->setData(0, "BackGround"+QString::number(sctID) );
                item->setPos(x,y+RectPlus);
                item->setZValue(Z_backImage);
            }

        }
        else
        {
            //Attached to Top
            item = addRect(0, 0, sctW, sctH, Qt::NoPen, QBrush(srcimg));
            item->setData(0, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y);
            item->setZValue(Z_backImage);
        }
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "acceptedID is "+QString::number(sctID)+" data is "+item->data(0).toString());
    WriteToLog(QtDebugMsg, "Draw BG -> Drawed");
    #endif
}





//  ////////////////////////////Drawing the inactive intersection space////////////////////////////////////
void LvlScene::drawSpace()
{
    const long padding = 400000;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find and remove current"));
    #endif
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="Space")
        {
            removeItem(spaceItem); delete spaceItem;
            continue;
        }
        if(spaceItem->data(0).toString()=="SectionBorder")
        {
            removeItem(spaceItem); delete spaceItem;
            continue;
        }
    }

    QPolygon bigSpace;
    QGraphicsItem * item;
    QGraphicsItem * item2;
    QVector<QPoint > drawing;

    int i, j;
    long l, r, t, b;
         //x, y, h, w;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find minimal"));
    #endif
    j=0;
    do
    {
        l = LvlData->sections[j].size_left;
        r = LvlData->sections[j].size_right;
        t = LvlData->sections[j].size_top;
        b = LvlData->sections[j].size_bottom;
        j++;
    }
    while(
          ((LvlData->sections[j-1].size_left==0) &&
          (LvlData->sections[j-1].size_right==0) &&
          (LvlData->sections[j-1].size_top==0) &&
          (LvlData->sections[j-1].size_bottom==0)) && (j<LvlData->sections.size())
    );

    for(i=0;i<LvlData->sections.size(); i++)
    {

        if(
                (LvlData->sections[i].size_left==0) &&
                (LvlData->sections[i].size_right==0) &&
                (LvlData->sections[i].size_top==0) &&
                (LvlData->sections[i].size_bottom==0))
            continue;

        if(LvlData->sections[i].size_left < l)
            l = LvlData->sections[i].size_left;
        if(LvlData->sections[i].size_right > r)
            r = LvlData->sections[i].size_right;
        if(LvlData->sections[i].size_top < t)
            t = LvlData->sections[i].size_top;
        if(LvlData->sections[i].size_bottom > b)
            b = LvlData->sections[i].size_bottom;
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Draw polygon"));
    #endif

    drawing.clear();
    drawing.push_back(QPoint(l-padding, t-padding));
    drawing.push_back(QPoint(r+padding, t-padding));
    drawing.push_back(QPoint(r+padding, b+padding));
    drawing.push_back(QPoint(l-padding, b+padding));
    drawing.push_back(QPoint(l-padding, t+padding));

    bigSpace = QPolygon(drawing);


    l = LvlData->sections[LvlData->CurSection].size_left;
    r = LvlData->sections[LvlData->CurSection].size_right;
    t = LvlData->sections[LvlData->CurSection].size_top;
    b = LvlData->sections[LvlData->CurSection].size_bottom;


    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Draw editing hole"));
    #endif

    drawing.clear();
    drawing.push_back(QPoint(l-1, t-1));
    drawing.push_back(QPoint(r+1, t-1));
    drawing.push_back(QPoint(r+1, b+1));
    drawing.push_back(QPoint(l-1, b+1));
    drawing.push_back(QPoint(l-1, t-1));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, QString("Draw intersection space-> add polygon to Item"));
    #endif
    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 2));
    item->setZValue(Z_sys_interspace1);
    item2->setZValue(Z_sys_sctBorder);
    item->setOpacity(qreal(0.4));
    item->setData(0, "Space");
    item2->setData(0, "SectionBorder");
}


