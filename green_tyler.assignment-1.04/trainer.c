#include "trainer.h"

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "world.h"
#include "map.h"

extern world_t world;

void place_trainers() {
    uint8_t i, j;

    trainer_t hiker;
    trainer_t rival;
    uint8_t done;

    hiker.type = trainer_hiker;
    rival.type = trainer_rival;

    for (i = 0; i < world.num_trainers; i++) {
        world.cur_map->trainers[i] = hiker;
        world.cur_map->trainers[i].pos[dim_x] = 0;
        world.cur_map->trainers[i].pos[dim_y] = 0;
    }

    if (world.num_trainers < 1) {
        printf("world num_trainers should not be less than 1\n");
        return;
    } else if (world.num_trainers == 1) {
        if (rand() % 2) { // randomly pick a hiker or rival to place
            world.cur_map->trainers[1] = hiker;
        } else {
            world.cur_map->trainers[1] = rival;
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

uint32_t getMaxDescent(trainer_t *others, path_t map[MAP_Y][MAP_X], trainer_t *t, pair_t *to) {
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
                (*to)[dim_y] = t->pos[dim_y] + i;
                (*to)[dim_x] = t->pos[dim_x] + j;
            }
        }
    }

    return maxDescent;
}

uint32_t getNextMove(world_t *world, trainer_t *t, pair_t *to) {
    uint32_t max = INT_MAX;

    switch (t->type) {
        /*
        case trainer_pc:
            max = getMaxDescent(world->cur_map->trainers, world->pc_cost_map, t, &to);
            t->pos[dim_x] = to[dim_x];
            t->pos[dim_y] = to[dim_y];
            break;
        */

        case trainer_hiker:
            max = getMaxDescent(world->cur_map->trainers, world->hiker_cost_map, t, to);
            t->pos[dim_x] = (*to)[dim_x];
            t->pos[dim_y] = (*to)[dim_y];
            break;
        case trainer_rival:
            max = getMaxDescent(world->cur_map->trainers, world->rival_cost_map, t, to);
            t->pos[dim_x] = (*to)[dim_x];
            t->pos[dim_y] = (*to)[dim_y];
            break;
        case trainer_pacer:
            break;
        case trainer_wanderer:
            break;
        case trainer_sentry:
            break;
        case trainer_explorer:
            break;
        case trainer_pc:
        case trainer_num:
            break;
    }

    return max;
}
