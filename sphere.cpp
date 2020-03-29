#include "sphere.h"

// Sphere ray collision detection
bool Sphere::hit(const Ray& ray, double t_min, double t_max, CollisionPoint& point) const {
	// We only care if the number of solutions > 0 (sphere is hit)
	Vector3 direction = ray.dir; // Could make this unit
	double t_squared_coeff = dot(direction, direction);
	double t_coeff = 2.0 * dot (direction, (ray.pos - center));
	double const_coeff = dot(ray.pos - center, ray.pos - center);

	// To set quadratic equation to 0, we subtract radius squared from const term
	const_coeff -= (radius*radius);

	// Return -1 for no collision, or the closest t value for a collision
	double inside_sqrt = t_coeff * t_coeff - 4 * t_squared_coeff * const_coeff;
	if (inside_sqrt > 0) {
		double root = sqrt(inside_sqrt);
		double twice_a = 2.0 * t_squared_coeff;

		// Solve quadratic, chose -sqrt(b^2 - 4ac)
		double closest_t = ((-1.0 * t_coeff) - root) / (twice_a);
		if (closest_t < t_min || closest_t > t_max) {
			// Closest hit not within bounds; maybe the farthest hit is?
			closest_t = ((-1.0 * t_coeff) + root) / (twice_a);
			if (closest_t < t_min || closest_t > t_max) {
				// Still out of bounds, no luck
				goto BAD_COLLISION;
			}
		}

		// closest_t is a good t value now
		point.pos = ray.at(closest_t);
		point.normal = (point.pos - center) / radius; // Normalized normal vector
		point.t_collision = closest_t;		
		return true;
	}

	// goto Heck if you don't like gotos :D
BAD_COLLISION:
	point.pos = Vector3(0,0,0);
	point.normal = Vector3(0,0,0);
	point.t_collision = -1;
	return false;
}
