#include "mw.h"

CalibrationMain * MW::p = nullptr;


QPixmap MW::sprite()
{
    if(p)
        return p->m_xImageSprite;
    else
        return QPixmap();
}
