#ifndef VECTOR_H
#define VECTOR_H

#include <tgmath.h> // Type generic math

class Vector3 {
public:
	Vector3() : x(0), y(0), z(0) {}
	Vector3(double x_in, double y_in, double z_in) : x(x_in), y(y_in), z(z_in) {}

	double getx () const { return x; }
	double gety () const { return y; }
	double getz () const { return z; }

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
	double length() const { return sqrt(length_squared()); }
	double length_squared() const { return (x*x) + (y*y) + (z*z); }

	// The internal Cartesian dimensions
	double x;
	double y;
	double z;
};

// Misc. other non-member operator overload utilities:
// Mark operands as const in case temporaries are created
// Example: vectorA + (time * vectorB) 
// Will create a temporary that is added to vectorA
inline Vector3 operator+ (const Vector3& u, const Vector3& v) {
	return Vector3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline Vector3 operator- (const Vector3& u, const Vector3& v) {
	return Vector3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline Vector3 operator* (const Vector3& u, const Vector3& v) {
	return Vector3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline Vector3 operator* (double t, const Vector3& u) {
	return Vector3(t * u.x, t * u.y, t * u.z);
}

inline Vector3 operator* (const Vector3& u, double t) {
	return Vector3(t * u.x, t * u.y, t * u.z);
}

inline Vector3 operator/ (const Vector3& u, double t) {
	return Vector3 (u.x/t, u.y/t, u.z/t);
}

inline Vector3 unit (const Vector3& u) {
	return u / u.length();
}

inline double dot (const Vector3& u, const Vector3& v) {
	return u.x * v.x +
		   u.y * v.y +
		   u.z * v.z;
}

class Ray {
public:
	Ray(Vector3& pos_in, Vector3& dir_in) : pos(pos_in), dir(dir_in) {}

	Vector3 at (double t) const {
		return pos + (t*dir);
	}

	Vector3 &pos;
	Vector3 &dir;
};

#endif
