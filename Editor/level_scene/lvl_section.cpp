/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#include "lvlscene.h"
#include "../edit_level/leveledit.h"


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
        return; //Protector
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

       WriteToLog(QtDebugMsg, QString("InitSection -> Checking for collisions with other sections"));

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

               WriteToLog(QtDebugMsg, QString("InitSection -> Collision found!"));
               found=true;
           }

        if(found) //If collision found, move section right and try check agan
        {
            x += 20000; // move section right
            w += 20000; // move section right
        }
            else
        { //end loop
            WriteToLog(QtDebugMsg, QString("InitSection -> Collision pass"));
            collided=false;
        }

       }

       //Apply section's data values
       LvlData->sections[sect].size_left = x;
       LvlData->sections[sect].size_top = y;
       LvlData->sections[sect].size_right = w;
       LvlData->sections[sect].size_bottom = h;

       //set position to initialized section
       LvlData->sections[sect].PositionY = y;
       LvlData->sections[sect].PositionX = x;

       LvlData->sections[sect].bgcolor = 16291944;

       WriteToLog(QtDebugMsg, QString("InitSection -> Data was applyed X%1 Y%2 W%3 H%4")
                  .arg(x).arg(y).arg(w).arg(h));
    }
    setSectionBG( LvlData->sections[sect] );
}


/*
void LvlScene::ChangeSectionBG(int BG_Id) //set Backgrouns for currenctSection
{
    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(0)=="BackGround"+QString::number(LvlData->CurSection) )
        {
            WriteToLog(QtDebugMsg, QString("Remove item BackGround"+QString::number(LvlData->CurSection)) );
            removeItem(findBG); break;
        }
    }

    if((BG_Id>=0) && (BG_Id <= pConfigs->main_bg.size() )) // Deny unexist ID
            LvlData->sections[LvlData->CurSection].background = BG_Id;

    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    setSectionBG(LvlData->sections[LvlData->CurSection]);
}
*/

void LvlScene::ChangeSectionBG(int BG_Id, int SectionID)
{
    if(SectionID<0)
        SectionID = LvlData->CurSection;

    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(0)=="BackGround"+QString::number(SectionID) )
        {
            WriteToLog(QtDebugMsg, QString("Remove items BackGround"+QString::number(SectionID)) );
            removeItem(findBG);
        }
    }

    if((BG_Id>=0) && (BG_Id <= pConfigs->main_bg.size() )) // Deny unexist ID
            LvlData->sections[SectionID].background = BG_Id;

    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    setSectionBG(LvlData->sections[SectionID]);
}


// ////////////////////////Apply section background/////////////////////////////
void LvlScene::setSectionBG(LevelSection section)
{
    QGraphicsPixmapItem * item=NULL;
    QGraphicsRectItem * itemRect=NULL;
    QBrush brush;
    QPen pen;
    QPixmap image = QPixmap(QApplication::applicationDirPath() + "/" + "data/nobg.gif");
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

        WriteToLog(QtDebugMsg, "SetSectionBG-> Check for user images");

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

            for(j=0;j<pConfigs->main_bg.size();j++)
            {
                if(pConfigs->main_bg[j].id==section.background)
                {
                    noimage=false;
                      if(!isUser1)
                          img = pConfigs->main_bg[j].image;
                      if(!isUser2)
                          img2 = pConfigs->main_bg[j].second_image;
                    break;
                }
            }
            if((noimage)&&(!isUser1))
            {
                WriteToLog(QtWarningMsg, "SetSectionBG-> Image not found");
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
            item = new QGraphicsPixmapItem;
            DrawBG(x, y, w, h, img, img2, pConfigs->main_bg[j], item);
            //BgItem[section.id]->setParentItem(item);
            addItem(item);
            item->setPos(x, y);
        }
        else
        {
            itemRect = new QGraphicsRectItem;
            itemRect->setPen(pen);
            itemRect->setBrush(brush);
            itemRect->setRect(x, y, (long)fabs(x-w), (long)fabs(y-h));
            addItem(itemRect);
        }

        WriteToLog(QtDebugMsg, QString("SetSectionBG-> Item placed to x=%1 y=%2 h=%3 w=%4").arg(x).arg(y)
                   .arg((long)fabs(x-w)).arg((long)fabs(y-h)));
        //

        if(item!=NULL)
        {
            item->setData(0, "BackGround"+QString::number(section.id) );
            item->setZValue(bgZ);
        }
        if(itemRect!=NULL)
        {
            itemRect->setData(0, "BackGround"+QString::number(section.id) );
            itemRect->setZValue(bgZ);
        }

    } //Don't draw on reserved section entry

}




// ////////////////////////////////////Draw BG image/////////////////////////////////////////////////
void LvlScene::DrawBG(int x, int y, int w, int h, QPixmap srcimg, QPixmap srcimg2, obj_BG &bgsetup, QGraphicsPixmapItem * BgItem)
{
    /* Old Algorith */
    QPixmap BackImg;
    QPainter * BGPaint;
    QPixmap img;
    int si_attach, attach;
    long toX, toY, bgH, bgW;

    /* New Algorith */
    QGraphicsPixmapItem * itemP=NULL;
    QGraphicsRectItem * itemR=NULL;


    WriteToLog(QtDebugMsg, "Draw BG -> Draw BG Image");
    BackImg = QPixmap(QSize( (long)fabs(x-w), (long)fabs(y-h) ));
    attach = bgsetup.attached;
    long px = 0;

    if((bgsetup.type==0)&&(!bgsetup.editing_tiled))
    {   // ///////////////////SingleRow BG///////////////////////////

        WriteToLog(QtDebugMsg, "Draw BG -> Style: SingleRow BG");
        if(attach==0) // Get Pixel size
            BackImg.fill( srcimg.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,(srcimg.height()-1)) );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;
        bgW = srcimg.width();
        bgH = srcimg.height();

        do
        { //Draw row
            BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
            px += srcimg.width();
        }
        while( px < (long)fabs(x-w) );

    }
    else if((bgsetup.type==1)&&(!bgsetup.editing_tiled))
    {   // ///////////////////DoubleRow BG////////////////////////
        WriteToLog(QtDebugMsg, "Draw BG -> Style: DoubleRow BG");

        si_attach = bgsetup.second_attached; // Second image attach

        //Fill empty space
        if((!srcimg2.isNull()) && (si_attach==0))
            BackImg.fill( srcimg2.toImage().pixel(0,0) );
        else
            BackImg.fill( srcimg.toImage().pixel(0,0) );

        BGPaint = new QPainter(&BackImg);

        px=0;

        toY = (long)fabs(y-h)-srcimg.height();
        bgW = srcimg.width();
        bgH = srcimg.height();

        WriteToLog(QtDebugMsg, QString("Draw BG -> Draw first row, params: "));

        //Draw first row
            do{
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg);
                px += srcimg.width();
            } while( px < (long)fabs(x-w) );

        WriteToLog(QtDebugMsg, "Draw BG -> Draw second row");
        px=0;

        if(si_attach==0) // over first
            toY = (long)fabs(y-h)-srcimg.height()-srcimg2.height();
        else
        if(si_attach==1) // bottom
            toY = (long)fabs(y-h)-srcimg2.height();

        bgW = srcimg2.width();
        bgH = srcimg2.height();

        if(!srcimg2.isNull())
        {
            //Draw seconf row if it no null
            do {
                BGPaint->drawPixmap(px, toY, bgW, bgH, srcimg2);
                px += srcimg2.width();
            } while( px < (long)fabs(x-w) );
        } else WriteToLog(QtDebugMsg, "Draw BG -> second image is Null");
    }
    else
    { // ///////////////////////////////Tiled BG///////////////////////////////
        WriteToLog(QtDebugMsg, "Draw BG -> Style: Tiled");
        BackImg.fill( Qt::black );
        BGPaint = new QPainter(&BackImg);

        px=0;
        if(attach==0)
            toY = (long)fabs(y-h)-srcimg.height();
        else
            toY = 0;

        bgW = srcimg.width();
        bgH = srcimg.height();

        toX = 0;

        do{ // Draw colomn
            px=0;
            do
            { //Draw row
                BGPaint->drawPixmap(px, toY + toX*((attach==0)?(-1):1), bgW, bgH, srcimg);
                px += srcimg.width();
            }
            while( px < (long)fabs(x-w) );
            toX+=srcimg.height();
        }   while( toX < (long)fabs(y-h) + srcimg.height() );


    }
    BGPaint->end();

    WriteToLog(QtDebugMsg, "Draw BG -> Drawed");

    if(!BackImg.isNull())
        img = BackImg.copy(BackImg.rect());
    else
    {
        WriteToLog(QtDebugMsg, "Draw BG -> Drawed PixMap is null");
        img.fill( Qt::red );
    }
    WriteToLog(QtDebugMsg, "Draw BG -> Added to QPixmap");

    BgItem->setPixmap(QPixmap(img));
    //return img;
}







//  ////////////////////////////Drawing the inactive intersection space////////////////////////////////////
void LvlScene::drawSpace()
{
    const long padding = 400000;

    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find and remove current"));
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="Space")
        {
            removeItem(spaceItem);
            break;
        }
    }
    foreach(QGraphicsItem * spaceItem, items())
    {
        if(spaceItem->data(0).toString()=="SectionBorder")
        {
            removeItem(spaceItem);
            break;
        }
    }

    QPolygon bigSpace;
    QGraphicsItem * item;
    QGraphicsItem * item2;
    QVector<QPoint > drawing;

    int i, j;
    long l, r, t, b;
         //x, y, h, w;

    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Find minimal"));
    j=-1;
    do
    {
        j++;
        l = LvlData->sections[j].size_left;
        r = LvlData->sections[j].size_right;
        t = LvlData->sections[j].size_top;
        b = LvlData->sections[j].size_bottom;
    }
    while(
          ((LvlData->sections[j].size_left==0) &&
          (LvlData->sections[j].size_right==0) &&
          (LvlData->sections[j].size_top==0) &&
          (LvlData->sections[j].size_bottom==0)) && (j<LvlData->sections.size())
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

    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Draw polygon"));

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


    WriteToLog(QtDebugMsg, QString("Draw intersection space-> Draw editing hole"));
    drawing.clear();
    drawing.push_back(QPoint(l-1, t-1));
    drawing.push_back(QPoint(r+1, t-1));
    drawing.push_back(QPoint(r+1, b+1));
    drawing.push_back(QPoint(l-1, b+1));
    drawing.push_back(QPoint(l-1, t-1));

    bigSpace = bigSpace.subtracted(QPolygon(drawing));

    WriteToLog(QtDebugMsg, QString("Draw intersection space-> add polygon to Item"));
    item = addPolygon(bigSpace, QPen(Qt::NoPen), QBrush(Qt::black));//Add inactive space
    item2 = addPolygon(QPolygon(drawing), QPen(Qt::red, 2));
    item->setZValue(spaceZ1);
    item2->setZValue(spaceZ2);
    item->setOpacity(qreal(0.4));
    item->setData(0, "Space");
    item2->setData(0, "SectionBorder");

}

