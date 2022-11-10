all: poke327

prod: poke327.cpp heap.c heap.h map.cpp map.hpp world.cpp world.hpp trainer.cpp trainer.hpp pokedex.cpp pokedex.hpp pokemon.cpp pokemon.hpp
	g++ -Wall -Werror -c trainer.cpp -o trainer.o
	g++ -Wall -Werror -c map.cpp -o map.o
	gcc -Wall -Werror -c heap.c -o heap.o
	g++ -Wall -Werror -c world.cpp -o world.o
	g++ -Wall -Werror -c poke327.cpp -o poke327.o
	g++ -Wall -Werror -c player.cpp -o player.o -c
	g++ -Wall -Werror -c pokedex.cpp -o pokedex.o -c
	g++ -Wall -Werror -c pokemon.cpp -o pokemon.o -c
	g++ -Wall -Werror poke327.o map.o heap.o world.o trainer.o player.o pokedex.o pokemon.o -o poke327 -lncurses

poke327: poke327.o heap.o map.o world.o trainer.o player.o pokedex.o pokemon.o
	g++ -Wall -g poke327.o heap.o map.o world.o trainer.o player.o pokedex.o pokemon.o -o poke327 -lncurses

poke327.o: poke327.cpp heap.h map.hpp
	g++ -Wall -g poke327.cpp -o poke327.o -c

heap.o: heap.c heap.h
	gcc -Wall -g heap.c -o heap.o -c

map.o: map.cpp map.hpp heap.h trainer.hpp utility.hpp player.hpp world.hpp
	g++ -Wall -g map.cpp -o map.o -c

world.o: world.cpp world.hpp
	g++ -Wall -g world.cpp -o world.o -c

trainer.o: trainer.cpp trainer.hpp map.hpp world.hpp
	g++ -Wall -g trainer.cpp -o trainer.o -c

player.o: player.cpp player.hpp map.hpp world.hpp
	g++ -Wall -g player.cpp -o player.o -c

pokedex.o: pokedex.cpp pokedex.hpp
	g++ -Wall -g pokedex.cpp -o pokedex.o -c

pokemon.o: pokemon.cpp pokemon.hpp
	g++ -Wall -g pokemon.cpp -o pokemon.o -c

clean:
	rm -f poke327 && rm -f *.o
