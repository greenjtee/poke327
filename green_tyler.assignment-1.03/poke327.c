#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

#include "heap.h"
#include "map.h"
#include "world.h"

extern world_t world;

int main(int argc, char* argv[]) {
	uint8_t playing = 1;
	int16_t flyX = 0, flyY = 0;

	char command[4] = {0};
	srand(time(NULL)); // set the seed for the random number generator

	init_world();

	print_map(world.cur_map);

	while(playing) {
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
		new_map();
		print_map(world.cur_map);
	}

	delete_world();
	return 0;
}
