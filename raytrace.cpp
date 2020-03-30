#include <iostream>
#include <signal.h>
#include <gtk/gtk.h>
#include <limits>
#include <cstdlib>

#include "raytrace.h"
#include "vector.h"
#include "sphere.h"

// Aspect dimensions and number of pixels per "dimension"
#define ASPECT_X ((3))
#define ASPECT_Y ((2))
#define RESOLUTION_SCALER ((300))

// Image pixel dimensions
#define DIM_X ((RESOLUTION_SCALER * ASPECT_X))
#define DIM_Y ((RESOLUTION_SCALER * ASPECT_Y))

// Number of samples per pixel (for anti-aliasing)
#define NUM_SAMPLES 100

// How deep can rays bounce? (Number of bounces before terminating)
#define RAY_BOUNCE_DEPTH 100

// How many characters wide is the progress bar?
#define PROGRESS_BAR_WIDTH 60

using namespace std;

// Application (this needs to be static because of SIGINT)
static GtkApplication *__app__ = NULL;

// Image render target
static RenderTarget *render_target = NULL;

// Largest value allowed for a double
static double Infinity = std::numeric_limits<double>::infinity();

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

// Lerp- Linear interpolate
// Requires T to implement * and + operators
// Assumes t only between 0.0 and 1.0
template <typename T>
T Lerp (T a, T b, double t) {
	return ((1.0 - t) * a) + (t * b);
}

// Returns the sky color for a given ray
Vector3 get_sky_color (const Ray& r) {
	// Interpolate down the Y component of the ray
	// If the ray is pointing to (0,1,0) then do blue
	// If the ray is poitning to (0,-1,0) then do whiteish
	// By using a unit vector, we get a beautiful falloff near the bottom
	// As the vector magnitude changes for X, so equivalent Y values follow a nice subtle curve
	Vector3 ray_unit = unit(r.dir);
	double y_dist_from_bottom = (0.5 * ray_unit.y) + 0.5;
	return Lerp(Vector3(1.0,1.0,1.0), Vector3(0.25, (166.0/255), (254.0/255)), y_dist_from_bottom);
}

// Returns a random double [0.0,1.0)
double rand_double() {
	// Adding the 1.0 prevents this from ever returning RAND_MAX
	return rand() / (RAND_MAX + 1.0);
}

double rand_range(double min, double max) {
	return Lerp(min, max, rand_double());
}

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
double sphere_collision (const Vector3& center, double radius, const Ray& ray) {
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
		// Solve quadratic, chose -sqrt(b^2 - 4ac)
		return ((-1.0 * t_coeff) - sqrt(inside_sqrt)) / (2.0 * t_squared_coeff);
	}

	return -1.0;
}

/***************
 * raytrace
 *
 * Recursively traces a ray through the provided set
 * of WorldObjects. Terminates when the maxmimum
 * recursion depth is exceeded, or when the ray
 * hits the sky.
 *
 * Inputs: ray - the ray to test
 *         objects - the array of objects to test
 *         curdepth - the current recursion depth
 * Outputs: the color (as Vector3) this ray ended up at
 * Side Effects: None
 ***************/
Vector3 raytrace (const Ray& ray, const std::vector<WorldObject*> objects, uint curdepth) {
	CollisionPoint test_point, closest_point;

	// Have we hit something in the world?
	bool hit_something = false;
	bool hit_light = false;

	// Test for collision with all objects in list of world objects
	double closest_hit = Infinity;

	// Recursion depth exceeded, return default diffuse
	if (curdepth > RAY_BOUNCE_DEPTH) {
		return Vector3(0,0,0);
	}

	// Iterate over all objects
	int i = 0;
	for (WorldObject *obj : objects) {
		if (obj->hit(ray, 0.00001, Infinity, test_point)) {
			hit_something = true;
			if (test_point.t_collision < closest_hit) {
				closest_hit = test_point.t_collision;
				closest_point = test_point;
				//if (i == 1) { hit_light = true; }
			}
		}
		i++;
	}

	if (hit_something) {
		if (hit_light) { return Vector3(1,1,1); }
		// Bounce & find new direction
		Vector3 random_dir = Vector3(rand_range(-1.0,1.0), rand_range(-1.0,1.0), rand_range(-1.0,1.0));
		random_dir /= random_dir.length();
		Ray next_ray = Ray(closest_point.pos, closest_point.normal + random_dir);
		return 0.5 * raytrace(next_ray, objects, curdepth+1);
	}
	else {
		// No collision, draw sky
		return get_sky_color(ray);
	}
}

/***************
 * render
 *
 * Renders into a given RenderTarget (img)
 * Inputs: img - the RenderTarget to render to
 * Outputs: true if successful, false otherwise
 * Side Effects: Changes RenderTarget's buf parameter
 ***************/
bool render(RenderTarget& img) {
	uint x, y;

	// We are using a left-handed coord system
	// (RH coord system but with -z pointing away from camera)
	// Camera position (0,0,0) looking towards (0,0,-1)
	Vector3 camera_pos = Vector3(0.0,0.0,0.0);
	// Vector3 up_dir = Vector3(0.0,1.0,0.0);
	// Vector3 right_dir = Vector3(1.0,0.0,0.0);
	// Vector3 forward_dir = Vector3(0.0,0.0,-1.0);

	// World Objects:
	std::vector<WorldObject*> objects;
	objects.push_back(new Sphere(Vector3(0,0,-1), 0.5));
	objects.push_back(new Sphere(Vector3(0,-100.5, 0), 100));

	// Iterate over every pixel
	printf("Raytracing!\n");
	for (y = 0; y < img.h; y++) {
		// Print progress bar:
		if (y % 10 == 0) {
			double progress = (y*100)/img.h;

			printf("[");
			for (uint pbar = 0; pbar < PROGRESS_BAR_WIDTH; pbar++) {
				if (progress > (pbar*100)/PROGRESS_BAR_WIDTH) printf ("=");
				else printf(" ");
			}
			printf("]");
			std::cout << " " << progress << "%\r";
			std::cout.flush();
		}

		for (x = 0; x < img.w; x++) {
			Vector3 pixel_color = Vector3(0,0,0);

			// Trace out vectors that form a square from -1 to 1 on both dimensions
			for (uint sample = 0; sample < NUM_SAMPLES; sample++) {
				Vector3 pointer = Vector3(ASPECT_X * (2.0 * ((x*1.0+rand_range(-1,1))/img.w) - 1.0), ASPECT_Y * (2.0 * ((img.h-y+rand_range(-1,1))*1.0/img.h) - 1.0), -1.0);
				Ray r = Ray(camera_pos, pointer);
				
				pixel_color += raytrace(r, objects, 0);
			}

			pixel_color /= NUM_SAMPLES;
			img.setpix(x,y,pixel_color);
		}
	}

	// Display done message!
	printf("[");
	for (uint pbar = 0; pbar < PROGRESS_BAR_WIDTH; pbar++) {
		printf ("=");
	}
	printf("]");
	std::cout << " " << 100 << "%\r\n" << "Done!\n";
	std::cout.flush();

	// Convert internal framebuffer to GTK-friendly version
	return img.RenderGTK();
}

// Initial callback upon application creation:
void myapp_activate(GtkApplication *app, gpointer user_data) {
	GtkWidget *window = NULL;

	// Render:
	render(*render_target);

	// Initialize a GdkPixbuf with the GBytes buffer:
	GdkPixbuf *imgpixbuf = gdk_pixbuf_new_from_data(
							(guchar*)render_target->gtkbuf,
							GDK_COLORSPACE_RGB,
							false, // No Alpha
							8, // 8 bits per sample
							render_target->w,
							render_target->h,
							BYTES_PER_PIXEL * render_target->w, // Row length
							NULL,
							0
						);

	// Initialize a GtkImage
	GtkWidget *image_widget;
	image_widget = gtk_image_new_from_pixbuf(imgpixbuf);

	// Create window
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW(window), "Raytracer Boi");
	gtk_window_set_default_size (GTK_WINDOW(window), DIM_X, DIM_Y);

	// Attach image to window
	gtk_container_add(GTK_CONTAINER (window), image_widget);

	// Show window
	gtk_widget_show_all(window);
}

// Launch GTK App
int main (int argc, char **argv) {
	int app_status = 0;

	// SIGINT handler (just in case ;D):
	signal(SIGINT, sigint_handler);

	srand(time(NULL));

	// Create image buffer:
	render_target = new RenderTarget(DIM_X, DIM_Y);

	// Allocate GTK app:
	__app__ = gtk_application_new("org.jprx.cpu_raytracer", G_APPLICATION_FLAGS_NONE);

	// Setup activation handler (app 'main' loop):
	g_signal_connect(__app__, "activate", G_CALLBACK(myapp_activate), NULL);

	// Run app and store return code:
	app_status = g_application_run(G_APPLICATION (__app__), argc, argv);

	// Cleanup
	sigint_handler(-1);
	return 0;
}

// Cleanup and close down GTK app
void sigint_handler(int signum) {
	printf("\nGoodbye!\n");
	delete render_target;
	g_object_unref(__app__);
	exit(0);
}