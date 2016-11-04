raytrace.exe : raytrace.o parser.o ppmrw.o
	gcc raytrace.o parser.o ppmrw.o -o raytrace.exe

raytrace.o : raytrace.c
	gcc -c raytrace.c

parser.o : parser.c
	gcc -c parser.c

ppmrw.o : ppmrw.c
	gcc -c ppmrw.c
