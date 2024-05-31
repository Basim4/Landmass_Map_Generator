# Landmass Generator Makefile 

landmass_map.exe: landmass_generator.o
	g++ -g -Wall landmass_generator.o termcolor.hpp -o landmass_map.exe

landmass_generator.o: landmass_generator.cpp
	g++ -g -Wall -c landmass_generator.cpp

clean:
	rm landmass_map.exe *.o
