#pragma once
#ifndef ITEMS_H
#define ITEMS_H

#include <QPixmap>

#include "../defines.h"
#include "../mainwindow.h"

class Items
{
public:
    /*!
     * \brief Retreives sprite or single frame of the requested element
     * \param [__in] itemType Type of the element (Block, BGO, NPC, Terrain tile, World map scenery, Path tile, Level tile)
     * \param [__in] ItemID ID of requested element
     * \param [__out] outImg Target Pixmap where will be stored requested sprite
     * \param [__in] scene Pointer to edit scene to access custom graphics, if NULL, default configs are will be used
     * \param [__in] whole Retreive entire srite, overwise single default frame will be retured
     * \param [__in] targetSize Scale image to requested size, if 0:0 sent, image will be returned as-is
     */
    static void getItemGFX(int itemType, unsigned long ItemID, QPixmap &outImg, QGraphicsScene *scene=NULL, bool whole=false, QSize targetSize=QSize(0,0));

    /*!
     * \brief Retreives sprite or single frame of the block object configuration
     * \param [__in]  inObj Block object config structure
     * \param [__out] outImg Target Pixmap where will be stored requested sprite
     * \param [__in]  whole Retreive entire srite, overwise single default frame will be retured
     * \param [__in]  targetSize Scale image to requested size, if 0:0 sent, image will be returned as-is
     */

    static void getItemGFX(const obj_block* inObj, QPixmap &outImg, bool whole=false, QSize targetSize=QSize(0,0));
    /*!
     * \brief Retreives sprite or single frame of the BGO object configuration
     * \param [__in]  inObj BGO object config structure
     * \param [__out] outImg Target Pixmap where will be stored requested sprite
     * \param [__in]  whole Retreive entire srite, overwise single default frame will be retured
     * \param [__in]  targetSize Scale image to requested size, if 0:0 sent, image will be returned as-is
     */

    static void getItemGFX(const obj_bgo* inObj, QPixmap &outImg, bool whole=false, QSize targetSize=QSize(0,0));
    /*!
     * \brief Retreives sprite or single frame of the NPC object configuration
     * \param [__in]  inObj NPC object config structure
     * \param [__out] outImg Target Pixmap where will be stored requested sprite
     * \param [__in]  whole Retreive entire srite, overwise single default frame will be retured
     * \param [__in]  targetSize Scale image to requested size, if 0:0 sent, image will be returned as-is
     */

    static void getItemGFX(const obj_npc* inObj, QPixmap &outImg, bool whole=false, QSize targetSize=QSize(0,0));

    /*!
     * \brief Retreives sprite or single frame of the any generic world map object configuration
     * \param [__in]  inObj Generic world map object config structure
     * \param [__out] outImg Target Pixmap where will be stored requested sprite
     * \param [__in]  whole Retreive entire srite, overwise single default frame will be retured
     * \param [__in]  targetSize Scale image to requested size, if 0:0 sent, image will be returned as-is
     */
    static void getItemGFX(const obj_wld_generic* inObj, QPixmap &outImg, bool whole=false, QSize targetSize=QSize(0,0));

    /*!
     * \brief Convert item type string into ItemType enum enum
     * \param type String contains a word which specifies item type
     * \return Item type ID enumerated value
     */
    static int getItemType(QString type);
};

#endif // ITEMS_H
