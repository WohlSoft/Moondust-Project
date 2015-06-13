#include "ihistoryelement.h"

QGraphicsScene *IHistoryElement::scene() const
{
    return m_scene;
}

void IHistoryElement::setScene(QGraphicsScene *scene)
{
    m_scene = scene;
}
