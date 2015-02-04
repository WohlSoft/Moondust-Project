#ifndef HISTORYELEMENTRESIZESECTION_H
#define HISTORYELEMENTRESIZESECTION_H

#include "ihistoryelement.h"

class HistoryElementResizeSection : public QObject, public IHistoryElement
{
    Q_OBJECT
    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementResizeSection(int sectionID, const QRect &oldSize, const QRect &newSize, QObject *parent = 0);
    virtual ~HistoryElementResizeSection();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();

signals:

public slots:

private:
    QRect m_oldSize;
    QRect m_newSize;
    int m_sectionID;
};

#endif // HISTORYELEMENTRESIZESECTION_H
