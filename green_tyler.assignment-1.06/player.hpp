#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "character.hpp"

class player : public character {
private:

public:
    player();
    ~player();

    int move(int y, int x);

};

#endif
