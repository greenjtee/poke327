#ifndef MAP_H
#define MAP_H

#include "heap.h"
#include <inttypes.h>

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
} path_t;

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;

typedef uint16_t pair_t[num_dims];

#define MAP_X              80
#define MAP_Y              21
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef enum __attribute__ ((__packed__)) terrain_type {
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

typedef enum __attribute__ ((__packed__)) trainer_type {
  trainer_pc,
  trainer_hiker,
  trainer_rival,
  trainer_other,
  trainer_num
} trainer_type_t;

typedef struct map {
  terrain_type_t map[MAP_Y][MAP_X];
  uint8_t height[MAP_Y][MAP_X];
  uint8_t n, s, e, w;
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

#endif