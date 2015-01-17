#ifndef HISTORYELEMENTMODIFICATION_H
#define HISTORYELEMENTMODIFICATION_H

#include "ihistoryelement.h"
#include <file_formats/file_formats.h>

class HistoryElementModification : public QObject, public IHistoryElement
{
    Q_OBJECT

    Q_INTERFACES(IHistoryElement)

public:
    explicit HistoryElementModification(const LevelData &oldData, const LevelData &newData, QObject *parent = 0);
    virtual ~HistoryElementModification();
    virtual QString getHistoryName();
    virtual void undo();
    virtual void redo();


    LevelData oldData() const;
    void setOldData(const LevelData &oldData);

    LevelData newData() const;
    void setNewData(const LevelData &newData);

    QString customHistoryName() const;
    void setCustomHistoryName(const QString &customHistoryName);

signals:

public slots:


private:
    LevelData m_oldData;
    LevelData m_newData;
    QString m_customHistoryName;
};

#endif // HISTORYELEMENTMODIFICATION_H
