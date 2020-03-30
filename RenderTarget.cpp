#include "RenderTarget.h"

RenderTarget::RenderTarget() {
	w = 0;
	h = 0;
	gtkbuf = NULL;
	dbuf = NULL;
}

RenderTarget::RenderTarget(uint w_in, uint h_in) : w(w_in), h(h_in) {
	ssize_t img_mem_size = BYTES_PER_PIXEL * (this->w * this->h);
	gtkbuf = NULL;
	dbuf = NULL;
	if (img_mem_size <= MAX_IMAGE_SIZE) {
		gtkbuf = (typeof(gtkbuf)) malloc(img_mem_size * sizeof(*gtkbuf));
		if (NULL == gtkbuf) { fprintf(stderr, MEM_ERR_MSG); }

		dbuf = (typeof(dbuf)) malloc(img_mem_size * sizeof(*dbuf));
		if (NULL == dbuf) { fprintf(stderr, MEM_ERR_MSG); if (NULL != gtkbuf) { free(gtkbuf); }}
	}
	else {
		fprintf(stderr, MEM_ERR_MSG);
	}
}

RenderTarget::~RenderTarget () {
	free(dbuf);
	free(gtkbuf);
}

// Returns whether a coordinate is in bounds for this image
bool RenderTarget::in_bounds (uint x, uint y) {
	return (x < this->w && y < this->h);
}

// Set a pixel to a color
// Returns true on success, false on failure
bool RenderTarget::setpix(uint x, uint y, color_t *c) {
	if (NULL != this->dbuf && this->in_bounds(x,y)) {
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 0)] = c->r;
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 1)] = c->g;
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 2)] = c->b;
		return true;
	}
	return false;
}

// Set a pixel to a color using vector's XYZ as RGB channels
// Returns true on success, false on failure
bool RenderTarget::setpix(uint x, uint y, const Vector3& v) {
	if (NULL != this->dbuf && this->in_bounds(x,y)) {
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 0)] = v.x;
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 1)] = v.y;
		this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 2)] = v.z;
		return true;
	}
	return false;
}

// Get a pixel (returns false if pixel doesn't exist)
// Modifies c
bool RenderTarget::getpix (uint x, uint y, color_t *c) {
	if (NULL != this->dbuf && NULL != c && this->in_bounds(x,y)) {
		c->r = this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 0)];
		c->g = this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 1)];
		c->b = this->dbuf[((BYTES_PER_PIXEL * (x + this->w * y)) + 2)];
		return true;
	}
	return false;
}

// Maps buf -> dbuf so this RenderTarget can be displayed in a GTKWidget
bool RenderTarget::RenderGTK(void) {
	uint x, y;
	color_t c;
	uint8_t r, g, b;

	if (NULL != this->gtkbuf && NULL != this->dbuf) {
		for (x = 0; x < this->w; x++) {
			for (y = 0; y < this->h; y++) {
				getpix(x,y,&c);
				r = (uint8_t)(255 * c.r);
				g = (uint8_t)(255 * c.g);
				b = (uint8_t)(255 * c.b);
				setgtkpix(x,y,r,g,b);
			}
		}
		return true;
	}
	return false;
}

// Set a GTK pixel to a color
// Returns true on success, false on failure
bool RenderTarget::setgtkpix(uint x, uint y, uint8_t r, uint8_t g, uint8_t b) {
	if (NULL != this->gtkbuf && this->in_bounds(x,y)) {
		this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 0)] = r;
		this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 1)] = g;
		this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 2)] = b;
		return true;
	}
	return false;
}

// Get a GTK pixel (returns false if pixel doesn't exist)
// Modifies c
bool RenderTarget::getgtkpix (uint x, uint y, uint8_t *r, uint8_t *g, uint8_t *b) {
	if (NULL != this->gtkbuf && this->in_bounds(x,y)) {
		*r = this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 0)];
		*g = this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 1)];
		*b = this->gtkbuf[sizeof(*this->gtkbuf) * ((BYTES_PER_PIXEL * (x + this->w * y)) + 2)];
		return true;
	}
	return false;
}
