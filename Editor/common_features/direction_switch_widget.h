#ifndef DIRECTION_SWITCH_WIDGET_H
#define DIRECTION_SWITCH_WIDGET_H

#include <QWidget>
#include <QPixmap>
#include <QHash>

// TODO: Implement an ability to turn a widget into absolute degree direction pointing

/**
 * @brief A convenient controller to set a enumerated direction value of something
 *
 * This widget gives a 3x3 grid of buttons: each button can be assigned for a role,
 * can take a picture, may act as a button (when a mapped value is presented)
 * or as a static picture (when no mapped value).
 */
class DirectionSwitchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectionSwitchWidget(QWidget *parent = nullptr);
    virtual ~DirectionSwitchWidget();

    /**
     * @brief Direction buttons
     */
    enum Sides
    {
        S_UNKNOWN = -1,
        S_CENTER = 0,
        S_LEFT,
        S_TOP,
        S_RIGHT,
        S_BOTTOM,
        S_TOP_LEFT,
        S_TOP_RIGHT,
        S_BOTTOM_LEFT,
        S_BOTTOM_RIGHT
    };

    /**
     * @brief Enable using of diagonal directions
     * @param diagonals Show and enable diagonal directions
     */
    void setUseDiagonals(bool diagonals);

    /**
     * @brief Are diagonal directions enabled
     * @return true when diagonal directions are enabled
     */
    bool useDiagonals();

    /**
     * @brief Set a value of direction (values from the map are acceptable)
     * @param direction Value of direction in condition it's exist in the map
     */
    void setDirection(int direction);

    /**
     * @brief Current value of direction
     * @return integer code of direction map entry
     */
    int  direction();

    /**
     * @brief Rotate the icon at center in condition of current direction (default state of icon - pointing to up)
     * @param rc true to enable center icon rotation in favor of direction
     */
    void setRotatingCenter(bool rc);
    /**
     * @brief Rotate the icon at center in condition of current direction (default state of icon - pointing to up)
     * @return true if center icon rotation is enabled
     */
    bool rotatingCenter();

    /**
     * @brief Assign normal state pixmap with a side of widget
     * @param side Widget's side
     * @param pixmap Picture that will be shown in a normal state
     */
    void setPixmap(Sides side, const QPixmap &pixmap);

    /**
     * @brief Assign enabled state pixmap with a side of widget
     * @param side Widget's side
     * @param pixmap Picture that will be shown in an enabled state
     */
    void setPixmapOn(Sides side, const QPixmap &pixmap);

    /**
     * @brief Set a mapping of value with a specific side of widget
     * @param side Widget's side
     * @param value Integer value
     */
    void mapValue(Sides side, int value);

    /**
     * @brief Remove mapping entry for specific side of widget
     * @param side Widget's side
     */
    void unMapValue(Sides side);

    /**
     * @brief Assign a tooltip with a side of widget (will be show in a condition that mapped value is presented)
     * @param side Widget's side
     * @param toolTip A text of tooltip
     */
    void mapToolTip(Sides side, QString toolTip);

protected:
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void leaveEvent(QEvent *event);
    void paintEvent(QPaintEvent *);
    bool event(QEvent *event);
    QSize sizeHint() const;

signals:
    /**
     * @brief Executing when a direction value was changed by user or by setDirection() call
     * @param direction A direction value that was set
     */
    void directionChanged(int direction);
    /**
     * @brief Executing when a direction value was changed by user by mouse clicking only
     * @param direction A direction value that was set
     */
    void clicked(int direction);

private:
    /**
     * @brief Find a side of widget, currently pointed by mouse at X and Y coords (not-mapped sides will NOT be return)
     * @param x X position of mouse cursor
     * @param y Y position of mouse cursor
     * @return A side of widget which is currently pointing by a mouse
     */
    Sides findSide(int x, int y);
    /**
     * @brief Find a side of widget, currently pointed by mouse at X and Y coords (not-mapped sides will NOT be return)
     * @param pos A position of mouse cursor
     * @return A side of widget which is currently pointing by a mouse
     */
    Sides findSide(const QPoint &pos);

    /**
     * @brief Get a rectangle box with a position and size of a widget's side
     * @param side Widget's size
     * @return Rectangle that includes a widget's side area
     */
    QRect getRect(Sides side);

    /**
     * @brief Get a pixmap assigned with a widget's side
     * @param side Widget's side
     * @return A pixmap with an icon or a null when not found or absence
     */
    const QPixmap &getPixMap(Sides side);

    /**
     * @brief Draw a button of specific side (private function used in a paint event)
     * @param painter Painter instance
     * @param cellWidth Width of a cell of 3x3 grid of widget
     * @param cellHeight Height of a cell of 3x3 grid of widget
     * @param side A side to draw
     */
    void drawButton(QPainter &painter,
                    int cellWidth, int cellHeight,
                    DirectionSwitchWidget::Sides side);

    //! Current value of direction (mapped value)
    int m_direction = 0;
    //! A currently selected side of widget
    Sides m_currentSide = S_UNKNOWN;
    //! A currently hovering by mouse side of widget
    Sides m_hoverSide = S_UNKNOWN;
    //! A currently pressing by mouse side of widget
    Sides m_pressSide = S_UNKNOWN;
    //! Are diagonal sides of widget enabled?
    bool m_hasDiagonals = false;
    //! Is rotating center mode enabled?
    bool m_rotatingCenter = false;

    //! A map of pixmaps per each side
    QHash<Sides, QPixmap>   m_pixmaps;
    //! A map of pixmaps with ON state per each side
    QHash<Sides, QPixmap>   m_pixmapsOn;
    //! A map of integer values per each side
    QHash<Sides, int>       m_valuesMap;
    //! A map of tooltips shown for each side
    QHash<Sides, QString>   m_toolTipsMap;
};

#endif // DIRECTION_SWITCH_WIDGET_H
