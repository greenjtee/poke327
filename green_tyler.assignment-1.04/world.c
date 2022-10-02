#include "world.h"
#include <stdlib.h>

#include "map.h"

world_t world;

// The world is global because of its size, so init_world is parameterless
void init_world()
{
  world.cur_idx[dim_x] = world.cur_idx[dim_y] = WORLD_SIZE / 2;
  world.pc.pos[dim_x] = 5;
  world.pc.pos[dim_y] = 5;
  world.pc.type = trainer_pc;
  world.pc.nextMove = 0;

  new_map();
}

void delete_world()
{
  int x, y;

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      // if (world.world[y][x]) {
        // heap_delete(&world.world[y][x]->trainer_queue);
      // }
      if (world.world[y][x]) {
        free(world.world[y][x]);
        world.world[y][x] = NULL;

      }
    }
  }
}
