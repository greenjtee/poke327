#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAP_W 80
#define MAP_H 21

#define MAP_PATH 		'#'
#define MAP_POKEMART 	'M'
#define MAP_POKECENTER 	'C'
#define MAP_BORDER		'%'
#define MAP_CLEARING	'.'
#define MAP_TALL_GRASS	':'
#define MAP_TREE		'^'
#define MAP_BOULDER		'%'
#define MAP_PLAYER		'@'

#define MAX_TRIES				128
#define TALL_GRASS_CHANCE		50
#define MAP_TALL_GRASS_REGIONS	 3
#define MAX_TALL_GRASS_DEPTH	25
#define TREE_CHANCE				10

void gen_path(char map[MAP_H][MAP_W]) {
	int i, j;
	
	unsigned intersectX1 = 0, intersectY1 = 0;
	unsigned intersectX2 = 0, intersectY2 = 0;
	
	// these might be passed into the function at some point
	intersectX1 = rand() % (MAP_W - 4) + 3; // x value of north border exit
	intersectY1 = rand() % (MAP_H - 4) + 3; // y value of west border exit

	intersectX2 = rand() % (MAP_W - 4) + 3; // x value of south border exit
	intersectY2 = rand() % (MAP_H - 4) + 3; // y value of east border exit

	// initialize empty map	
	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			map[i][j] = MAP_CLEARING;
		}
	}

	// create N/S border
	for (i = 0; i < MAP_W; i++) {
		map[0][i] = MAP_BORDER; // N
		map[MAP_H - 1][i] = MAP_BORDER; // S
	}

	// create E/W border
	for (i = 0; i < MAP_H; i++) {
		map[i][0] = MAP_BORDER;
		map[i][MAP_W - 1] = MAP_BORDER;
	}

	// if the two intersections overlap, switch connections	
	if (intersectX1 > intersectX2 && intersectY1 > intersectY2) {
		// create path from north border to intersectX1, intersectY2 
		for (i = 0; i <= intersectY2; i++) {
			map[i][intersectX1] = MAP_PATH;	
		}

		// create path from west border to intersectX2, intersectY1 
		for (i = 0; i <= intersectX2; i++) {
			map[intersectY1][i] = MAP_PATH;	
		}

		// create path from south border to intersectX2, intersectY1 or intersectY2
		for (i = MAP_H-1; i >= intersectY1 || i >= intersectY2; i--) {
			map[i][intersectX2] = MAP_PATH;	
		}

		// create path from east border to intersectX1 or intersectX2, intersectY2 
		for (i = MAP_W-1; i >= intersectX1 || i >= intersectX2; i--) {
			map[intersectY2][i] = MAP_PATH;	
		}

	} else {
		// create path from north border to intersectX1, intersectY1 
		for (i = 0; i <= intersectY1; i++) {
			map[i][intersectX1] = MAP_PATH;	
		}

		// create path from west border to intersectX1, intersectY1 
		for (i = 0; i <= intersectX1 || i <= intersectX2; i++) {
			map[intersectY1][i] = MAP_PATH;	
		}

		// create path from south border to intersectX2, intersectY2 
		for (i = MAP_H-1; i >= intersectY2 || i >= intersectY1; i--) {
			map[i][intersectX2] = MAP_PATH;	
		}

		// create path from east border to intersectX2, intersectY2 
		for (i = MAP_W-1; i >= intersectX2; i--) {
			map[intersectY2][i] = MAP_PATH;	
		}
	}	
}

void gen_pokecenter(char map[MAP_H][MAP_W], unsigned* pcX, unsigned* pcY) {
	unsigned tries = 0;	
	unsigned found = 0;

	// pick a location for the pokemart and pokecenter
	tries = 0;
	unsigned pokecenterX = rand() % MAP_W;
	unsigned pokecenterY = 1;

	while (!found) {
		if (pokecenterX > 0 && pokecenterX < MAP_W-2 && pokecenterY > 0 && pokecenterY < MAP_H - 2 && map[pokecenterY - 1][pokecenterX] == MAP_PATH &&
				map[pokecenterY][pokecenterX] != MAP_PATH && map[pokecenterY][pokecenterX+1] != MAP_PATH && map[pokecenterY+1][pokecenterX] != MAP_PATH && map[pokecenterY+1][pokecenterX+1] != MAP_PATH) {
			map[pokecenterY][pokecenterX] = MAP_POKECENTER;
			map[pokecenterY][pokecenterX + 1] = MAP_POKECENTER;
			map[pokecenterY + 1][pokecenterX] = MAP_POKECENTER;
			map[pokecenterY + 1][pokecenterX + 1] = MAP_POKECENTER;

			found = 1;
		} else {
			if (pokecenterY < MAP_H - 1) {
				pokecenterY++;
			} else {
				pokecenterX = rand() % MAP_W;
				pokecenterY = 1;
			}

			tries++;

			if (tries > MAX_TRIES) {
				printf("Trying to gen pokecenter again\n");
				gen_path(map);
				tries = 0;
			}
		}
	}

	*pcX = pokecenterX;
	*pcY = pokecenterY;
}

void gen_pokemart(char map[MAP_H][MAP_W], unsigned* pmX, unsigned* pmY, unsigned* pcX, unsigned* pcY) {
	unsigned tries = 0, found = 0;
	unsigned pokecenterX = 0, pokecenterY = 0;

	unsigned pokemartY = rand() % MAP_H;
	unsigned pokemartX = 1;

	while (!found) {
		if (pokemartX > 0 && pokemartX < MAP_W-2 && pokemartY > 0 && pokemartY < MAP_H - 2 && map[pokemartY][pokemartX - 1] == MAP_PATH &&
				(abs(pokecenterX - pokemartX) > 2 || abs(pokecenterY - pokemartY) > 2) &&
				map[pokemartY][pokemartX] != MAP_PATH && map[pokemartY][pokemartX+1] != MAP_PATH && map[pokemartY+1][pokemartX] != MAP_PATH && map[pokemartY+1][pokemartX+1] != MAP_PATH) {

			map[pokemartY][pokemartX] = MAP_POKEMART;
			map[pokemartY][pokemartX + 1] = MAP_POKEMART;
			map[pokemartY + 1][pokemartX] = MAP_POKEMART;
			map[pokemartY + 1][pokemartX + 1] = MAP_POKEMART;

			found = 1;
		} else {
			if (pokemartX < MAP_W - 1) {
				pokemartX++;
			} else {
				pokemartY = rand() % MAP_H;
				pokemartX = 1;
			}

			tries++;
			
			if (tries > MAX_TRIES) {
				printf("Trying to gen pokemart again\n");
				gen_path(map);
				gen_pokecenter(map, &pokecenterX, &pokecenterY);
				tries = 0;
			}
		}
	}

	*pmX = pokemartX;
	*pmY = pokemartY;
}

void gen_tall_grass(char map[MAP_H][MAP_W], unsigned x, unsigned y, unsigned last_dir, unsigned depth) {
	if (depth > MAX_TALL_GRASS_DEPTH) {
		return;
	}
	
	if (x < (MAP_W-1) && x > 0 && y < (MAP_H-1) && y > 0) {
		if (map[y][x] == MAP_CLEARING) {
			if ((rand() % 100) < TREE_CHANCE) {
				map[y][x] = MAP_TREE;
			} else {
				map[y][x] = MAP_TALL_GRASS;
			}
		}

		if (last_dir != 0 && (rand() % 100) < TALL_GRASS_CHANCE) {
			gen_tall_grass(map, x-1, y, 0, depth + 1);
		}

		if (last_dir != 1 && (rand() % 100) < TALL_GRASS_CHANCE) {
			gen_tall_grass(map, x+1, y, 1, depth + 1);
		}

		if (last_dir != 2 && (rand() % 100) < TALL_GRASS_CHANCE) {
			gen_tall_grass(map, x, y-1, 2, depth + 1);
		}

		if (last_dir != 3 && (rand() % 100) < TALL_GRASS_CHANCE) {
			gen_tall_grass(map, x, y+1, 3, depth + 1);
		}
	}
}

void initialize_map(char map[MAP_H][MAP_W]) {
	unsigned pokemartX = 0, pokemartY = 0, pokecenterX = 0, pokecenterY = 0;
	unsigned i = 0;

	gen_path(map);	

	gen_pokecenter(map, &pokemartX, &pokemartY);	

	gen_pokemart(map, &pokemartX, &pokemartY, &pokecenterX, &pokecenterY);

	unsigned tall_grass_seedX = 1;
	unsigned tall_grass_seedY = 1;

	for (i = 0; i < MAP_TALL_GRASS_REGIONS; i++) {
		char seed_value = 0;
		while (seed_value != MAP_CLEARING) {
			tall_grass_seedX = (rand() % (MAP_W - 2)) + 1; // select an x on the left half of the screen
			tall_grass_seedY = (rand() % (MAP_H - 2)) + 1; // select an x on the left half of the screen
			seed_value = map[tall_grass_seedY][tall_grass_seedX];
		}	
		
		gen_tall_grass(map, tall_grass_seedX, tall_grass_seedY, 5, 0);
	}

	// printf("pokemart X = %d, pokemart Y = %d, pokecenter X = %d, pokecenter Y = %d\n", pokemartX, pokemartY, pokecenterX, pokecenterY);
	
}

void print_map(char map[MAP_H][MAP_W]) {
	int i, j;

	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			printf("%c", map[i][j]);	
		}
		printf("\n");
	}

	printf("\n");
}

int main(int argc, char* argv[]) {
	// printf("Hello world.\n");

	char map[MAP_H][MAP_W];
	
	srand(time(NULL)); // set the seed for the random number generator

	initialize_map(map);
	print_map(map);

	return 0;
}
