#ifndef ITEMS_H
#define ITEMS_H

#include <QPixmap>

#include "../defines.h"
#include "mainwinconnect.h"

class Items
{
public:
    /*!
     * \brief Retreives sprite or single frame of the requested element
     * \param itemType Type of the element (Block, BGO, NPC, Terrain tile, World map scenery, Path tile, Level tile)
     * \param ItemID ID of requested element
     * \param whole Retreive whole srite, overwise single default frame will be retured
     * \param scene Pointer to edit scene, if NULL, current active scene will be retreived
     * \return Pixmap which contains requested sprite
     */
    static QPixmap getItemGFX(int itemType, unsigned long ItemID, bool whole=false, QGraphicsScene *scene=NULL);
    /*!
     * \brief Convert item type string into ItemType enum enum
     * \param type String contains a word which specifies item type
     * \return Item type ID enumerated value
     */
    static int getItemType(QString type);
};

#endif // ITEMS_H
