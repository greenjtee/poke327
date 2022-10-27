#ifndef UTILITY_H
#define UTILITY_H

#include <inttypes.h>
#include "heap.h"

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint16_t pair_t[num_dims];

typedef struct path {
  heap_node_t *hn = nullptr;
  pair_t pos;
  pair_t from;
  int32_t cost = 0;
} path_t;

#endif
