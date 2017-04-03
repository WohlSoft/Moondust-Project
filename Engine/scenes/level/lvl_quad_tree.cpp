
#include "lvl_quad_tree.h"
#include <common_features/QuadTree/LooseQuadtree.h>

class QTreePGE_Phys_ObjectExtractor
{
public:
    static void ExtractBoundingBox(const PGE_Phys_Object *object, loose_quadtree::BoundingBox<double> *bbox)
    {
        bbox->left      = object->m_momentum.x;
        bbox->top       = object->m_momentum.y;
        bbox->width     = object->m_momentum.w;
        bbox->height    = object->m_momentum.h;
    }
};

struct LvlQuadTree_private
{
    typedef loose_quadtree::LooseQuadtree<double, PGE_Phys_Object, QTreePGE_Phys_ObjectExtractor> IndexTreeQ;
    IndexTreeQ tree;
};


LvlQuadTree::LvlQuadTree() :
    p(new LvlQuadTree_private)
{}

LvlQuadTree::~LvlQuadTree()
{}

bool LvlQuadTree::insert(PGE_Phys_Object *obj)
{
    return p->tree.Insert(obj);
}

bool LvlQuadTree::update(PGE_Phys_Object *obj)
{
    return p->tree.Update(obj);
}

bool LvlQuadTree::remove(PGE_Phys_Object *obj)
{
    return p->tree.Remove(obj);
}

void LvlQuadTree::clear()
{
    p->tree.Clear();
}

void LvlQuadTree::query(PGE_RectF &zone, LvlQuadTree::t_resultCallback a_resultCallback, void *context)
{
    LvlQuadTree_private::IndexTreeQ::Query q = p->tree.QueryIntersectsRegion(loose_quadtree::BoundingBox<double>(zone.x(), zone.y(), zone.width(), zone.height()));
    while(!q.EndOfQuery())
    {
        a_resultCallback(q.GetCurrent(), context);
        q.Next();
    }
}
