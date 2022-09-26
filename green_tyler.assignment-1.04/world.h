#ifndef WORLD_H
#define WORLD_H

#include "map.h"

#define WORLD_SIZE 401
#define DEFAULT_NUM_TRAINERS 10

typedef struct world {
  map_t *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  map_t *cur_map;
  path_t hiker_cost_map[MAP_Y][MAP_X];
  path_t rival_cost_map[MAP_Y][MAP_X];
} world_t;

void init_world();
void delete_world();

#endif
