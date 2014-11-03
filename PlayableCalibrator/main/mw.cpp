#include "mw.h"

CalibrationMain * MW::p = NULL;


QPixmap MW::sprite()
{
    if(p)
        return p->x_imageSprite;
    else
        return QPixmap();
}
