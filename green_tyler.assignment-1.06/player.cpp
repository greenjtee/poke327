#include "player.hpp"

player::player(uint8_t y, uint8_t x) {
    this->pos[dim_y] = y;
    this->pos[dim_x] = x;

    this->velocity[dim_x] = 0;
    this->velocity[dim_y] = 0;

    this->next_move = 0;
}

int player::move(int y, int x) {
    return 0;
}

player::~player() {

}
