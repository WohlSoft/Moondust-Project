#ifndef PGE_EDIT_SCENE_ITEM_H
#define PGE_EDIT_SCENE_ITEM_H

#include <QRect>

class PGE_EditScene;
class PGE_EditSceneItem
{
    friend class PGE_EditScene;
    PGE_EditScene* m_scene;
    bool m_selected;

public:
    explicit PGE_EditSceneItem(PGE_EditScene* parent);
    PGE_EditSceneItem(const PGE_EditSceneItem &it);

    void setSelected(bool selected);
    bool selected();

    bool isTouches(int x, int y);
    bool isTouches(QRect &rect);

    QRect m_posRect;
    QRect m_posRectTree;
};

#endif // PGE_EDIT_SCENE_ITEM_H
