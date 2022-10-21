#ifndef TRAINER_HPP
#define TRAINER_HPP

#include "character.hpp"

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

class trainer : public character {

public:
    trainer_type_t type;
    bool defeated;

    trainer();
    ~trainer();

    int move(int y, int x);

};

#endif
