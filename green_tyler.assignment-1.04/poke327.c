#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "map.h"
#include "heap.h"
#include "world.h"
#include "trainer.h"

extern world_t world;

int main(int argc, char* argv[]) {
	uint8_t playing = 1;
	// int16_t flyX = 0, flyY = 0;
	uint8_t argi = 0;
	// uint32_t i;
	world.num_trainers = DEFAULT_NUM_TRAINERS;
	world.time = 0;

	// pair_t pc;
	// pc[dim_x] = 5;
	// pc[dim_y] = 5;


	// char command[4] = {0};
	time_t seed = time(NULL);

	for (argi = 0; argi < argc; argi++) {
		if (!strcmp(argv[argi], "--numtrainers")) {
			// number of trainers in the map specified
			if (argc > (argi+1)) { // make sure the number was provided
				world.num_trainers = atoi(argv[argi+1]);
				if (world.num_trainers == 0) {
					printf("the number of trainers must be an integer greater than 0\n");
					return 1;
				}
				printf("number of trainers: %d\n", world.num_trainers);
			} else {
				printf("number of trainers not given, using default: %d\n", world.num_trainers);
			}
		}
		
		if (!strcmp(argv[argi], "--seed")) {
			if (argc > (argi+1)) { // make sure the number was provided
				seed = atoi(argv[argi+1]);
			} else {
				printf("seed not provided\n");
			}
		}
	}

	printf("using seed %ld\n", seed);
	srand(seed); // set the seed for the random number generator

	init_world();

	pair_t lastPos;
	lastPos[dim_x] = 0; // world.pc.pos[dim_x];
	lastPos[dim_y] = 0; // world.pc.pos[dim_y];

	while(playing) {
		new_map();

		if (world.pc.pos[dim_x] != lastPos[dim_x] || world.pc.pos[dim_y] != lastPos[dim_y]) { // update cost map if pc position changed
			dijkstra_map(world.cur_map, world.pc_cost_map, world.pc.pos, trainer_pc);
			dijkstra_map(world.cur_map, world.hiker_cost_map, world.pc.pos, trainer_hiker);
			dijkstra_map(world.cur_map, world.rival_cost_map, world.pc.pos, trainer_rival);
		}

		pair_t to;
		trainer_t* nextUp = heap_remove_min(&world.cur_map->trainer_queue);
		if (nextUp) {
			uint32_t minCost = getNextMove(&world, nextUp, &to);
			
			if (minCost != INT_MAX) {
				nextUp->nextMove = world.time + minCost;
				world.time = nextUp->nextMove;

				nextUp->pos[dim_x] = to[dim_x];
				nextUp->pos[dim_y] = to[dim_y];
				heap_insert(&world.cur_map->trainer_queue, nextUp);
			}

			lastPos[dim_x] = world.pc.pos[dim_x];
			lastPos[dim_y] = world.pc.pos[dim_y];

			print_map(world.cur_map);
			// print_cost_map(world.rival_cost_map);
			usleep(250000);
			// getchar();

		}

		// print_cost_map(world.hiker_cost_map);
		// print_cost_map(world.rival_cost_map);

		/*
		printf("current position is %hd, %hd. Command: ", world.cur_idx[0] - 200, world.cur_idx[1] - 200);
		
		scanf(" %3s", command);

		if (!strcmp(command, "q")) {
			// quit
			playing = 0;
		} else if (!strcmp(command, "n")) {
			// move north
			if (world.cur_idx[1] > 0) {
				world.cur_idx[1]--;
			}
		} else if (!strcmp(command, "e")) {
			// move east
			if (world.cur_idx[0] < (WORLD_SIZE-1)) {
				world.cur_idx[0]++;
			}
		} else if (!strcmp(command, "s")) {
			// move south
			if (world.cur_idx[1] < (WORLD_SIZE-1)) {
				world.cur_idx[1]++;
			}
		} else if (!strcmp(command, "w")) {
			// move west
			if (world.cur_idx[0] > 0) {
				world.cur_idx[0]--;
			}
		} else if (!strcmp(command, "f")) {
			scanf(" %hd %hd", &flyX, &flyY);
			if (flyX + 200 >= 0 && flyX + 200 < WORLD_SIZE && flyY + 200 >= 0 && flyY + 200 < WORLD_SIZE) {
				world.cur_idx[0] = flyX + 200;
				world.cur_idx[1] = flyY + 200;
			}
		} else {
			printf("Unknown Command.\n");
			continue;
		}
		*/
	}

	delete_world();
	return 0;
}
