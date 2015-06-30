#ifndef GRAPHICSWORKSPACE_H
#define GRAPHICSWORKSPACE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QScrollBar>
#include <QKeyEvent>
#include <QEvent>
#include <QApplication>
#include <QStyle>
#include <QPainter>
#include <QRubberBand>

#include <QStyleHintReturnMask>
#include <QStyleOptionRubberBand>

class GraphicsWorkspace : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsWorkspace(QWidget *parent = 0);
    ~GraphicsWorkspace();
    qreal zoom();
    void setZoom(qreal zoom);
    void zoomIn();
    void zoomOut();
    QRubberBand *rubberBand() const;

signals:
    void zoomValueChanged(int percent);
    void zoomValueChanged(const QString &percent);

public slots:

private slots:
    void moveLeft();
    void moveRight();
    void moveUp();
    void moveDown();
    void doMove();
    void updateTimerInterval();

private:
    QTimer Mover;
    enum Movements{
        MOVE_IDLE=0,
        MOVE_LEFT=1,
        MOVE_UP=2,
        MOVE_DOWN=4,
        MOVE_RIGHT=8
    };
    int movement;
    int step;
    int keyTime;
    qreal zoomValue;

    double scaleFactor;
    double scaleMin;
    double scaleMax;

protected:
    virtual void 	focusOutEvent ( QFocusEvent * event );

    virtual void 	keyPressEvent ( QKeyEvent * event );
    virtual void 	keyReleaseEvent ( QKeyEvent * event );
    virtual void 	wheelEvent ( QWheelEvent * event );


    ///
    /// My Crazy "Bycicle" :P
    ///
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent * event);
    virtual void mouseReleaseEvent( QMouseEvent * event);

    void mouseMoveEventHandler(QMouseEvent *event);
    void storeMouseEvent(QMouseEvent *event);

    QRegion rubberBandRegion(const QWidget *widget, const QRect &rect) const;
    void updateRubberBand(const QMouseEvent *event);


    QRect rubberBandRect;
    //QRegion rubberBandRegion(const QWidget *widget, const QRect &rect) const;
    bool rubberBanding;
    Qt::ItemSelectionMode rubberBandSelectionMode;
    // [ASM] Multi-selection
    bool useLastMouseEvent;
    QMouseEvent lastMouseEvent;
    QList<QGraphicsItem *> rubberBandExtendSelection;
    QPoint mousePressViewPoint;
    QPointF mousePressScenePoint;
    QPoint mousePressScreenPoint;
    QPointF lastMouseMoveScenePoint;
    QPoint lastMouseMoveScreenPoint;

    QCursor originalCursor;

    Qt::MouseButton mousePressButton;

    bool handScrolling;
    int handScrollMotions;

    bool hasStoredOriginalCursor;

    void _q_setViewportCursor(const QCursor &cursor);
    void _q_unsetViewportCursor();
    QList<QGraphicsItem *> cachedItemsUnderMouse;
    QPointF lastRubberbandScenePoint;

    QPoint originPoint;
    QRubberBand *rubberBandX;

protected slots:
    void replayLastMouseEvent(int x=0);

};

#endif // GRAPHICSWORKSPACE_H
