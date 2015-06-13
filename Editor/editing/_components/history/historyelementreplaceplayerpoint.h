#ifndef HISTORYELEMENTREPLACEPLAYERPOINT_H
#define HISTORYELEMENTREPLACEPLAYERPOINT_H

#include "ihistoryelement.h"
#include <PGE_File_Formats/file_formats.h>

class HistoryElementReplacePlayerPoint : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementReplacePlayerPoint(PlayerPoint plr, QVariant oldPos, QObject *parent = 0);
    virtual ~HistoryElementReplacePlayerPoint();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

public slots:
    void historyRemovePlayerPoint(const PlayerPoint &plr, QGraphicsItem *item);
private:
    PlayerPoint m_plr;
    QVariant m_oldPos;
};

#endif // HISTORYELEMENTREPLACEPLAYERPOINT_H
