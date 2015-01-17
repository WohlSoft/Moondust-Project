#ifndef IHISTORYELEMENT_H
#define IHISTORYELEMENT_H

#include <QObject>
#include <QGraphicsScene>


///
/// \brief The IHistoryElement class is a base class for all history elements
///
class IHistoryElement
{
public:
    virtual ~IHistoryElement(){}
    ///
    /// \brief getHistoryName Returns the history name
    /// \return The History Name
    ///
    virtual QString getHistoryName() = 0;
    ///
    /// \brief undo Undos this operation.
    ///
    virtual void undo() = 0;
    ///
    /// \brief redo Redos this operation.
    ///
    virtual void redo() = 0;

    QGraphicsScene *scene() const;
    void setScene(QGraphicsScene *scene);

protected:
    QGraphicsScene* m_scene;
};


Q_DECLARE_INTERFACE(IHistoryElement, "IHistoryElement")

#endif // IHISTORYELEMENT_H
