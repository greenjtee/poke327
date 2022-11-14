#ifndef WORLD_HPP
#define WORLD_HPP

#include <inttypes.h>

#include "map.hpp"
#include "player.hpp"
#include "trainer.hpp"
#include "pokedex.hpp"
#include "pokemon.hpp"

#include "utility.hpp"

typedef enum status
{
    status_ok,
    status_default,
    status_center_error,
    status_end,
    status_beginning,
    status_move_error,
    status_battle,
    status_encounter
} status_t;

typedef enum menu
{
    menu_map,
    menu_trainer_list,
    menu_pokemart,
    menu_pokecenter,
    menu_battle,
    menu_encounter,
    menu_bag
} menu_t;

class world
{

public:
    static const uint16_t world_size = 401;
    uint8_t num_trainers;

    map *maps[world_size][world_size];
    pair_t cur_idx;

    path_t pc_cost_map[map::map_y][map::map_x];
    path_t hiker_cost_map[map::map_y][map::map_x];
    path_t rival_cost_map[map::map_y][map::map_x];

    menu_t display_menu;
    uint8_t trainer_start_index;
    bool skip_queue;
    trainer *battling;

    player pc;

    pokedex pdex;

    pokemon* encounter;

    world(uint8_t num_trainers, std::vector<std::string> &paths);

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
    bool next();
    void generate_cost_maps();
    void place_pc();
    bool encounter_pokemon();
    void gen_trainer_pokemon();

    ~world();
};

#endif
