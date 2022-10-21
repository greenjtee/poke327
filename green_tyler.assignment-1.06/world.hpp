#ifndef WORLD_HPP
#define WORLD_HPP

#include <inttypes.h>

#include "map.hpp"
#include "player.hpp"

#include "utility.hpp"

#define STATUS_OK 0
#define STATUS_DEFAULT 1
#define STATUS_CENTER_ERROR 2
#define STATUS_END 3
#define STATUS_BEGINNING 4
#define STATUS_MOVE_ERROR 5
#define STATUS_BATTLE 6

typedef enum menu
{
    menu_map,
    menu_trainer_list,
    menu_pokemart,
    menu_pokecenter,
    menu_battle
} menu_t;

class world
{
private:
    menu_t display_menu;
    uint8_t trainer_start_index;

public:
    static const uint16_t world_size = 401;
    uint8_t num_trainers;

    map *maps[world_size][world_size];
    pair_t cur_idx;

    path_t pc_cost_map[map::map_y][map::map_x];
    path_t hiker_cost_map[map::map_y][map::map_x];
    path_t rival_cost_map[map::map_y][map::map_x];

    player pc;

    world(uint8_t num_trainers);

    int new_map();

    void print_map() const
    {
        this->maps[cur_idx[dim_y]][cur_idx[dim_x]]->print(this->pc.pos, this->num_trainers);
    }

    map *cur_map()
    {
        return this->maps[cur_idx[dim_y]][cur_idx[dim_x]];
    }

    map *get_map(int y, int x)
    {
        return this->maps[y][x];
    }

    void set_map(map *m, int y, int x)
    {
        this->maps[y][x] = m;
    }

    bool process_input();

    ~world();
};

#endif
