#include <sys/types.h>
#include <stdlib.h>

// Largest image memory allocated allowed
#define BYTES_PER_PIXEL 3 // RGB, no A
#define MAX_IMAGE_SIZE ((1024) * (768) * BYTES_PER_PIXEL)

// Struct to handle a color:
typedef struct __attribute__((__packed__)) color_struct_t {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t;

// Handle to a CPU allocated image buffer:
class RenderTarget {
public:
	RenderTarget();
	RenderTarget(uint w_in, uint h_in);
	~RenderTarget();

	// Set a pixel to a color
	bool setpix(uint x, uint y, color_t *c);

	// Get a pixel (returns false if pixel doesn't exist)
	// Modifies c
	bool getpix(uint x, uint y, color_t *c);

	// Test whether x and y are in bounds for this image:
	bool in_bounds(uint x, uint y);

	// Width and height:
	uint w, h;

	// RGB 8-bit data:
	uint8_t *buf;

	// Amount of memory allocated here:
	// (Useful for interfacing with GTK libraries)
	ssize_t bytes_allocated;
};

RenderTarget::RenderTarget() {
	w = 0;
	h = 0;
	buf = NULL;
	bytes_allocated = 0;
}

RenderTarget::RenderTarget(uint w_in, uint h_in) : w(w_in), h(h_in) {
	ssize_t img_mem_size = sizeof(*buf) * BYTES_PER_PIXEL * (this->w * this->h);
	buf = NULL;
	bytes_allocated = 0;
	if (img_mem_size <= MAX_IMAGE_SIZE) {
		buf = (typeof(buf)) malloc(img_mem_size);
		bytes_allocated = img_mem_size;
	}
	else {
		fprintf(stderr, "[Error] Out of Memory!\n");
	}
}

RenderTarget::~RenderTarget () {
	free(buf);
}

// Returns whether a coordinate is in bounds for this image
bool RenderTarget::in_bounds (uint x, uint y) {
	return (x < this->w && y < this->h);
}

// Set a pixel to a color
// Returns true on success, false on failure
bool RenderTarget::setpix(uint x, uint y, color_t *c) {
	if (NULL != this->buf && this->in_bounds(x,y)) {
		this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 0] = c->r;
		this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 1] = c->g;
		this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 2] = c->b;
		return true;
	}
	return false;
}

// Get a pixel (returns false if pixel doesn't exist)
// Modifies c
bool RenderTarget::getpix (uint x, uint y, color_t *c) {
	if (NULL != this->buf && NULL != c && this->in_bounds(x,y)) {
		c->r = this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 0];
		c->g = this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 1];
		c->b = this->buf[(BYTES_PER_PIXEL * (x + this->w * y)) + 2];
		return true;
	}
	return false;
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