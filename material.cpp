#include "material.h"
#include "utils.h" // For utilities

// Bounce diffusively, moving a random direction from the normal of the collision point
bool Diffuse::scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out) {
	// Generate random bounce direction
	Vector3 random_dir = Vector3(rand_range(-1.0,1.0), rand_range(-1.0,1.0), rand_range(-1.0,1.0));
	random_dir /= random_dir.length();

	// Follow normal and then move a random direction from the normal vector tip:
	ray_out = Ray(point.pos, point.normal + random_dir);

	// Diffuse objects attenuate by color:
	attenuation_out = this->color;
	//ray_out = next_ray;

	return true;
}

// We are a light source, no further bouncing needed!
bool Emissive::scatter_ray(const Ray& ray_in, CollisionPoint point, Ray& ray_out, Vector3& attenuation_out) {
	attenuation_out = this->color;
	return false;
}
