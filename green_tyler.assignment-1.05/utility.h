#ifndef UTILITY_H
#define UTILITY_H

#include <inttypes.h>
#include "heap.h"

#define MAP_X              80
#define MAP_Y              21

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint16_t pair_t[num_dims];

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;


#endif