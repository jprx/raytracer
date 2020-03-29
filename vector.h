#ifndef VECTOR_H
#define VECTOR_H

#include <tgmath.h> // Type generic math

class Vector3 {
public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(double x_in, double y_in, double z_in) : x(x_in), y(y_in), z(z_in) {}

	double getx () { return x; }
	double gety () { return y; }
	double getz () { return z; }

	// Negation operator
	Vector3 operator- () { return Vector3(-x, -y, -z); }

	// Indexing (because why not?)
	double& operator[](int idx) {
		switch (idx) {
			case 0: return this->x;
			case 1: return this->y;
			case 2: return this->z;
		}
	}

	// Addition
	Vector3& operator+=(const Vector3& other) {
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;
		return *this;
	}

	// Multiplication
	Vector3& operator*=(double t) {
		this->x *= t;
		this->y *= t;
		this->z *= t;
		return *this;
	}

	// Division
	Vector3& operator/=(double t) {
		return *this *= 1/t;
	}

	// Length
	double length() { return sqrt(length_squared()); }
	double length_squared() { return (x*x) + (y*y) + (z*z); }

	// The internal Cartesian dimensions
	double x;
	double y;
	double z;
};

// Misc. other non-member operator overload utilities:
inline Vector3 operator+ (Vector3& u, Vector3& v) {
	return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vector3 operator- (Vector3& u, Vector3& v) {
	return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vector3 operator* (Vector3& u, Vector3& v) {
	return Vector3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vector3 operator* (double t, Vector3& u) {
	return Vector3(t * u.x, t * u.y, t * u.z);
}

inline Vector3 operator* (Vector3& u, double t) {
	return Vector3(t * u.x, t * u.y, t * u.z);
}

inline Vector3 operator/ (Vector3& u, double t) {
	return Vector3 (u.x/t, u.y/t, u.z/t);
}

inline Vector3 unit (Vector3& u) {
	return u / u.length();
}

class Ray {

};

#endif