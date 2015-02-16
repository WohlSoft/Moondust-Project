#ifndef HISTORYELEMENTADDWARP_H
#define HISTORYELEMENTADDWARP_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementAddWarp : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementAddWarp(int array_id, int listindex, int doorindex, QObject *parent = 0);
    virtual ~HistoryElementAddWarp();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    int m_listindex;
    int m_doorindex;
};

#endif // HISTORYELEMENTADDWARP_H
