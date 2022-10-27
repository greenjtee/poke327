#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "character.hpp"

// #include "world.hpp"

class world;

class player : public character {
private:

public:
    player(uint8_t y, uint8_t x);
    ~player();

    int move(int y, int x, world &world);

};

#endif
