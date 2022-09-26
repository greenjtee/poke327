#ifndef UTILITY_H
#define UTILITY_H

#include <inttypes.h>

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint16_t pair_t[num_dims];

#endif