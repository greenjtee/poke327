#include "trainer.hpp"

trainer::trainer() {
    this->defeated = false;
    this->type = trainer_hiker;

    this->pos[dim_x] = 0;
    this->pos[dim_y] = 0;

    this->velocity[dim_x] = 0;
    this->velocity[dim_y] = 0;

    this->next_move = 0;
}

int trainer::move(int y, int x) {
    return 0;
}

trainer::~trainer() {

}
