#ifndef WORLD_OBJECT_H
#define WORLD_OBJECT_H

#include "vector.h"

// All information associated with a given ray collision
class CollisionPoint {
public:
	Vector3 pos;
	Vector3 normal;
	double t_collision; // t value associated with ray to create collision
};

// A WorldObject is just something that a ray can hit!
class WorldObject {
public:
	// Any classes extending WorldObject are visible to Ray collisions
	virtual bool hit(const Ray& ray, double t_min, double t_max, CollisionPoint& point) const = 0;
};

#endif
