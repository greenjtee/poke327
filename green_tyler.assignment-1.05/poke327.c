#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <ncurses.h>

#include "map.h"
#include "heap.h"
#include "world.h"
#include "trainer.h"

extern world_t world;

int main(int argc, char* argv[]) {
	uint8_t playing = 1;
	uint8_t argi = 0;
	uint32_t ch;
	uint8_t trainer_list_open = 0;
	int32_t status = 0, valid_input = 0;
	trainer_t* nextUp;
	uint32_t min_cost;
	pair_t pc_last_pos, to;
	time_t seed = time(NULL);
	// char statusMessage[MAP_X] = {0};

	world.num_trainers = DEFAULT_NUM_TRAINERS;
	world.time = 0;

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

	// printf("using seed %ld\n", seed);
	srand(seed); // set the seed for the random number generator

	init_world();

	initscr();
	raw();
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
	start_color();

	init_pair(ter_boulder, COLOR_WHITE, COLOR_BLACK);
	init_pair(ter_tree, COLOR_GREEN, COLOR_BLACK);
	init_pair(ter_path, COLOR_CYAN, COLOR_BLACK);
	init_pair(ter_mart, COLOR_RED, COLOR_BLACK);
	init_pair(ter_center, COLOR_RED, COLOR_BLACK);
	init_pair(ter_grass, COLOR_GREEN, COLOR_BLACK);
	init_pair(ter_clearing, COLOR_GREEN, COLOR_BLACK);
	init_pair(ter_mountain, COLOR_WHITE, COLOR_BLACK);
	init_pair(ter_forest, COLOR_GREEN, COLOR_BLACK);
	init_pair(ter_exit, COLOR_WHITE, COLOR_BLACK);

	refresh();

	pc_last_pos[dim_x] = 0;
	pc_last_pos[dim_y] = 0;

	while(playing) {
		new_map();

		print_map_nc(world.cur_map);
		refresh();

		if (world.pc.pos[dim_x] != pc_last_pos[dim_x] || world.pc.pos[dim_y] != pc_last_pos[dim_y]) { // update cost map if pc position changed
			dijkstra_map(world.cur_map, world.pc_cost_map, world.pc.pos, trainer_pc);
			dijkstra_map(world.cur_map, world.hiker_cost_map, world.pc.pos, trainer_hiker);
			dijkstra_map(world.cur_map, world.rival_cost_map, world.pc.pos, trainer_rival);
		}

		nextUp = heap_remove_min(&world.cur_map->trainer_queue);
		while (nextUp != &world.pc) {
			min_cost = get_next_move(nextUp, to);
			
			if (min_cost != INT_MAX) {
				nextUp->nextMove = world.time + min_cost;
				world.time = nextUp->nextMove;

				nextUp->pos[dim_x] = to[dim_x];
				nextUp->pos[dim_y] = to[dim_y];
				heap_insert(&world.cur_map->trainer_queue, nextUp);
			}
			nextUp = heap_remove_min(&world.cur_map->trainer_queue);
		}

		pc_last_pos[dim_x] = world.pc.pos[dim_x];
		pc_last_pos[dim_y] = world.pc.pos[dim_y];

		print_map_nc(world.cur_map);
		refresh();

		// usleep(100000);

		// print_cost_map(world.hiker_cost_map);

		// mvaddstr(WINDOW_STATUS_TOP, 0, "Status: ");

		valid_input = 0;
		while(!valid_input) {
			valid_input = 1;
			ch = getch();

			switch(ch) {
				case '7': // attempt to move pc upper left
				case 'y':
					if (!trainer_list_open) {
						status = move_pc(-1, -1);
					}
					break;
				case '8': // attempt to move pc up
				case 'k':
					if (!trainer_list_open) {
						status = move_pc(-1, 0);
					}
					break;
				case '9': // attempt to move pc upper right
				case 'u':
					if (!trainer_list_open) {
						status = move_pc(-1, 1);
					}
					break;
				case '6': // attempt to move pc right
				case 'l':
					if (!trainer_list_open) {
						status = move_pc(0, 1);
					}
					break;
				case '3': // attempt to move pc lower right
				case 'n':
					if (!trainer_list_open) {
						status = move_pc(1, 1);
					}
					break;
				case '2': // attempt to move pc down
				case 'j':
					if (!trainer_list_open) {
						status = move_pc(1, 0);
					}
					break;
				case '1': // attempt to move pc lower left
				case 'b':
					if (!trainer_list_open) {
						status = move_pc(1, -1);
					}
					break;
				case '4': // attempt to move pc left
				case 'h':
					if (!trainer_list_open) {
						status = move_pc(0, -1);
					}
					break;
				case '>': // attempt to enter pokecenter or pokemart
					break;
				case '5': // rest for turn
				case ' ':
				case '.':
					break;
				case 't': // display trainer list
					break;
				case KEY_UP: // scroll up trainer list
					break;
				case KEY_DOWN: // scroll down trainer list
					break;
				case 27: // escape - return to character control from trainer list
					break;
				case 'Q': // quit game
					playing = 0;
					break;
				default:
					status = 1;
					break;
			}

			if (status != 0) {
				// invalid input
				valid_input = 0;

				// print reason for bad input
				if (status == -1) {
					// sprintf(statusMessage, "Error: ")
					mvaddstr(WINDOW_STATUS_TOP, 0, "Error: cant move here");
				} else if (status == 1) {
					//no op -- do nothing but wait for valid input from user
				}

				refresh();
			} else {
				move(WINDOW_STATUS_TOP, 0);
				clrtoeol();
				refresh();
				// mvaddstr(WINDOW_STATUS_TOP, 0, "");
			}
		}

		world.pc.nextMove = world.time + 10;

		// mvaddstr(WINDOW_STATUS_TOP, 0, "Status: ");
		heap_insert(&world.cur_map->trainer_queue, &world.pc);

	}

	endwin();

	delete_world();
	return 0;
}
