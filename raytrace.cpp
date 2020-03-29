#include <iostream>
#include <signal.h>
#include <gtk/gtk.h>

#include "raytrace.h"
#include "vector.h"

using namespace std;

// Application (this needs to be static because of SIGINT)
static GtkApplication *__app__ = NULL;

// Image render target
static RenderTarget *render_target = NULL;

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
	return Lerp(Vector3(1.0,0.8,1.0), Vector3(0.25, (166.0/255), (254.0/255)), 1.0 - y_dist_from_bottom);
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

	// Iterate over every pixel
	for (y = 0; y < img.h; y++) {
		for (x = 0; x < img.w; x++) {
			// Trace out vectors that form a square from -1 to 1 on both dimensions
			Vector3 pointer = Vector3(2.0 * (x*1.0/img.w) - 1.0, 2.0 * (y*1.0/img.h) - 1.0, -1.0);
			Ray r = Ray(camera_pos, pointer);
			img.setpix(x,y,get_sky_color(r));
		}
		printf("[%d]: %f\n", y, (y*1.0/img.h));
	}

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
	gtk_window_set_default_size (GTK_WINDOW(window), 640, 480);

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

	// Create image buffer:
	render_target = new RenderTarget(640, 480);

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
	printf("Goodbye!\n");
	delete render_target;
	g_object_unref(__app__);
	exit(0);
}