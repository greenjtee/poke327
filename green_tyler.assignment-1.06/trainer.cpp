#include "trainer.hpp"

#include <limits.h>
#include <random>

#include "map.hpp"
#include "world.hpp"

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

uint32_t get_max_descent(world &world, path_t map[map::map_y][map::map_x], trainer *t, pair_t to) {
    uint32_t maxDescent = INT_MAX; // quickest descent is terrain with lowest cost
    uint32_t gradient;
    int8_t i, j, k, occupied;
    trainer *others = world.cur_map()->trainers;

    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            if (i == 0 && j == 0) { // dont check self
                continue;
            }

            if (t->pos[dim_y] + i <= 0 || t->pos[dim_x] + j <= 0 || t->pos[dim_y] + i >= (map::map_y-1) || t->pos[dim_x] + j >= (map::map_x-1)) { // border map check
                continue;
            }

            gradient = map[(t->pos)[dim_y] + i][(t->pos)[dim_x] + j].cost;
            occupied = 0;

            for (k = 0; k < world.num_trainers; k++) {
                if (others[k].pos[dim_y] == t->pos[dim_y] + i && others[k].pos[dim_x] == t->pos[dim_x] + j) { // if space is occupied by another trainer
                    occupied = 1;
                    break;
                }

            }

            if (world.pc.pos[dim_y] == t->pos[dim_y] + i && world.pc.pos[dim_x] == t->pos[dim_x] + j) { // cant move to pc location
                occupied = 1;
            }

            if (!occupied && gradient <= maxDescent) {
                if (gradient == maxDescent) { // two paths with same cost, randomly pick to swap or stay
                    if (rand() % 2) {
                        continue;
                    }
                }
                maxDescent = gradient;
                to[dim_y] = t->pos[dim_y] + i;
                to[dim_x] = t->pos[dim_x] + j;
            }
        }
    }

    return maxDescent;
}

uint32_t trainer::get_next_move(world &world, trainer *t, pair_t to) {
    uint32_t max = INT_MAX;
    int32_t newX, newY;
    uint32_t k, cost;
    uint8_t occupied;

    switch (t->type) {
        case trainer_hiker:
            max = get_max_descent(world, world.rival_cost_map, t, to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
        case trainer_rival:
            max = get_max_descent(world, world.hiker_cost_map, t, to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
            break;
        case trainer_pacer:
            newX = t->pos[dim_x] + t->velocity[dim_x];
            newY = t->pos[dim_y] + t->velocity[dim_y];

            if (newX < (map::map::map_x-1) && newX > 0 && newY < (map::map::map_y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
                        occupied = 1;
                        break;
                    }
                }

                cost = world.rival_cost_map[newY][newX].cost;
                if (!occupied && cost != INT_MAX && (world.pc.pos[dim_y] != newY || world.pc.pos[dim_x] != newX)) { // cant move to pc location
                    to[dim_y] = newY; //t->pos[dim_y];
                    to[dim_x] = newX;
                    return cost;
                }

            }

            // turn around if we made it this far
            t->velocity[dim_y] *= -1;
            t->velocity[dim_x] *= -1;
            
            // stay in the same place until next turn
            to[dim_x] = t->pos[dim_x];
            to[dim_y] = t->pos[dim_y];

            return 30; // turnaround cost
            break;
        case trainer_wanderer:
            newX = t->pos[dim_x] + t->velocity[dim_x];
            newY = t->pos[dim_y] + t->velocity[dim_y];

            if (newX < (map::map_x-1) && newX > 0 && newY < (map::map_y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
                        occupied = 1;
                        break;
                    }
                }

                cost = world.rival_cost_map[newY][newX].cost;
                if (!occupied && cost != INT_MAX && (world.pc.pos[dim_y] != newY || world.pc.pos[dim_x] != newX)
                     && world.cur_map()->get_terrain(newY, newX) == world.cur_map()->get_terrain(t->pos[dim_y], t->pos[dim_x])) { // cant move to pc location or out of current terrain region
                    to[dim_y] = newY;
                    to[dim_x] = newX;
                    return cost;
                }
            }

            // turn around if we made it this far
            t->velocity[dim_y] = (rand() % 2) * 2 - 1;
            t->velocity[dim_x] = (rand() % 2) * 2 - 1;
            
            // stay in the same place until next turn
            to[dim_x] = t->pos[dim_x];
            to[dim_y] = t->pos[dim_y];

            return 30; // turnaround cost
            break;
        case trainer_sentry: // sentries dont move
            to[dim_y] = t->pos[dim_y];
            to[dim_x] = t->pos[dim_x];
            break;
        case trainer_explorer:
            newY = t->pos[dim_y] + t->velocity[dim_y];
            newX = t->pos[dim_x] + t->velocity[dim_x];

            if (newX < (map::map_x-1) && newX > 0 && newY < (map::map_y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
                        occupied = 1;
                        break;
                    }
                }

                cost = world.rival_cost_map[newY][newX].cost;
                if (!occupied && cost != INT_MAX && (world.pc.pos[dim_y] != newY || world.pc.pos[dim_x] != newX)) { // cant move to pc location
                    to[dim_y] = newY;
                    to[dim_x] = newX;
                    return cost;
                }
            }

            // turn around if we made it this far
            t->velocity[dim_x] = (rand() % 2) * 2 - 1;
            t->velocity[dim_y] = (rand() % 2) * 2 - 1;
            
            // stay in the same place until next turn
            to[dim_x] = t->pos[dim_x];
            to[dim_y] = t->pos[dim_y];

            return 30; // turnaround cost
 
            break;
        case trainer_pc:
        case trainer_num:
            break;
    }

    return max;
}

trainer::~trainer() {

}
