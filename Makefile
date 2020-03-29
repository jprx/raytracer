
raytrace : raytrace.cpp raytrace.h
	g++ -I /usr/include/eigen3/ `pkg-config --cflags gtk+-3.0` raytrace.cpp -o raytrace `pkg-config --libs gtk+-3.0`

clean : 
	rm raytrace