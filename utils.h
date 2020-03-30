#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>

// Utility functions:
// Lerp- Linear interpolate
// Requires T to implement * and + operators
// Assumes t only between 0.0 and 1.0
template <typename T>
inline T Lerp (T a, T b, double t) {
	return ((1.0 - t) * a) + (t * b);
}

// Returns a random double [0.0,1.0)
inline double rand_double() {
	// Adding the 1.0 prevents this from ever returning RAND_MAX
	return rand() / (RAND_MAX + 1.0);
}

inline double rand_range(double min, double max) {
	return Lerp(min, max, rand_double());
}

#endif