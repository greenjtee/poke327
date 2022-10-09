#include "trainer.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "world.h"
#include "map.h"

extern world_t world;

void place_trainers() {
    uint8_t i, j;
    uint8_t done;

    for (i = 0; i < world.num_trainers; i++) {
        world.cur_map->trainers[i].pos[dim_x] = 0;
        world.cur_map->trainers[i].pos[dim_y] = 0;
    }

    if (world.num_trainers < 1) {
        printf("world num_trainers should not be less than 1\n");
        return;
    } else if (world.num_trainers == 1) {
        if (rand() % 2) { // randomly pick a hiker or rival to place
            world.cur_map->trainers[0].type = trainer_hiker;
        } else {
            world.cur_map->trainers[0].type = trainer_rival;
        }

        done = 0;

        while (!done) {
            world.cur_map->trainers[0].pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
            world.cur_map->trainers[0].pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;

            if (world.pc.pos[dim_x] != world.cur_map->trainers[0].pos[dim_x] || 
                world.pc.pos[dim_y] != world.cur_map->trainers[0].pos[dim_y]) {
                done = 1;
            }
        }
        world.cur_map->trainers[i].direction = (rand()%2) * 2 - 1;
        world.cur_map->trainers[i].heading = rand()%2;

    } else {
        // place hiker
        done = 0;

        while (!done) {
            world.cur_map->trainers[0].pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
            world.cur_map->trainers[0].pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;

            if (world.pc.pos[dim_x] != world.cur_map->trainers[0].pos[dim_x] || 
                world.pc.pos[dim_y] != world.cur_map->trainers[0].pos[dim_y]) {
                done = 1;
            }
        }
        world.cur_map->trainers[0].type = trainer_hiker;
        world.cur_map->trainers[0].direction = (rand()%2) * 2 - 1;
        world.cur_map->trainers[0].heading = rand()%2;

        // place rival
        done = 0;

        while (!done) {
            world.cur_map->trainers[1].pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
            world.cur_map->trainers[1].pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;

            if (world.pc.pos[dim_x] != world.cur_map->trainers[1].pos[dim_x] || 
                world.pc.pos[dim_y] != world.cur_map->trainers[1].pos[dim_y]) {
                done = 1;
            }

            if (world.cur_map->trainers[0].pos[dim_x] != world.cur_map->trainers[1].pos[dim_x] || 
                world.cur_map->trainers[0].pos[dim_y] != world.cur_map->trainers[1].pos[dim_y]) {
                done = 1;
            } else {
                done = 0;
            }
        }

        world.cur_map->trainers[1].type = trainer_rival;
        world.cur_map->trainers[1].direction = (rand()%2) * 2 - 1;
        world.cur_map->trainers[1].heading = rand()%2;

        // place remaining trainers
        for (i = 2; i < world.num_trainers; i++) { // place trainer number i
            done = 0; // not done

            while (!done) {
                done = 1;

                world.cur_map->trainers[i].pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
                world.cur_map->trainers[i].pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;

                if (world.pc.pos[dim_x] == world.cur_map->trainers[i].pos[dim_x] && 
                    world.pc.pos[dim_y] == world.cur_map->trainers[i].pos[dim_y]) {
                    done = 0;
                    continue;
                }
    
                for (j = 0; j < i; j++) {
                    if (world.cur_map->trainers[j].pos[dim_x] == world.cur_map->trainers[i].pos[dim_x] && 
                        world.cur_map->trainers[j].pos[dim_y] == world.cur_map->trainers[i].pos[dim_y]) {
                        done = 0;
                        continue;
                    }
                }

                world.cur_map->trainers[i].type = (rand() % (trainer_num-1)) + 1;
                world.cur_map->trainers[i].direction = (rand()%2) * 2 - 1;
                world.cur_map->trainers[i].heading = rand()%2;
            }

            // printf("trainer %d at %d %d\n", world.cur_map->trainers[i].type, world.cur_map->trainers[i].pos[dim_x], world.cur_map->trainers[i].pos[dim_y]);
        }


        // printf("trainers placed: %d\n", world.num_trainers);
    }

    for (i = 0; i < world.num_trainers; i++) {
        world.cur_map->trainers[i].nextMove = 0;
        world.cur_map->trainers[i].n = heap_insert(&world.cur_map->trainer_queue, &world.cur_map->trainers[i]);
    }
}

uint32_t get_max_descent(trainer_t *others, path_t map[MAP_Y][MAP_X], trainer_t *t, pair_t to) {
    uint32_t maxDescent = INT_MAX; // quickest descent is terrain with lowest cost
    uint32_t gradient;
    int8_t i, j, k, occupied;

    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            if (i == 0 && j == 0) { // dont check self
                continue;
            }

            if (t->pos[dim_y] + i <= 0 || t->pos[dim_x] + j <= 0 || t->pos[dim_y] + i >= (MAP_Y-1) || t->pos[dim_x] + j >= (MAP_X-1)) { // border map check
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

uint32_t get_next_move(trainer_t *t, pair_t to) {
    uint32_t max = INT_MAX;
    int32_t newX, newY;
    uint32_t k, cost;
    uint8_t occupied;

    switch (t->type) {
        /*
        case trainer_pc:
            max = getMaxDescent(world->cur_map->trainers, world->pc_cost_map, t, &to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
            break;
        */

        case trainer_hiker:
            max = get_max_descent(world.cur_map->trainers, world.hiker_cost_map, t, to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
            break;
        case trainer_rival:
            max = get_max_descent(world.cur_map->trainers, world.rival_cost_map, t, to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
            break;
        case trainer_pacer:
            newX = t->pos[dim_x];
            newY = t->pos[dim_y];

            if (t->heading) { // N/S pacer
                newY = t->pos[dim_y] + t->direction;
            } else { // E/W pacer
                newX = t->pos[dim_x] + t->direction;
            }

            if (newX < (MAP_X-1) && newX > 0 && newY < (MAP_Y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map->trainers[k].pos[dim_y] == newY && world.cur_map->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
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
            t->direction *= -1;
            
            // stay in the same place until next turn
            to[dim_x] = t->pos[dim_x];
            to[dim_y] = t->pos[dim_y];

            return 30; // turnaround cost
            break;
        case trainer_wanderer:
            newX = t->pos[dim_x];
            newY = t->pos[dim_y];

            if (t->heading) { // moving n/s
                newY = t->pos[dim_y] + t->direction;
            } else { // moving e/w
                newX = t->pos[dim_x] + t->direction;
            }

            if (newX < (MAP_X-1) && newX > 0 && newY < (MAP_Y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map->trainers[k].pos[dim_y] == newY && world.cur_map->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
                        occupied = 1;
                        break;
                    }
                }

                cost = world.rival_cost_map[newY][newX].cost;
                if (!occupied && cost != INT_MAX && (world.pc.pos[dim_y] != newY || world.pc.pos[dim_x] != newX)
                     && world.cur_map->map[newY][newX] == world.cur_map->map[t->pos[dim_y]][t->pos[dim_x]]) { // cant move to pc location or out of current terrain region
                    to[dim_y] = newY;
                    to[dim_x] = newX;
                    return cost;
                }
            }

            // turn around if we made it this far
            t->direction = (rand() % 2) * 2 - 1;
            t->heading = rand() % 2;
            
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
            newX = t->pos[dim_x];
            newY = t->pos[dim_y];

            if (t->heading) { // moving n/s
                newY = t->pos[dim_y] + t->direction;
            } else { // moving e/w
                newX = t->pos[dim_x] + t->direction;
            }

            if (newX < (MAP_X-1) && newX > 0 && newY < (MAP_Y-1) && newY > 0) {
                occupied = 0;

                for (k = 0; k < world.num_trainers; k++) {
                    if (world.cur_map->trainers[k].pos[dim_y] == newY && world.cur_map->trainers[k].pos[dim_x] == newX) { // if space is occupied by another trainer
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
            t->direction = (rand() % 2) * 2 - 1;
            t->heading = rand() % 2;
            
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
