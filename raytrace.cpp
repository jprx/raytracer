#include <iostream>
#include <signal.h>
#include <gtk/gtk.h>

#include "raytrace.h"
#include "vector.h"
#include "sphere.h"
#include "RenderTarget.h"
#include "material.h"
#include "utils.h"

// Application (this needs to be static because of SIGINT)
static GtkApplication *__app__ = NULL;

// Image render target
static RenderTarget *render_target = NULL;

// Largest value allowed for a double
static double Infinity = std::numeric_limits<double>::infinity();

using namespace std;

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
	Material *hit_mat = NULL;

	// Have we hit something in the world?
	bool hit_something = false;

	// Test for collision with all objects in list of world objects
	double closest_hit = Infinity;

	// Recursion depth exceeded, return default diffuse
	if (curdepth > RAY_BOUNCE_DEPTH) {
		return Vector3(0,0,0);
	}

	// Iterate over all objects
	for (WorldObject *obj : objects) {
		if (obj->hit(ray, 0.00001, Infinity, test_point)) {
			hit_something = true;
			if (test_point.t_collision < closest_hit) {
				closest_hit = test_point.t_collision;
				closest_point = test_point;
				hit_mat = &obj->material;
			}
		}
	}

	if (hit_something) {
		// Scatter according to the object's material
		Ray next_ray;
		Vector3 attenuation;
		bool continue_bouncing = false;
		continue_bouncing = (*hit_mat).scatter_ray(ray, closest_point, next_ray, attenuation);

		if (continue_bouncing)
			return attenuation * raytrace(next_ray, objects, curdepth+1);
		else
			return attenuation;
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
	Diffuse diffuse_mat = Diffuse(Vector3(0.5,0.5,0.5));
	Emissive emissive_mat = Emissive(Vector3(1,1,1));
	Metal metal_mat = Metal(Vector3(0.75,0.75,0.75));

	Diffuse rand_mats[4];
	rand_mats[0] = Diffuse(Vector3(1,0,1));
	rand_mats[1] = Diffuse(Vector3(1,1,0));
	rand_mats[2] = Diffuse(Vector3(0,1,1));
	rand_mats[3] = Diffuse(Vector3(1,1,1));

	Emissive rand_matse[4];
	rand_matse[0] = Emissive(Vector3(1,0,1));
	rand_matse[1] = Emissive(Vector3(1,1,0));
	rand_matse[2] = Emissive(Vector3(0,1,1));
	rand_matse[3] = Emissive(Vector3(1,1,1));

	printf ("Generating random non-overlapping spheres...\n");

	// This is hacky, I know, will fix eventually
	// It does look really good though :)
	
	for (uint i = 0; i < 50; i++) {
		bool correct_params = false;
		double randsize = 1.0;
		Vector3 randpos = Vector3(0,0,-1);
		while (!correct_params) {
			randsize = rand_range(0.05,0.075);
			randpos = Vector3(rand_range(-2.35,2.35),-0.5 + randsize,rand_range(-2,0.5));
			double found_it = true;
			for (WorldObject* j : objects) {
				double dist_between_objects = 0.0;
				dist_between_objects += pow(((Sphere*)j)->center.x - randpos.x, 2);
				dist_between_objects += pow(((Sphere*)j)->center.y - randpos.y, 2);
				dist_between_objects += pow(((Sphere*)j)->center.z - randpos.z, 2);
				dist_between_objects = sqrt(dist_between_objects);
				if (dist_between_objects < (((Sphere*)j)->radius) + randsize) found_it = false;
			}
			if (found_it) correct_params = true;
		}
		objects.push_back(new Sphere(randpos, randsize, rand_mats[rand() % 4]));
	}

	for (uint i = 0; i < 50; i++) {
		bool correct_params = false;
		double randsize = 1.0;
		Vector3 randpos = Vector3(0,0,-1);
		while (!correct_params) {
			randsize = rand_range(0.05,0.075);
			randpos = Vector3(rand_range(-2.35,2.35),-0.5 + randsize,rand_range(-2,0.5));
			double found_it = true;
			for (WorldObject* j : objects) {
				double dist_between_objects = 0.0;
				dist_between_objects += pow(((Sphere*)j)->center.x - randpos.x, 2);
				dist_between_objects += pow(((Sphere*)j)->center.y - randpos.y, 2);
				dist_between_objects += pow(((Sphere*)j)->center.z - randpos.z, 2);
				dist_between_objects = sqrt(dist_between_objects);
				if (dist_between_objects < (((Sphere*)j)->radius) + randsize) found_it = false;
			}
			if (found_it) correct_params = true;
		}
		objects.push_back(new Sphere(randpos, randsize, rand_matse[rand() % 4]));
	}

	for (uint i = 0; i < 75; i++) {
		bool correct_params = false;
		double randsize = 1.0;
		Vector3 randpos = Vector3(0,0,-1);
		while (!correct_params) {
			randsize = rand_range(0.05,0.075);
			randpos = Vector3(rand_range(-2.35,2.35),-0.5 + randsize,rand_range(-2,0.5));
			double found_it = true;
			for (WorldObject* j : objects) {
				double dist_between_objects = 0.0;
				dist_between_objects += pow(((Sphere*)j)->center.x - randpos.x, 2);
				dist_between_objects += pow(((Sphere*)j)->center.y - randpos.y, 2);
				dist_between_objects += pow(((Sphere*)j)->center.z - randpos.z, 2);
				dist_between_objects = sqrt(dist_between_objects);
				if (dist_between_objects < (((Sphere*)j)->radius) + randsize) found_it = false;
			}
			if (found_it) correct_params = true;
		}
		objects.push_back(new Sphere(randpos, randsize, metal_mat));
	}

	// Create some world objects:
	objects.push_back(new Sphere(Vector3(0,0,-1), 0.657, metal_mat));
	objects.push_back(new Sphere(Vector3(0,-1000.5, 0), 1000, diffuse_mat));
	//objects.push_back(new Sphere(Vector3(-1.15,-0.35,-1), 0.15, diffuse_mat));
	//objects.push_back(new Sphere(Vector3(1.15,-0.35,-1), 0.15, emissive_mat));

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