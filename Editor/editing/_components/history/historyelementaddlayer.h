#ifndef HISTORYELEMENTADDLAYER_H
#define HISTORYELEMENTADDLAYER_H


#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementAddLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementAddLayer(int array_id, QString name, QObject *parent = 0);
    virtual ~HistoryElementAddLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    int m_array_id;
    QString m_name;
};

#endif // HISTORYELEMENTADDLAYER_H
