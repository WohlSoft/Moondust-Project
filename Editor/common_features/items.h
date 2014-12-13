#ifndef ITEMS_H
#define ITEMS_H

#include <QPixmap>

#include "../defines.h"
#include "mainwinconnect.h"

class Items
{
public:
    ///
    /// \brief getItemGFX Get pixmap by id
    /// \param itemType type of item, defined by enum ItemTypes::itemTypes
    /// \param ItemID ID of target item
    /// \param whole return whole image. If "false" will be returned only one frame
    /// \param confId pointer, where will be write the config index by item type
    /// \return The Pixmap
    ///
    static QPixmap getItemGFX(int itemType, unsigned long ItemID, bool whole=false, long  *confId = NULL, QGraphicsScene *scene=NULL);
};

#endif // ITEMS_H
