#ifndef SPHERE_H
#define SPHERE_H

#include "worldObject.h"
#include "material.h"

class Sphere : public WorldObject {
public:
	Sphere(const Vector3 center_in, double radius_in, Material& material_in) : WorldObject(material_in), center(center_in), radius(radius_in) {}

	// Extend WorldObject hit method:
	virtual bool hit(const Ray& ray, double t_min, double t_max, CollisionPoint& point) const;

	Vector3 center;
	double radius;
};

#endif
