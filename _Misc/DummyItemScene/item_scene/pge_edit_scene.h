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

    void addRect(int x, int y);

    void clearSelection();
    void moveSelection(int deltaX, int deltaY);

    void select(PGE_EditSceneItem& item);
    void deselect(PGE_EditSceneItem& item);
    void toggleselect(PGE_EditSceneItem& item);
    void setItemSelected(PGE_EditSceneItem& item, bool selected);

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
    SelectionMap m_selectedItems;
    QPoint       m_mouseOld;
    QPoint       m_mouseBegin;
    QPoint       m_mouseEnd;
    bool         m_ignoreMove;
    bool         m_ignoreRelease;
    bool         m_rectSelect;
    QPoint       m_cameraPos;
    double       m_zoom;

    QPoint       mapToWorld(const QPoint &mousePos);
    QRect        applyZoom(const QRect &r);

    struct Mover
    {
        Mover(): speedX(0), speedY(0), keysH(K_NONE), keysV(K_NONE) {}
        QTimer  timer;
        int     speedX;
        int     speedY;
        enum Keys{
            K_NONE = 0,
            K_LEFT = 1,
            K_RIGHT= 2,
            K_UP   = 4,
            K_DOWN = 8,
        };
        int     keysH;
        int     keysV;
    } m_mover;

    void moveCamera();
    void moveCamera(int deltaX, int deltaY);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
};



#endif // THESCENE_H
