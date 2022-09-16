#ifndef WORLD_H
#define WORLD_H

#include "map.h"

#define WORLD_SIZE 401

typedef struct world {
  map_t *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  map_t *cur_map;
} world_t;

void init_world();
void delete_world();

#endif