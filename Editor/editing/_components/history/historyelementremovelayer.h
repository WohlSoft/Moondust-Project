#ifndef HISTORYELEMENTREMOVELAYER_H
#define HISTORYELEMENTREMOVELAYER_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementRemoveLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementRemoveLayer(LevelData modData, QObject *parent = 0);
    virtual ~HistoryElementRemoveLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    LevelData m_modData;
};

#endif // HISTORYELEMENTREMOVELAYER_H
