/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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
#include <common_features/themes.h>
#include <editing/edit_level/level_edit.h>
#include <PGE_File_Formats/file_formats.h>

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

    if( (sect >= m_data->sections.size()) && (sect<0) )
    {
        //Expand sections
        int needToAdd = (m_data->sections.size()-1) - sect;
        while(needToAdd > 0)
        {
            LevelSection dummySct = FileFormats::CreateLvlSection();
            dummySct.id = m_data->sections.size();
            m_data->sections.push_back(dummySct);
            needToAdd--;
        }
    }

    long x,y,h,w;

    if(
        (m_data->sections[sect].size_left==0) &&
        (m_data->sections[sect].size_top==0) &&
        (m_data->sections[sect].size_bottom==0) &&
        (m_data->sections[sect].size_right==0)
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
           for(i=0;i<m_data->sections.size(); i++)
           {
               if(i == sect) continue;

               leftB = m_data->sections[i].size_left;
               rightB = m_data->sections[i].size_right;
               topB = m_data->sections[i].size_top;
               bottomB = m_data->sections[i].size_bottom;

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
       m_data->sections[sect].size_left = x;
       m_data->sections[sect].size_top = y;
       m_data->sections[sect].size_right = w;
       m_data->sections[sect].size_bottom = h;

       //set position to initialized section
       m_data->sections[sect].PositionY = y-10;
       m_data->sections[sect].PositionX = x-10;

       m_data->sections[sect].bgcolor = 16291944;

       #ifdef _DEBUG_
       WriteToLog(QtDebugMsg, QString("InitSection -> Data was applyed X%1 Y%2 W%3 H%4")
                  .arg(x).arg(y).arg(w).arg(h));
       #endif
    }
    setSectionBG( m_data->sections[sect] );
}




void LvlScene::ChangeSectionBG(int BG_Id, int SectionID, bool forceTiled)
{
    int sctID=0;
    if(SectionID<0)
        sctID = m_data->CurSection;
    else
        sctID = SectionID;

    foreach (QGraphicsItem * findBG, items() )
    {
        if(findBG->data(ITEM_TYPE).toString()== QString("BackGround%1").arg(m_data->sections[sctID].id) )
        {
            #ifdef _DEBUG_
            WriteToLog(QtDebugMsg, QString("Remove items "+findBG->data(ITEM_TYPE).toString()+" by id="+QString::number(sctID)+" by SctID="+QString::number(LvlData->sections[sctID].id)) );
            #endif
            removeItem(findBG);
            if(findBG) delete findBG;
        }
    }

    if(m_configs->main_bg.contains(BG_Id))
        m_data->sections[sctID].background = BG_Id;
    else
        m_data->sections[sctID].background = 0;

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "set Background to "+QString::number(BG_Id));
    #endif
    setSectionBG(m_data->sections[sctID], forceTiled);
}




// ////////////////////////Apply section background/////////////////////////////
void LvlScene::setSectionBG(LevelSection section, bool forceTiled)
{
    //QGraphicsPixmapItem * item=NULL;
    QGraphicsRectItem * itemRect=NULL;
    QBrush brush;
    QPen pen;
    QPixmap image = Themes::Image(Themes::dummy_bg);
    QPixmap nullimage;
    obj_BG *bgConfig=NULL;
    QPixmap *img=NULL;
    QPixmap *img2=NULL; //Second image buffer
    //need a BGitem

    bool noimage=false, wrong=false;
    long x,y,h,w;

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

        noimage=true;
        if(section.background != 0 )
        {
            //Find user image
            if(m_localConfigBackgrounds.contains(section.background))
            {
                noimage=false;
                obj_BG &bgx = m_localConfigBackgrounds[section.background];
                bgConfig=&bgx;
                img =&bgx.image;
                img2=&bgx.second_image;
            } else //If not exist, will be used default
            if(m_configs->main_bg.contains(section.background))
            {
                noimage=false;
                obj_BG &bgx = m_configs->main_bg[section.background];
                bgConfig=&bgx;
                img =&bgx.image;
                img2=&bgx.second_image;
            }
            if(noimage)
            {
                #ifdef _DEBUG_
                WriteToLog(QtWarningMsg, "SetSectionBG-> Image not found");
                #endif
                img=&image;
                img2=&nullimage;
                wrong=true;
            }
        }
        else noimage=true;

        brush = QBrush(QColor(0, 0, 0));
        pen = QPen(Qt::NoPen);

        if((!noimage)&&(!img->isNull()))
        {
            DrawBG(x, y, w, h, section.id, *img, *img2, *bgConfig, forceTiled);
        }
        else
        {
            if(wrong)
                brush.setTexture(image);
            itemRect = new QGraphicsRectItem;
            itemRect->setPen(pen);
            itemRect->setBrush(brush);
            itemRect->setRect(x, y, labs(x-w), labs(y-h));
            addItem(itemRect);
        }

        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, QString("SetSectionBG-> Item placed to x=%1 y=%2 h=%3 w=%4").arg(x).arg(y)
                   .arg((long)fabs(x-w)).arg((long)fabs(y-h)));
        #endif
        //

        if(itemRect!=NULL)
        {
            itemRect->setData(ITEM_TYPE, QString("BackGround%1").arg(section.id) );
            itemRect->setZValue(Z_backImage);
        }

    } //Don't draw on reserved section entry

}


bool LvlScene::isInSection(long x, long y, int sectionIndex, int padding)
{
    assert(sectionIndex >= 0);
    assert(sectionIndex < m_data->sections.size());
    return (x >= m_data->sections[sectionIndex].size_left-padding) &&
        (x <= m_data->sections[sectionIndex].size_right+padding) &&
        (y >= m_data->sections[sectionIndex].size_top-padding) &&
             (y <= m_data->sections[sectionIndex].size_bottom+padding);
}


bool LvlScene::isInSection(long x, long y, int width, int height, int sectionIndex, int padding)
{
    assert(sectionIndex >= 0);
    assert(sectionIndex < m_data->sections.size());
    LevelSection& sct = m_data->sections[sectionIndex];
    int r = x+width;
    int b = y+height;
    if(r < sct.size_left - padding)
        return false;
    if(x > sct.size_right + padding)
        return false;
    if(b < sct.size_top - padding)
        return false;
    if(y > sct.size_bottom + padding)
        return false;
    return true;
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

    sctW = abs(x-w);
    sctH = abs(y-h);

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "Draw BG -> Draw BG Image");
    #endif

    attach = bgsetup.setup.attached;

// ///////////////////SingleRow BG///////////////////////////
    if((bgsetup.setup.type==0)&&(!bgsetup.setup.editing_tiled)&&(!forceTiled))
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
        item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
        item->setPos(x, y+toY);
        item->setZValue(Z_backImage);

        if(R1H < sctH)
        {
            item = addRect(0, 0, sctW, sctH-R1H, Qt::NoPen, QBrush(FillColor));
            item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y+RectPlus);
            item->setZValue(Z_backImage);
        }
    }
    else

// ///////////////////DoubleRow BG////////////////////////
    if((bgsetup.setup.type==1)&&(!bgsetup.setup.editing_tiled)&&(!forceTiled))
    {
        #ifdef _DEBUG_
        WriteToLog(QtDebugMsg, "Draw BG -> Style: DoubleRow BG");
        #endif

        si_attach = bgsetup.setup.second_attached; // Second image attach

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
        item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
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
            item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
            item->setPos(x, y+toY);
            item->setZValue(Z_backImage+0.0000000001);
            // /////////////////////Draw second row//////////////////

        } else
            if(srcimg2.isNull())
                LogWarning("Draw BG -> second image is Null");


        if( R1H+RectPlus < sctH )
        {
            item = addRect(0, 0, sctW, sctH-R1H-RectPlus, Qt::NoPen, QBrush(FillColor));
            item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
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
            item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y);
            item->setZValue(Z_backImage);

            if(sctH >= R1H)
            {
                item = addRect(0, 0, sctW, sctH-RectPlus, Qt::NoPen, QBrush(srcimg));
                item->setData(ITEM_TYPE, "BackGround"+QString::number(sctID) );
                item->setPos(x,y+RectPlus);
                item->setZValue(Z_backImage);
            }

        }
        else
        {
            //Attached to Top
            item = addRect(0, 0, sctW, sctH, Qt::NoPen, QBrush(srcimg));
            item->setData(ITEM_TYPE, QString("BackGround%1").arg(sctID) );
            item->setPos(x,y);
            item->setZValue(Z_backImage);
        }
    }

    #ifdef _DEBUG_
    WriteToLog(QtDebugMsg, "acceptedID is "+QString::number(sctID)+" data is "+item->data(ITEM_TYPE).toString());
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
        if(spaceItem->data(ITEM_TYPE).toString()=="Space")
        {
            removeItem(spaceItem); delete spaceItem;
            continue;
        }
        if(spaceItem->data(ITEM_TYPE).toString()=="SectionBorder")
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
        l = m_data->sections[j].size_left;
        r = m_data->sections[j].size_right;
        t = m_data->sections[j].size_top;
        b = m_data->sections[j].size_bottom;
        j++;
    }
    while(
          ((m_data->sections[j-1].size_left==0) &&
          (m_data->sections[j-1].size_right==0) &&
          (m_data->sections[j-1].size_top==0) &&
          (m_data->sections[j-1].size_bottom==0)) && (j<m_data->sections.size())
    );

    for(i=0;i<m_data->sections.size(); i++)
    {

        if(
                (m_data->sections[i].size_left==0) &&
                (m_data->sections[i].size_right==0) &&
                (m_data->sections[i].size_top==0) &&
                (m_data->sections[i].size_bottom==0))
            continue;

        if(m_data->sections[i].size_left < l)
            l = m_data->sections[i].size_left;
        if(m_data->sections[i].size_right > r)
            r = m_data->sections[i].size_right;
        if(m_data->sections[i].size_top < t)
            t = m_data->sections[i].size_top;
        if(m_data->sections[i].size_bottom > b)
            b = m_data->sections[i].size_bottom;
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


    l = m_data->sections[m_data->CurSection].size_left;
    r = m_data->sections[m_data->CurSection].size_right;
    t = m_data->sections[m_data->CurSection].size_top;
    b = m_data->sections[m_data->CurSection].size_bottom;


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
    item->setData(ITEM_TYPE, "Space");
    item2->setData(ITEM_TYPE, "SectionBorder");
}


