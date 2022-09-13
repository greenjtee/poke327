#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#define WORLD_W 401
#define WORLD_H 401

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

typedef struct map_t {
	char values[MAP_H][MAP_W];
	uint8_t northExitX;
	uint8_t eastExitY;
	uint8_t southExitX;
	uint8_t westExitY;

	uint8_t pokecenter[2];
	uint8_t pokemart[2];
} map_t;

// void get_map_from_world(map_t ** world, int16_t x, int16_t y, map_t *map);
int8_t check_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map);
int8_t get_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map);
void gen_path(map_t **map);
void gen_pokecenter(map_t **map);
void gen_pokemart(map_t **map);
void gen_tall_grass(map_t **map, uint8_t x, uint8_t y, uint8_t last_dir, uint8_t depth);
void initialize_map(map_t **map);
void print_map(map_t **map);

void free_world(map_t *world[WORLD_H][WORLD_W]) {
	uint16_t i, j;

	for (i = 0; i < WORLD_H; i++) {
		for (j = 0; j < WORLD_W; j++) {
			if (world[i][j]) {
				// printf("freeing i=%d j=%d\n", i, j);
				free(world[i][j]);
			}
		}
	}
}

int8_t check_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map) {
	if (x < -200 || x > 200 || y < -200 || y > 200) {
		return -1;
	}
	
	if (!(world[y+200][x+200])) {
		return 1;
	}

	*map = world[y+200][x+200];

	return 0;
}

int8_t get_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map) {
	map_t *northMap = NULL, *eastMap = NULL, *southMap = NULL, *westMap = NULL;
	int8_t n = 0, e = 0, s = 0, w = 0;
	int8_t m = 0;

	m = check_map(world, x, y, map);

	if (m == 0) {
		return 0;
	} else if (m == 1) {
		*map = (map_t*)malloc(sizeof(map_t));
		world[y+200][x+200] = *map;
	}

	n = check_map(world, x, y-1, &northMap);
	e = check_map(world, x+1, y, &eastMap);
	s = check_map(world, x, y+1, &southMap); 
	w = check_map(world, x-1, y, &westMap);

	if (n == 1) {
		(*map)->northExitX = rand() % (MAP_W - 4) + 3;
	} else if (n == 0) {
		(*map)->northExitX = northMap->southExitX;
	}
	
	if (e == 1) {
		(*map)->eastExitY = rand() % (MAP_H - 4) + 3;
	} else if (e == 0) {
		(*map)->eastExitY = eastMap->westExitY;
	}
	
	if (s == 1) {
		(*map)->southExitX = rand() % (MAP_W - 4) + 3;
	} else if (s == 0) {
		(*map)->southExitX = southMap->northExitX;
	}
	
	if (w == 1) {
		(*map)->westExitY = rand() % (MAP_H - 4) + 3;
	} else if (w == 0) {
		(*map)->westExitY = westMap->eastExitY;
	}

	initialize_map(map);

	return m;
}

void gen_path(map_t **map) {
	uint8_t i, j;

	// initialize empty map	
	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			(*map)->values[i][j] = MAP_CLEARING;
		}
	}

	// create N/S border
	for (i = 0; i < MAP_W; i++) {
		(*map)->values[0][i] = MAP_BORDER; // N
		(*map)->values[MAP_H - 1][i] = MAP_BORDER; // S
	}

	// create E/W border
	for (i = 0; i < MAP_H; i++) {
		(*map)->values[i][0] = MAP_BORDER;
		(*map)->values[i][MAP_W - 1] = MAP_BORDER;
	}

	// if the two intersections overlap, switch connections	
	if ((*map)->northExitX > (*map)->southExitX && (*map)->westExitY > (*map)->eastExitY) {
		// create path from north border to intersectX1, intersectY2 
		for (i = 0; i <= (*map)->eastExitY; i++) {
			(*map)->values[i][(*map)->northExitX] = MAP_PATH;	
		}

		// create path from west border to intersectX2, intersectY1 
		for (i = 0; i <= (*map)->southExitX; i++) {
			(*map)->values[(*map)->westExitY][i] = MAP_PATH;	
		}

		// create path from south border to southExitX, westExitY or eastExitY
		for (i = MAP_H-1; i >= (*map)->westExitY || i >= (*map)->eastExitY; i--) {
			(*map)->values[i][(*map)->southExitX] = MAP_PATH;	
		}

		// create path from east border to northExitX or southExitX, eastExitY 
		for (i = MAP_W-1; i >= (*map)->northExitX || i >= (*map)->southExitX; i--) {
			(*map)->values[(*map)->eastExitY][i] = MAP_PATH;	
		}

	} else {
		// create path from north border to northExitX, westExitY 
		for (i = 0; i <= (*map)->westExitY; i++) {
			(*map)->values[i][(*map)->northExitX] = MAP_PATH;	
		}

		// create path from west border to northExitX, westExitY 
		for (i = 0; i <= (*map)->northExitX || i <= (*map)->southExitX; i++) {
			(*map)->values[(*map)->westExitY][i] = MAP_PATH;	
		}

		// create path from south border to southExitX, eastExitY 
		for (i = MAP_H-1; i >= (*map)->eastExitY || i >= (*map)->westExitY; i--) {
			(*map)->values[i][(*map)->southExitX] = MAP_PATH;	
		}

		// create path from east border to southExitX, eastExitY 
		for (i = MAP_W-1; i >= (*map)->southExitX; i--) {
			(*map)->values[(*map)->eastExitY][i] = MAP_PATH;	
		}
	}	
}

void gen_pokecenter(map_t **map) {
	unsigned tries = 0;	
	unsigned found = 0;

	// pick a location for the pokemart and pokecenter
	tries = 0;
	unsigned pokecenterX = rand() % MAP_W;
	unsigned pokecenterY = 1;

	while (!found) {
		if (pokecenterX > 0 && pokecenterX < MAP_W-2 && pokecenterY > 0 && pokecenterY < MAP_H - 2 && (*map)->values[pokecenterY - 1][pokecenterX] == MAP_PATH &&
				(*map)->values[pokecenterY][pokecenterX] != MAP_PATH && (*map)->values[pokecenterY][pokecenterX+1] != MAP_PATH && (*map)->values[pokecenterY+1][pokecenterX] != MAP_PATH && (*map)->values[pokecenterY+1][pokecenterX+1] != MAP_PATH) {
			(*map)->values[pokecenterY][pokecenterX] = MAP_POKECENTER;
			(*map)->values[pokecenterY][pokecenterX + 1] = MAP_POKECENTER;
			(*map)->values[pokecenterY + 1][pokecenterX] = MAP_POKECENTER;
			(*map)->values[pokecenterY + 1][pokecenterX + 1] = MAP_POKECENTER;

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
				// printf("Trying to gen pokecenter again\n");
				gen_path(map);
				tries = 0;
			}
		}
	}

	(*map)->pokecenter[0] = pokecenterX;
	(*map)->pokecenter[1] = pokecenterY;
}

void gen_pokemart(map_t **map) {
	unsigned tries = 0, found = 0;
	unsigned pokecenterX = 0, pokecenterY = 0;

	unsigned pokemartY = rand() % MAP_H;
	unsigned pokemartX = 1;

	while (!found) {
		if (pokemartX > 0 && pokemartX < MAP_W-2 && pokemartY > 0 && pokemartY < MAP_H - 2 && (*map)->values[pokemartY][pokemartX - 1] == MAP_PATH &&
				(abs(pokecenterX - pokemartX) > 2 || abs(pokecenterY - pokemartY) > 2) &&
				(*map)->values[pokemartY][pokemartX] != MAP_PATH && (*map)->values[pokemartY][pokemartX+1] != MAP_PATH && (*map)->values[pokemartY+1][pokemartX] != MAP_PATH && (*map)->values[pokemartY+1][pokemartX+1] != MAP_PATH) {

			(*map)->values[pokemartY][pokemartX] = MAP_POKEMART;
			(*map)->values[pokemartY][pokemartX + 1] = MAP_POKEMART;
			(*map)->values[pokemartY + 1][pokemartX] = MAP_POKEMART;
			(*map)->values[pokemartY + 1][pokemartX + 1] = MAP_POKEMART;

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
				// printf("Trying to gen pokemart again\n");
				gen_path(map);
				gen_pokecenter(map);
				tries = 0;
			}
		}
	}

	(*map)->pokemart[0] = pokemartX;
	(*map)->pokemart[1] = pokemartY;

	(*map)->pokecenter[0] = pokecenterX;
	(*map)->pokecenter[1] = pokecenterY;
}

void gen_tall_grass(map_t **map, uint8_t x, uint8_t y, uint8_t last_dir, uint8_t depth) {
	if (depth > MAX_TALL_GRASS_DEPTH) {
		return;
	}
	
	if (x < (MAP_W-1) && x > 0 && y < (MAP_H-1) && y > 0) {
		if ((*map)->values[y][x] == MAP_CLEARING) {
			if ((rand() % 100) < TREE_CHANCE) {
				(*map)->values[y][x] = MAP_TREE;
			} else {
				(*map)->values[y][x] = MAP_TALL_GRASS;
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

void initialize_map(map_t **map) {
	uint8_t i = 0;

	gen_path(map);

	gen_pokecenter(map);	

	gen_pokemart(map);

	unsigned tall_grass_seedX = 1;
	unsigned tall_grass_seedY = 1;

	for (i = 0; i < MAP_TALL_GRASS_REGIONS; i++) {
		char seed_value = 0;
		while (seed_value != MAP_CLEARING) {
			tall_grass_seedX = (rand() % (MAP_W - 2)) + 1; // select an x on the left half of the screen
			tall_grass_seedY = (rand() % (MAP_H - 2)) + 1; // select an x on the left half of the screen
			seed_value = (*map)->values[tall_grass_seedY][tall_grass_seedX];
		}	
		
		gen_tall_grass(map, tall_grass_seedX, tall_grass_seedY, 5, 0);
	}

	// printf("pokemart X = %d, pokemart Y = %d, pokecenter X = %d, pokecenter Y = %d\n", pokemartX, pokemartY, pokecenterX, pokecenterY);
	
}

void print_map(map_t **map) {
	uint16_t i, j;

	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			printf("%c", (*map)->values[i][j]);	
		}
		printf("\n");
	}

	printf("\n");
}

int main(int argc, char* argv[]) {
	map_t *world[WORLD_H][WORLD_W] = {0};
	map_t *currentMap = NULL;
	uint8_t playing = 1;
	int16_t x = 0, y = 0;

	char command[4] = {0};
	int32_t flyX = 0, flyY = 0;

	srand(time(NULL)); // set the seed for the random number generator
	get_map(world, x, y, &currentMap);

	while(playing) {
		print_map(&currentMap);

		scanf(" %4s", command);

		if (!strcmp(command, "q")) {
			// quit
			playing = 0;
		} else if (!strcmp(command, "n")) {
			// move north
			flyY = y - 1;
		} else if (!strcmp(command, "e")) {
			// move east
			flyX = x + 1;
		} else if (!strcmp(command, "s")) {
			// move south
			flyY = y + 1;
		} else if (!strcmp(command, "w")) {
			// move west
			flyX = x - 1;
		} else if (!strcmp(command, "fly")) {
			// move west
			scanf(" %d %d", &flyX, &flyY);
		}
		
		if (flyX + 200 < WORLD_W && flyX + 200 >= 0 && flyY + 200 < WORLD_H && flyY + 200 >= 0) {
			x = flyX;
			y = flyY;
		} else {
			printf("Bounds provided are outside of map\n");
		}
		
		get_map(world, x, y, &currentMap);
	}

	free_world(world);

	return 0;
}
