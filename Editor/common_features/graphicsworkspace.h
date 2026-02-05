#pragma once
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

class QGestureEvent;
class QPanGesture;
class QPinchGesture;


// Internal replacement for QMouseEvent which is impossible to copy at Qt6
class PGEMouseEvent
{
public:
    PGEMouseEvent() = default;
    PGEMouseEvent(const PGEMouseEvent &o) = default;
    PGEMouseEvent &operator=(const PGEMouseEvent &o) = default;
    PGEMouseEvent(const QMouseEvent *o);

    PGEMouseEvent(QEvent::Type type,
                  const QPointF &localPos,
                  const QPointF &windowPos,
                  const QPointF &screenPos,
                  Qt::MouseButton button,
                  Qt::MouseButtons buttons,
                  Qt::KeyboardModifiers modifiers);

    // Compatibility to original QMouseEvent
    PGEMouseEvent &operator=(const QMouseEvent *o);

    QEvent::Type type() const { return m_type; }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QInputDevice *device() const { return m_device; }
#endif
    QPoint pos() const { return m_pos; }
    QPointF localPos() const { return m_localPos; }
    QPointF globalPos() const { return m_globalPos; }
    QPointF screenPos() const { return m_screenPos; }
    QPointF windowPos() const { return m_windowPos; }
    Qt::MouseButton button() const { return m_button; }
    Qt::MouseButtons buttons() const { return m_buttons; }
    Qt::KeyboardModifiers modifiers() const { return m_modifiers; }
    bool spontaneous() const { return m_spontanous; }

    bool isAccepted() const { return m_accepted; }
    void setAccepted(bool accepted) { m_accepted = accepted; }

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    const QInputDevice *m_device = nullptr;
#endif
    QEvent::Type m_type = QEvent::None;
    QPoint m_pos = QPoint();
    QPointF m_localPos = QPointF();
    QPointF m_globalPos = QPointF();
    QPointF m_windowPos = QPointF();
    QPointF m_screenPos = QPointF();
    Qt::MouseButton m_button = Qt::NoButton;
    Qt::MouseButtons m_buttons;
    Qt::KeyboardModifiers m_modifiers;
    bool m_accepted = false;
    bool m_spontanous = false;
};


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
    enum Movements
    {
        MOVE_IDLE = 0,
        MOVE_LEFT = 1,
        MOVE_UP = 2,
        MOVE_DOWN = 4,
        MOVE_RIGHT = 8
    };
    int movement;
    int step;
    int keyTime;
    qreal zoomValue;

    double scaleFactor;
    double scaleMin;
    double scaleMax;

    qreal scaleGestureFactor;

protected:
    virtual void focusOutEvent(QFocusEvent *event) override;

    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void keyReleaseEvent(QKeyEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

    virtual bool event(QEvent *event) override;
#ifdef __APPLE__
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
#endif
private:
    bool gestureEvent(QGestureEvent *event);

    void gesturePan(QPanGesture *gesture);
    void gesturePinch(QPinchGesture *gesture);

    void eventScroll(int dirx, int x, int diry, int y);
    void eventZoom(int dir, float delta);

protected:
    ///
    /// My Crazy "Bycicle" :P
    ///
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

    void mouseMoveEventHandler(PGEMouseEvent &event);
    void storeMouseEvent(const PGEMouseEvent &event);
    void storeMouseEvent(const QMouseEvent *event);

    QRegion rubberBandRegion(const QWidget *widget, const QRect &rect) const;
    void updateRubberBand(const PGEMouseEvent &event);


    QRect rubberBandRect;
    //QRegion rubberBandRegion(const QWidget *widget, const QRect &rect) const;
    bool rubberBanding;
    Qt::ItemSelectionMode rubberBandSelectionMode;
    // [ASM] Multi-selection
    bool useLastMouseEvent;
    PGEMouseEvent lastMouseEvent;
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
    void replayLastMouseEvent(int x = 0);
};

#endif // GRAPHICSWORKSPACE_H
