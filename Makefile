
raytrace : raytrace.cpp raytrace.h vector.o
	g++ `pkg-config --cflags gtk+-3.0` vector.o raytrace.cpp -o raytrace `pkg-config --libs gtk+-3.0`

vector.o : vector.cpp vector.h
	g++ vector.cpp -c `pkg-config --libs gtk+-3.0`

clean : 
	rm -f raytrace vector.o