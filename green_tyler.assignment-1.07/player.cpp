#include "player.hpp"

#include "map.hpp"
#include "world.hpp"
#include <limits.h>

#define STATUS_MOVE_ERROR 5
#define STATUS_BATTLE 6

player::player(uint8_t y, uint8_t x)
{
    this->pos[dim_y] = y;
    this->pos[dim_x] = x;

    this->velocity[dim_x] = 0;
    this->velocity[dim_y] = 0;

    this->next_move = 0;
}

int player::move(int y, int x, world &world)
{
    int16_t newY = this->pos[dim_y] + y;
    int16_t newX = this->pos[dim_x] + x;

    // for now, don't let pc move to any edges

    uint8_t i;

    if (newY < 1 || newY > (map::map_y - 2) || newX < 1 || newX > (map::map_x - 2))
    {
        if (newX == map::map_x-1 && newY == world.cur_map()->e)
        { // leaving east exit
            world.cur_idx[dim_x]++;
            // this->pos[dim_y] = newY;
            this->pos[dim_x] = 1;
            this->pos[dim_y] = newY;
            world.new_map();
            this->next_move = world.cur_map()->time;
            return 0;
        }

        if (newX == 0 && newY == world.cur_map()->w)
        { // leaving west exit
            world.cur_idx[dim_x]--;
            // this->pos[dim_y] = newY;
            this->pos[dim_x] = map::map_x-2;
            this->pos[dim_y] = newY;
            world.new_map();
            this->next_move = world.cur_map()->time;
            return 0;
        }

        if (newY == 0 && newX == world.cur_map()->n)
        { // leaving north exit
            world.cur_idx[dim_y]--;
            this->pos[dim_y] = map::map_y-2;
            this->pos[dim_x] = newX;
            // this->pos[dim_x] = newX;
            world.new_map();
            this->next_move = world.cur_map()->time;
            return 0;
        }

        if (newY == map::map_y-1 && newX == world.cur_map()->s)
        { // leaving south exit
            world.cur_idx[dim_y]++;
            this->pos[dim_y] = 1;
            this->pos[dim_x] = newX;
            // this->pos[dim_x] = newX;
            world.new_map();
            this->next_move = world.cur_map()->time;
            return 0;
        }

        return STATUS_MOVE_ERROR;
    }

    if (world.pc_cost_map[newY][newX].cost == INT_MAX)
    {
        return STATUS_MOVE_ERROR;
    }

    for (i = 0; i < world.num_trainers; i++)
    {
        if (newY == world.cur_map()->trainers[i].pos[dim_y] && newX == world.cur_map()->trainers[i].pos[dim_x] && !world.cur_map()->trainers[i].defeated)
        { // pc tried to move to trainer location, battle!
            world.battling = &world.cur_map()->trainers[i];
            return STATUS_BATTLE;
        }
    }

    this->pos[dim_y] = newY;
    this->pos[dim_x] = newX;

    // this->next_move = world.cur_map()->time + world.pc_cost_map[newY][newX].cost;

    return 0;
}

player::~player()
{
}
