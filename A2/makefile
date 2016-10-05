raycast.exe : raycast.o parser.o ppmrw.o
	gcc raycast.o parser.o ppmrw.o -o raycast.exe

raycast.o : raycast.c
	gcc -c raycast.c

parser.o : parser.c
	gcc -c parser.c

ppmrw.o : ppmrw.c
	gcc -c ppmrw.c
