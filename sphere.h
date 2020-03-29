#ifndef SPHERE_H
#define SPHERE_H

#include "worldObject.h"

class Sphere : public WorldObject {
public:
	Sphere() {}
	Sphere(const Vector3 center_in, double radius_in) : center(center_in), radius(radius_in) {}

	// Extend WorldObject hit method:
	virtual bool hit(const Ray& ray, double t_min, double t_max, CollisionPoint& point) const;

	Vector3 center;
	double radius;
};

#endif
