
raytrace : raytrace.cpp raytrace.h vector.o sphere.o
	g++ `pkg-config --cflags gtk+-3.0` vector.o sphere.o raytrace.cpp -o raytrace `pkg-config --libs gtk+-3.0`

vector.o : vector.cpp vector.h
	g++ vector.cpp -c `pkg-config --libs gtk+-3.0`

sphere.o : sphere.cpp sphere.h
	g++ sphere.cpp -c

clean : 
	rm -f raytrace vector.o