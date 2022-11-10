#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utility.hpp"

class character {
public:
    pair_t pos, velocity;
    uint32_t next_move;

    // virtual int move(int x, int y) = 0;
};

#endif
