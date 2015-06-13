#ifndef HISTORYELEMENTNEWLAYER_H
#define HISTORYELEMENTNEWLAYER_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementNewLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementNewLayer(const int array_id, const QString &name, QObject *parent = 0);
    virtual ~HistoryElementNewLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    QString m_name;
};

#endif // HISTORYELEMENTNEWLAYER_H
