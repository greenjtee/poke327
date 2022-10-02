all: poke327

prod: poke327.c heap.c heap.h map.c map.h world.c world.h trainer.c trainer.h utility.h
	gcc -Wall -Werror -c trainer.c -o trainer.o
	gcc -Wall -Werror -c map.c -o map.o
	gcc -Wall -Werror -c heap.c -o heap.o
	gcc -Wall -Werror -c world.c -o world.o
	gcc -Wall -Werror -c poke327.c -o poke327.o
	gcc -Wall -Werror poke327.o map.o heap.o world.o trainer.o -o poke327

poke327: poke327.o heap.o map.o world.o trainer.o
	gcc -Wall -Werror -g poke327.o map.o heap.o world.o trainer.o -o poke327

poke327.o: poke327.c heap.h map.h
	gcc -Wall -g poke327.c -o poke327.o -c

heap.o: heap.c heap.h
	gcc -Wall -g heap.c -o heap.o -c

map.o: map.c map.h heap.h trainer.h
	gcc -Wall -g map.c -o map.o -c

world.o: map.c map.h world.c world.h
	gcc -Wall -g world.c -o world.o -c

trainer.o: trainer.c trainer.h map.h
	gcc -Wall -g trainer.c -o trainer.o -c

clean:
	rm -f poke327 && rm -f *.o
