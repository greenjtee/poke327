#ifndef TRAINER_H
#define TRAINER_H

// #include "map.h"
// #include "world.h"
#include "utility.h"

typedef struct world world_t;

typedef enum trainer_type {
  trainer_pc,
  trainer_hiker,
  trainer_rival,
  trainer_pacer,
  trainer_wanderer,
  trainer_sentry,
  trainer_explorer,
  trainer_num
} trainer_type_t;

typedef struct trainer {
    pair_t pos;
    trainer_type_t type;
    int32_t nextMove;
    heap_node_t* n;
    int8_t direction;
    int8_t heading;
} trainer_t;

void place_trainers();
uint32_t getNextMove(trainer_t *t, pair_t to);
uint32_t getMaxDescent(trainer_t *others, path_t map[MAP_Y][MAP_X], trainer_t *pos, pair_t to);

#endif
