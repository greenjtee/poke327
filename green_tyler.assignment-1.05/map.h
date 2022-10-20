#ifndef MAP_H
#define MAP_H

#include "heap.h"
#include "utility.h"
#include <inttypes.h>
#include "trainer.h"

#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95

#define WINDOW_MAIN_TOP 1
#define WINDOW_STATUS_TOP 22

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum terrain_type {
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

typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  uint8_t n, s, e, w;
  trainer_t* trainers;
  heap_t trainer_queue;
} map_t;

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

int32_t path_cmp(const void *key, const void *with);
int32_t edge_penalty(uint8_t x, uint8_t y);
void dijkstra_path(map_t *m, pair_t from, pair_t to);
void dijkstra_map(map_t *m, path_t cost_map[MAP_Y][MAP_X], pair_t from, trainer_type_t trainer);
int build_paths(map_t *m);
int smooth_height(map_t *m);
void find_building_location(map_t *m, pair_t p);
int place_pokemart(map_t *m);
int place_center(map_t *m);
int map_terrain(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w);
int place_boulders(map_t *m);
int place_trees(map_t *m);
int terrain_cost(terrain_type_t terrain, trainer_type_t trainer);
int new_map();
void print_map(map_t *m);
void print_map_nc(map_t *m);
void print_cost_map(path_t cm[MAP_Y][MAP_X]);

#endif
