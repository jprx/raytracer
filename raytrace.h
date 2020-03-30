#ifndef RAYTRACE_H
#define RAYTRACE_H

#include <sys/types.h>
#include <stdlib.h>
#include <limits>

#include "vector.h"
#include "RenderTarget.h"

// Aspect dimensions and number of pixels per "dimension"
#define ASPECT_X ((3))
#define ASPECT_Y ((2))
#define RESOLUTION_SCALER ((200))

// Image pixel dimensions
#define DIM_X ((RESOLUTION_SCALER * ASPECT_X))
#define DIM_Y ((RESOLUTION_SCALER * ASPECT_Y))

// Number of samples per pixel (for anti-aliasing)
#define NUM_SAMPLES 100

// How deep can rays bounce? (Number of bounces before terminating)
#define RAY_BOUNCE_DEPTH 100

// How many characters wide is the progress bar?
#define PROGRESS_BAR_WIDTH 60

// Utility functions:
// Render a quick testpattern to ensure everything is working
bool render_testpattern(RenderTarget& img) {
	uint x, y;
	color_t c;

	for (y = 0; y < img.h; y++) {
		for (x = 0; x < img.w; x++) {
			c.r = (x*1.0f)/(1.0f*img.w);
			c.g = (y*1.0f)/(1.0f*img.h);
			c.b = 1.0f;
			img.setpix(x,y,&c);
		}
	}

	// Convert internal framebuffer to GTK-friendly version
	return img.RenderGTK();
}

// raytrace.cpp methods:
void sigint_handler(int signum);
void myapp_activate(GtkApplication *app, gpointer user_data);

/***************
 * render
 *
 * Renders into a given RenderTarget (img)
 * Inputs: img - the RenderTarget to render to
 * Outputs: true if successful, false otherwise
 * Side Effects: Changes RenderTarget's buf parameter
 ***************/
bool render(RenderTarget& img);

/**************************************
 *
 * sphere_collision
 *
 * Returns what scalar multiple of direction added
 * to a Ray's origin results in an intersection with 
 * the given sphere.
 *
 * See my math writeup for a derivation of this formula
 *
 * Returns -1.0 for no collision, or the closest t value
 * if a collision is detected.
 *
 * Inputs: 
 * Vector3 center: center of the sphere
 * double radius: radius of the sphere
 * Ray ray: the ray to cast
 *
 * Side Effects: None
 *
 * To find intersections, we need to solve the vector algebra equation in t:
 *
 * t^2 (b * b) + 2t b * (a-c) + (a-c) * (a-c) = radius^2
 *
 * a is the origin of our ray, b is its direction vector, and c is 
 * the center of the sphere.
 *
 * 0 solution = no collision
 * 1 solution = just touch edge of sphere
 * 2 solutions = pass through sphere and collide in two places
 *
 **************************************/
double sphere_collision (const Vector3& center, double radius, const Ray& ray);

bool render_testpattern(RenderTarget& img);

#endif