#ifndef THESCENE_H
#define THESCENE_H

#include <QWidget>
#include <QList>
#include <QRect>
#include <QSet>
#include <QTimer>

#include "pge_edit_scene_item.h"
#include "RTree.h"

class PGE_EditScene : public QWidget
{
    Q_OBJECT
public:
    explicit PGE_EditScene(QWidget *parent = 0);
    virtual ~PGE_EditScene();

    void addRect(int x, int y);

    void clearSelection();
    void moveSelection(int deltaX, int deltaY);

    void select(PGE_EditSceneItem& item);
    void deselect(PGE_EditSceneItem& item);
    void toggleselect(PGE_EditSceneItem& item);
    void setItemSelected(PGE_EditSceneItem& item, bool selected);

    void moveStart();
    void moveEnd(bool esc=false);


    typedef QList<PGE_EditSceneItem*> PGE_EditItemList;
    typedef RTree<PGE_EditSceneItem*, int, 2, int > IndexTree;
    QList<PGE_EditSceneItem> m_items;
    typedef int RPoint[2];
    IndexTree m_tree;
    struct RRect{int l; int t; int r; int b;};
    void queryItems(RRect &zone,  PGE_EditItemList *resultList);
    void queryItems(int x, int y, PGE_EditItemList *resultList);
    void registerElement(PGE_EditSceneItem *item);
    void unregisterElement(PGE_EditSceneItem *item);

    typedef QSet<PGE_EditSceneItem*> SelectionMap;
    SelectionMap    m_selectedItems;
    QPoint          m_mouseOld;
    QPoint          m_mouseBegin;
    QPoint          m_mouseEnd;

    bool            m_ignoreMove;
    bool            m_ignoreRelease;
    bool            m_moveInProcess;
    bool            m_rectSelect;

    QPoint          m_cameraPos;
    double          m_zoom;

    QPoint       mapToWorld(const QPoint &mousePos);
    QRect        applyZoom(const QRect &r);

    struct Mover
    {
        Mover():
            speedX(0),
            speedY(0),
            scrollStep(32),
            m_keys(K_NONE)
        {}
        QTimer  timer;
        int     speedX;
        int     speedY;

        int     scrollStep;

        enum Keys
        {
            K_NONE  = 0x00,
            K_LEFT  = 0x01,
            K_RIGHT = 0x02,
            K_UP    = 0x04,
            K_DOWN  = 0x08,
            K_SHIFT = 0x10,
        };

        unsigned int    m_keys;

        inline void set(Keys k, bool v)
        {
            if(v)
                m_keys |= k;
            else
                m_keys &= ~k;
        }

        inline bool key(Keys k)
        {
            return (m_keys & k) != 0;
        }

        inline bool noKeys()
        {
            return (m_keys & 0x0F) == 0;
        }

        void setLeft(bool key)
        {
            set(K_LEFT, key);
            updTimer();
        }

        void setRight(bool key)
        {
            set(K_RIGHT, key);
            updTimer();
        }

        void setUp(bool key)
        {
            set(K_UP, key);
            updTimer();
        }

        void setDown(bool key)
        {
            set(K_DOWN, key);
            updTimer();
        }

        void setFaster(bool key)
        {
            set(K_SHIFT, key);
            updTimer();
        }

        void reset()
        {
            m_keys = K_NONE;
            updTimer();
        }

        void updTimer()
        {
            speedX = 0;
            speedY = 0;
            if( key(K_LEFT) ^ key(K_RIGHT) )
            {
                speedX = scrollStep *(key(K_LEFT) ? -1 : 1);
            }

            if( key(K_UP) ^ key(K_DOWN) )
            {
                speedY = scrollStep *(key(K_UP) ? -1 : 1);
            }

            if( noKeys() )
                timer.stop();
            else
            {
                int interval = key(K_SHIFT) ? 8 : 32;
                if(timer.isActive())
                    timer.setInterval(interval);
                else
                    timer.start(interval);
            }
        }
    } m_mover;

    void moveCamera();
    void moveCamera(int deltaX, int deltaY);
    void moveCameraUpdMouse(int deltaX, int deltaY);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void focusInEvent(QFocusEvent *event);
    void focusOutEvent(QFocusEvent *event);
};



#endif // THESCENE_H
