#ifndef MAP_HPP
#define MAP_HPP

#include "trainer.hpp"

#include "utility.hpp"

#define WINDOW_MAIN_TOP 1
#define WINDOW_STATUS_TOP 22

typedef enum terrain_type
{
    ter_debug,
    ter_boulder,
    ter_tree,
    ter_path,
    ter_mart,
    ter_center,
    ter_grass,
    ter_clearing,
    ter_mountain,
    ter_forest,
    ter_exit
} terrain_type_t;

typedef struct queue_node
{
    int x, y;
    struct queue_node *next;
} queue_node_t;

class map
{
public:
    static const int map_y = 21, map_x = 80;
    void dijkstra_map(path_t cost_map[map_y][map_x], pair_t from, trainer_type_t trainer);

private:
    static const int min_trees = 10, min_boulders = 10, tree_prob = 95, boulder_prob = 95;
    void dijkstra_path(pair_t from, pair_t to);

    void smooth_height();
    void map_terrain();

    void place_boulders();
    void place_trees();
    void place_pokemart();
    void place_pokecenter();
    void place_trainers(const uint8_t num_trainers, const pair_t player_pos);

    void find_building_location(pair_t p);

    int build_paths();

    void move_trainers();

public:
    void set_terrain(uint8_t y, uint8_t x, terrain_type_t t)
    {
        this->terrain[y][x] = t;
    }

    void set_height(uint8_t y, uint8_t x, int h)
    {
        this->height[y][x] = h;
    }

    terrain_type_t get_terrain(uint8_t y, uint8_t x)
    {
        return this->terrain[y][x];
    }

    terrain_type_t get_terrain(pair_t &p)
    {
        return this->terrain[p[dim_y]][p[dim_x]];
    }

    int get_height(uint8_t y, uint8_t x)
    {
        return this->height[y][x];
    }

    int get_height(pair_t &p)
    {
        return this->height[p[dim_y]][p[dim_x]];
    }

    terrain_type_t terrain[map_y][map_x];
    uint8_t height[map_y][map_x];
    uint8_t n, s, e, w;
    trainer *trainers;
    heap_t player_queue;
    uint32_t time;

    map(int n, int s, int e, int w, int d, int p, const uint8_t num_trainers, const pair_t player_pos);
    void print(const pair_t player_pos, const uint8_t num_trainers) const;

    ~map();
};

#endif
