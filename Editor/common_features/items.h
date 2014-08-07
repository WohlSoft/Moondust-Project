#ifndef ITEMS_H
#define ITEMS_H

#include <QPixmap>
#include "../defines.h"
#include "mainwinconnect.h"

class Items
{
public:
    static QPixmap getItemGFX(int itemType, unsigned long ItemID, bool whole=false, long  *confId = NULL);
    /*
     * itemType - type of item, defined by enum ItemTypes::itemTypes
     * ItemID   - ID of target item
     * whole    - return whole image. If "false" will be returned only one frame
     * confId   - pointer, where will be write the config index by item type
     */
};

#endif // ITEMS_H
