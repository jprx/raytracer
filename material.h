#ifndef MATERIAL_H
#define MATERIAL_H

#include "vector.h"
#include "CollisionPoint.h"

// A material just defines how rays get scattered, and
// how much they are attenuated per bounce
class Material {
public:
	// Returns true if we should continue bouncing, false if we should stop bouncing:
	// Writes the scattered ray into out_ray
	// Writes attenuation color into attenuation_out
	virtual bool scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out) = 0;
};

class Diffuse : public Material {
public:
	Diffuse() : color(Vector3(1,1,1)) {}
	Diffuse(const Vector3& color_in) : color(color_in) {}

	bool scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out);

	// The 'attenuation' parameter
	// Each reflected bounce is attenuated by this color paramter
	Vector3 color;
};

class Emissive : public Material {
public:
	Emissive() : color(Vector3(1,1,1)) {}
	Emissive(const Vector3& color_in) : color(color_in) {}

	bool scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out);

	// The 'attenuation' parameter
	// Each reflected bounce is attenuated by this color paramter
	Vector3 color;
};

class Metal : public Material {
public:
	Metal() : color(Vector3(1,1,1)) {}
	Metal(const Vector3& color_in) : color(color_in) {}

	bool scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out);

	Vector3 color;
};

#endif
