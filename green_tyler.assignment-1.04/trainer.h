#ifndef TRAINER_H
#define TRAINER_H

#include "map.h"
#include "utility.h"

typedef enum trainer_type {
  trainer_pc,
  trainer_hiker,
  trainer_rival,
  trainer_other,
  trainer_pacer,
  trainer_wanderer,
  trainer_sentry,
  trainer_explorer,
  trainer_num
} trainer_type_t;

typedef struct trainer {
    pair_t pos;
    trainer_type_t type;
} trainer_t;

#endif