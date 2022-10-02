#ifndef WORLD_H
#define WORLD_H

// #include "map.h"
#include "trainer.h"
#include "utility.h"

#define WORLD_SIZE 401
#define DEFAULT_NUM_TRAINERS 10

typedef struct map map_t;
// typedef struct trainer trainer_t;

typedef struct world {
  map_t *world[WORLD_SIZE][WORLD_SIZE];
  pair_t cur_idx;
  map_t *cur_map;
  path_t pc_cost_map[MAP_Y][MAP_X];
  path_t hiker_cost_map[MAP_Y][MAP_X];
  path_t rival_cost_map[MAP_Y][MAP_X];
  uint8_t num_trainers;
  trainer_t pc;
  uint32_t time;
} world_t;

void init_world();
void delete_world();

#endif
