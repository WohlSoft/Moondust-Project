#ifndef HISTORYELEMENTMERGELAYER_H
#define HISTORYELEMENTMERGELAYER_H

#include <QObject>

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementMergeLayer : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementMergeLayer(const LevelData &mergedData, const QString &newLayerName, QObject *parent = 0);
    virtual ~HistoryElementMergeLayer();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

private:
    LevelData m_mergedData;
    QString m_newLayerName;
};


#endif // HISTORYELEMENTMERGELAYER_H
