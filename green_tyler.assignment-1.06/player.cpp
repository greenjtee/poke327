#include "player.hpp"

player::player() {
    this->pos[dim_x] = 0;
    this->pos[dim_y] = 0;

    this->velocity[dim_x] = 0;
    this->velocity[dim_y] = 0;

    this->next_move = 0;
}

int player::move(int y, int x) {
    return 0;
}

player::~player() {

}
