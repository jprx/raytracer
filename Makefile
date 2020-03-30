
raytrace : raytrace.cpp raytrace.h vector.o sphere.o RenderTarget.o
	g++ `pkg-config --cflags gtk+-3.0` RenderTarget.o vector.o sphere.o raytrace.cpp -o raytrace `pkg-config --libs gtk+-3.0`

vector.o : vector.cpp vector.h
	g++ vector.cpp -c `pkg-config --libs gtk+-3.0`

sphere.o : sphere.cpp sphere.h
	g++ sphere.cpp -c

RenderTarget.o : RenderTarget.cpp RenderTarget.h
	g++ RenderTarget.cpp -c

clean : 
	rm -f raytrace vector.o sphere.o RenderTarget.o