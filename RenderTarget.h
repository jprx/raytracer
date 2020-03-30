#ifndef RENDER_TARGET_H
#define RENDER_TARGET_H

#include <sys/types.h>
#include <stdlib.h>
#include <limits>
#include <cstdlib>

#include "vector.h"

// Largest image memory allocated allowed
#define BYTES_PER_PIXEL 3 // RGB, no A
#define MAX_IMAGE_SIZE ((1024) * (768) * BYTES_PER_PIXEL)

#define MEM_ERR_MSG (("[Error] Out of Memory!\n"))

// Struct to handle a color:
typedef struct __attribute__((__packed__)) color_struct_t {
	double r;
	double g;
	double b;
} color_t;

// Handle to a CPU allocated image buffer:
class RenderTarget {
public:
	RenderTarget();
	RenderTarget(uint w_in, uint h_in);
	~RenderTarget();

	// Set a pixel to a color
	bool setpix(uint x, uint y, color_t *c);

	// Set a pixel to a color, using vector XYZ as RGB channels
	bool setpix(uint x, uint y, const Vector3 &v);

	// Get a pixel (returns false if pixel doesn't exist)
	// Modifies c
	bool getpix(uint x, uint y, color_t *c);

	// Test whether x and y are in bounds for this image:
	bool in_bounds(uint x, uint y);

	// Converts dbuf into buf:
	// Gets the format of this RenderTarget ready for GTK
	// Returns true on success, false on failure
	bool RenderGTK(void);

	// Width and height:
	uint w, h;

	/**********
	 * Buffers
	 * (Yes, both are allocated at object creation)
	 * Deal with it
	 **********/

	// RGB 8-bit data:
	uint8_t *gtkbuf;

	// RGB double data (laid out same way as buf, but this uses doubles instead)
	// Internal calculations use this before mapping down to 8bit RGB for GTK rendering
	// Can use this for higher-depth PNG output in the future
	double *dbuf;

private:
	// Setter & getter methods for the GTK buffer (gtkbuf)
	bool setgtkpix(uint x, uint y, uint8_t r, uint8_t g, uint8_t b);
	bool getgtkpix(uint x, uint y, uint8_t *r, uint8_t *g, uint8_t *b);
};

#endif
