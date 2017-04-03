#ifndef LVL_QUAD_TREE_H
#define LVL_QUAD_TREE_H

#include "lvl_base_object.h"
#include <memory>

struct LvlQuadTree_private;
class LvlQuadTree
{
    std::unique_ptr<LvlQuadTree_private> p;
public:
    LvlQuadTree();
    LvlQuadTree(const LvlQuadTree &qt) = delete;
    ~LvlQuadTree();

    bool insert(PGE_Phys_Object*obj);
    bool update(PGE_Phys_Object*obj);
    bool remove(PGE_Phys_Object*obj);
    void clear();
    typedef bool (*t_resultCallback)(PGE_Phys_Object*, void*);
    void query(PGE_RectF &zone, t_resultCallback a_resultCallback, void *context);
};

#endif // LVL_QUAD_TREE_H

