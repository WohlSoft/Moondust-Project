#ifndef HISTORYELEMENTRESIZEBLOCK_H
#define HISTORYELEMENTRESIZEBLOCK_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>
class HistoryElementResizeBlock : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementResizeBlock(const LevelBlock &block, const QRect &oldSize, const QRect &newSize, QObject *parent = 0);
    virtual ~HistoryElementResizeBlock();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

signals:

public slots:
    void historyUndoResizeBlock(const LevelBlock &orig, QGraphicsItem *item);
    void historyRedoResizeBlock(const LevelBlock &orig, QGraphicsItem *item);
private:
    QRect m_oldSize;
    QRect m_newSize;
    LevelBlock m_block;
};

#endif // HISTORYELEMENTRESIZEBLOCK_H
