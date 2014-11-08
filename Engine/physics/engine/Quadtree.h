#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>

class Quadtree;
class PGE_PhysBody;

class Quadtree {
public:
						Quadtree(float x, float y, float width, float height, int level, int maxLevel);

						~Quadtree();

	void					AddObject(PGE_PhysBody *object);
    std::vector<PGE_PhysBody*>				GetObjectsAt(float x, float y);
	void					Clear();

private:
	float					x;
	float					y;
	float					width;
	float					height;
	int					level;
	int					maxLevel;
    std::vector<PGE_PhysBody*>				objects;

	Quadtree *				parent;
	Quadtree *				NW;
	Quadtree *				NE;
	Quadtree *				SW;
	Quadtree *				SE;

	bool					contains(Quadtree *child, PGE_PhysBody *object);
};

#endif
