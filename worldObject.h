#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include <vector>
#include "vector.h"
#include "material.h"
#include "CollisionPoint.h"

using std::vector;

// A WorldObject is just something that a ray can hit!
class WorldObject {
public:
	WorldObject(Material& material_in) : material(material_in) {}

	// Any classes extending WorldObject are visible to Ray collisions
	virtual bool hit(const Ray& ray, double t_min, double t_max, CollisionPoint& point) const = 0;

public:
	Material& material;
};

/*
// A group of WorldObjects that can be hit
class WorldGroup : public WorldObject {
public:
	WorldGroup () {}
	WorldGroup (WorldObject *obj) { add(obj); }

	void clear() { objects.clear(); }
	void add(WorldObject *obj) { objects.push_back(obj); }

	virtual bool hit (const Ray& ray, double t_min, double t_max, CollisionPoint& point) const;

	vector<WorldObject*> objects;
};

// Iterate over all objects in the world group and return the closest one hit by the given ray
// This is how render main loop operates on its objects array
bool WorldGroup::hit (const Ray& ray, double t_min, double t_max, CollisionPoint& point) const {
	CollisionPoint closest_point;
	bool hit_something = false;

	// @TODO: Add the rest of this if necessary
	// See how main loop operates on the WorldObjects *objects[] array

	return true;
}
*/

#endif
