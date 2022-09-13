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

int8_t get_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map);
int8_t check_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map);
void generate_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t *map);
void print_map(map_t *map);

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
	if (!(world[y+200][x+200])) {
		return 1;
	}
	*map = world[y+200][x+200];

	return 0;
}

int8_t get_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t **map) {
	int8_t m = 0;

	if (x < -200 || x > 200 || y < -200 || y > 200) {
		return -1;
	}

	m = check_map(world, x, y, map);
	if (m == 0) {
		return 0;
	} else if (m == 1) {
		world[y+200][x+200] = *map = (map_t*)malloc(sizeof(map_t));
	}
	
	generate_map(world, x, y, *map);

	return m;
}
	
void generate_map(map_t *world[WORLD_H][WORLD_W], int16_t x, int16_t y, map_t *map) {
	uint8_t i, j;
	map_t *northMap = NULL, *eastMap = NULL, *southMap = NULL, *westMap = NULL;
	int8_t n = 0, e = 0, s = 0, w = 0;

	n = check_map(world, x, y-1, &northMap);
	e = check_map(world, x+1, y, &eastMap);
	s = check_map(world, x, y+1, &southMap); 
	w = check_map(world, x-1, y, &westMap);

	if (n == 1) {
		map->northExitX = rand() % (MAP_W - 4) + 3;
	} else if (n == 0) {
		map->northExitX = northMap->southExitX;
	}
	
	if (e == 1) {
		map->eastExitY = rand() % (MAP_H - 4) + 3;
	} else if (e == 0) {
		map->eastExitY = eastMap->westExitY;
	}
	
	if (s == 1) {
		map->southExitX = rand() % (MAP_W - 4) + 3;
	} else if (s == 0) {
		map->southExitX = southMap->northExitX;
	}
	
	if (w == 1) {
		map->westExitY = rand() % (MAP_H - 4) + 3;
	} else if (w == 0) {
		map->westExitY = westMap->eastExitY;
	}


	// initialize empty map	
	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			map->values[i][j] = MAP_CLEARING;
		}
	}

	// create N/S border
	for (i = 0; i < MAP_W; i++) {
		map->values[0][i] = MAP_BORDER;
		map->values[MAP_H - 1][i] = MAP_BORDER;
	}

	// create E/W border
	for (i = 0; i < MAP_H; i++) {
		map->values[i][0] = MAP_BORDER;
		map->values[i][MAP_W - 1] = MAP_BORDER;
	}
}

void print_map(map_t *map) {
	uint16_t i, j;

	for (i = 0; i < MAP_H; i++) {
		for (j = 0; j < MAP_W; j++) {
			printf("%c", map->values[i][j]);	
		}
		printf("\n");
	}

	printf("\n");
}

int main(int argc, char* argv[]) {
	map_t *world[WORLD_H][WORLD_W] = {0};
	map_t *currentMap = world[200][200];
	uint8_t playing = 1;
	int16_t x = 0, y = 0;

	char command[4] = {0};
	int32_t flyX = 0, flyY = 0;
	
	get_map(world, x, y, &currentMap);
	print_map(currentMap);
	srand(time(NULL)); // set the seed for the random number generator

	while(playing) {
		scanf(" %3s", command);

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
		} else {
			printf("Unknown Command.\n");
			continue;
		}
		
		if (flyX + 200 < WORLD_W && flyX + 200 >= 0 && flyY + 200 < WORLD_H && flyY + 200 >= 0) {
			if (x != flyX || y != flyY) {
				x = flyX;
				y = flyY;
				get_map(world, x, y, &currentMap);
				print_map(currentMap);
			}
		} else {
			printf("Bounds provided are outside of map\n");
		}
	}

	free_world(world);

	return 0;
}
