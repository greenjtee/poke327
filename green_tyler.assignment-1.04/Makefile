prod: poke327.o heap.o map.o world.o trainer.o
	gcc -Wall -Werror -g poke327.o map.o heap.o world.o trainer.o -o poke327

poke327.o: poke327.c heap.h map.h
	gcc -Wall -Werror -g poke327.c -o poke327.o -c

heap.o: heap.c heap.h
	gcc -Wall -Werror -g heap.c -o heap.o -c

map.o: map.c map.h heap.h trainer.h
	gcc -Wall -Werror -g map.c -o map.o -c

world.o: map.c map.h world.c world.h
	gcc -Wall -Werror -g world.c -o world.o -c

trainer.o: trainer.c trainer.h map.h
	gcc -Wall -Werror -g trainer.c -o trainer.o -c

clean:
	rm -f poke327 && rm -f *.o
