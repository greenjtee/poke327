#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "character.hpp"

class player : public character {
private:

public:
    player(uint8_t y, uint8_t x);
    ~player();

    int move(int y, int x);

};

#endif
