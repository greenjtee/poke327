prod: poke327.o heap.o map.o world.o
	gcc -Wall -Werror -g poke327.o map.o heap.o world.o -o poke327

poke327.o: poke327.c heap.c heap.h map.c map.h
	gcc -Wall -g poke327.c -o poke327.o -c

heap.o: heap.c heap.h
	gcc -Wall -g heap.c -o heap.o -c

map.o: map.c map.h heap.c heap.h
	gcc -Wall -g map.c -o map.o -c

world.o: map.c map.h world.c world.h
	gcc -Wall -g world.c -o world.o -c

clean:
	rm -f poke327 && rm -f *.o
