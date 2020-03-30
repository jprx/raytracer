#ifndef COLLISION_POINT_H
#define COLLISION_POINT_H

// All information associated with a given ray collision
class CollisionPoint {
public:
	Vector3 pos;
	Vector3 normal;
	double t_collision; // t value associated with ray to create collision
};

#endif
