#include "trainer.hpp"

#include <limits.h>
#include <random>

#include "map.hpp"
#include "world.hpp"

trainer::trainer()
{
    this->defeated = false;
    this->type = trainer_hiker;

    this->pos[dim_x] = 0;
    this->pos[dim_y] = 0;

    this->velocity[dim_x] = 0;
    this->velocity[dim_y] = 0;

    this->next_move = 0;
    this->selected_pokemon = 0;
}

int trainer::move(int y, int x)
{
    return 0;
}

uint32_t get_max_descent(world &world, path_t map[map::map_y][map::map_x], trainer *t, pair_t to)
{
    uint32_t maxDescent = INT_MAX; // quickest descent is terrain with lowest cost
    uint32_t gradient;
    int8_t i, j, k, occupied;
    trainer *others = world.cur_map()->trainers;

    for (i = -1; i < 2; i++)
    {
        for (j = -1; j < 2; j++)
        {
            if (i == 0 && j == 0)
            { // dont check self
                continue;
            }

            if (t->pos[dim_y] + i <= 0 || t->pos[dim_x] + j <= 0 || t->pos[dim_y] + i >= (map::map_y - 1) || t->pos[dim_x] + j >= (map::map_x - 1))
            { // border map check
                continue;
            }

            gradient = map[(t->pos)[dim_y] + i][(t->pos)[dim_x] + j].cost;
            occupied = 0;

            for (k = 0; k < world.num_trainers; k++)
            {
                if (others[k].pos[dim_y] == t->pos[dim_y] + i && others[k].pos[dim_x] == t->pos[dim_x] + j)
                { // if space is occupied by another trainer
                    occupied = 1;
                    break;
                }
            }

            // if (world.pc.pos[dim_y] == t->pos[dim_y] + i && world.pc.pos[dim_x] == t->pos[dim_x] + j) { // cant move to pc location
            //     occupied = 1;
            // }

            if (!occupied && gradient <= maxDescent)
            {
                if (gradient == maxDescent)
                { // two paths with same cost, randomly pick to swap or stay
                    if (rand() % 2)
                    {
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

uint32_t trainer::get_next_move(world &world, pair_t to)
{
    uint32_t max = INT_MAX;
    int32_t newX, newY;
    uint32_t k, cost;
    uint8_t occupied;

    switch (this->type)
    {
    case trainer_hiker:
        if (!this->defeated && world.pc.can_battle())
        {
            max = get_max_descent(world, world.rival_cost_map, this, to);
        } else if (!world.pc.can_battle())
        {
            max = 10;
            to[dim_x] = this->pos[dim_x];
            to[dim_y] = this->pos[dim_y];
        }
        else
        {
            to[dim_x] = this->pos[dim_x];
            to[dim_y] = this->pos[dim_y];
        }
        // t->pos[dim_x] = to[dim_x];
        // t->pos[dim_y] = to[dim_y];
        break;
    case trainer_rival:
        if (!this->defeated && world.pc.can_battle())
        {
            max = get_max_descent(world, world.hiker_cost_map, this, to);
        } else if (!world.pc.can_battle())
        {
            max = 10;
            to[dim_x] = this->pos[dim_x];
            to[dim_y] = this->pos[dim_y];
        }
        else
        {
            to[dim_x] = this->pos[dim_x];
            to[dim_y] = this->pos[dim_y];
        }
        // t->pos[dim_x] = to[dim_x];
        // t->pos[dim_y] = to[dim_y];
        break;
    case trainer_pacer:
        newX = this->pos[dim_x] + this->velocity[dim_x];
        newY = this->pos[dim_y] + this->velocity[dim_y];

        if (newX < (map::map::map_x - 1) && newX > 0 && newY < (map::map::map_y - 1) && newY > 0)
        {
            occupied = 0;

            for (k = 0; k < world.num_trainers; k++)
            {
                if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX)
                { // if space is occupied by another trainer
                    occupied = 1;
                    break;
                }
            }

            cost = world.rival_cost_map[newY][newX].cost;
            if (!occupied && cost != INT_MAX)
            {                     // cant move to pc location
                to[dim_y] = newY; // t->pos[dim_y];
                to[dim_x] = newX;
                return cost;
            }
        }

        // turn around if we made it this far
        this->velocity[dim_y] *= -1;
        this->velocity[dim_x] *= -1;

        // stay in the same place until next turn
        to[dim_x] = this->pos[dim_x];
        to[dim_y] = this->pos[dim_y];

        return 30; // turnaround cost
        break;
    case trainer_wanderer:
        newX = this->pos[dim_x] + this->velocity[dim_x];
        newY = this->pos[dim_y] + this->velocity[dim_y];

        if (newX < (map::map_x - 1) && newX > 0 && newY < (map::map_y - 1) && newY > 0)
        {
            occupied = 0;

            for (k = 0; k < world.num_trainers; k++)
            {
                if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX)
                { // if space is occupied by another trainer
                    occupied = 1;
                    break;
                }
            }

            cost = world.rival_cost_map[newY][newX].cost;
            if (!occupied && cost != INT_MAX && world.cur_map()->get_terrain(newY, newX) == world.cur_map()->get_terrain(this->pos[dim_y], this->pos[dim_x]))
            { // cant move to pc location or out of current terrain region
                to[dim_y] = newY;
                to[dim_x] = newX;
                return cost;
            }
        }

        // turn around if we made it this far
        this->velocity[dim_y] = (rand() % 2) * 2 - 1;
        this->velocity[dim_x] = (rand() % 2) * 2 - 1;

        // stay in the same place until next turn
        to[dim_x] = this->pos[dim_x];
        to[dim_y] = this->pos[dim_y];

        return 30; // turnaround cost
        break;
    case trainer_sentry: // sentries dont move
        to[dim_y] = this->pos[dim_y];
        to[dim_x] = this->pos[dim_x];
        break;
    case trainer_explorer:
        newY = this->pos[dim_y] + this->velocity[dim_y];
        newX = this->pos[dim_x] + this->velocity[dim_x];

        if (newX < (map::map_x - 1) && newX > 0 && newY < (map::map_y - 1) && newY > 0)
        {
            occupied = 0;

            for (k = 0; k < world.num_trainers; k++)
            {
                if (world.cur_map()->trainers[k].pos[dim_y] == newY && world.cur_map()->trainers[k].pos[dim_x] == newX)
                { // if space is occupied by another trainer
                    occupied = 1;
                    break;
                }
            }

            cost = world.rival_cost_map[newY][newX].cost;
            if (!occupied && cost != INT_MAX)
            { // cant move to pc location
                to[dim_y] = newY;
                to[dim_x] = newX;
                return cost;
            }
        }

        // turn around if we made it this far
        this->velocity[dim_x] = (rand() % 2) * 2 - 1;
        this->velocity[dim_y] = (rand() % 2) * 2 - 1;

        // stay in the same place until next turn
        to[dim_x] = this->pos[dim_x];
        to[dim_y] = this->pos[dim_y];

        return 30; // turnaround cost

        break;
    case trainer_pc:
    case trainer_num:
        break;
    }

    return max;
}

trainer::~trainer()
{
}
