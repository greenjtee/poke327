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

#define STATUS_CENTER_ERROR 2
#define STATUS_OK 0
#define STATUS_END 3
#define STATUS_BEGINNING 4

extern world_t world;

typedef enum menu {
	menu_map,
	menu_trainer_list,
	menu_pokemart,
	menu_pokecenter,
	menu_battle
} menu_t;

void display_trainer_list(uint8_t start_index) {
	uint8_t i, j = 1;
	int16_t x, y;
	char c;
	move(0,0);
	printw("- trainers ---------------------------------------------------------------------");

	for (i = 0; i < MAP_Y-2 && (i+start_index) < world.num_trainers; i++) {
		clrtoeol();
		move(j++, 0);
		switch (world.cur_map->trainers[i+start_index].type) {
			case trainer_hiker:
				c = 'h';
				break;
			case trainer_rival:
				c = 'r';
				break;
			case trainer_pacer:
				c = 'p';
				break;
			case trainer_wanderer:
				c = 'w';
				break;
			case trainer_sentry:
				c = 's';
				break;
			case trainer_explorer:
				c = 'e';
				break;
			default:
				c = '?';
				break;

		}

		x = world.pc.pos[dim_x] - world.cur_map->trainers[i+start_index].pos[dim_x];
		y = world.pc.pos[dim_y] - world.cur_map->trainers[i+start_index].pos[dim_y];

		if (x < 0) {
			if (y < 0) {
				printw("%c, %d south and %d east", c, abs(y), abs(x));
			} else {
				printw("%c, %d north and %d east", c, abs(y), abs(x));
			}
		} else {
			if (y < 0) {
				printw("%c, %d south and %d west", c, abs(y), abs(x));
			} else {
				printw("%c, %d north and %d west", c, abs(y), abs(x));
			}

		}

	}
	move(MAP_Y-1,0);
	printw("--------------------------------------------------------------------------------");
}

void display_pokemart() {
	move(0,0);
	printw("- pokemart/pokecenter ----------------------------------------------------------");
	move(MAP_Y-1,0);
	printw("--------------------------------------------------------------------------------");
}

void display_battle(trainer_t* battling) {
	move(0,0);
	printw("- battle -----------------------------------------------------------------------");
	move(MAP_Y-1,0);
	printw("--------------------------------------------------------------------------------");

	battling->defeated = 1;
}

int main(int argc, char* argv[]) {
	uint8_t playing = 1;
	uint8_t argi = 0;
	uint32_t ch;
	int32_t status = 0, valid_input = 0;
	trainer_t* nextUp;
	uint32_t min_cost;
	pair_t pc_last_pos, to;
	time_t seed = time(NULL);
	menu_t displayMenu = menu_map;
	uint8_t trainer_start_index = 0;
	uint8_t skip_queue = 0;
	trainer_t* battlingTrainer = NULL;
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

	pc_last_pos[dim_x] = 0;
	pc_last_pos[dim_y] = 0;

	print_map_nc(world.cur_map);
	refresh();

	while(playing) {
		valid_input = 0;
		while(!valid_input) {
			status = STATUS_OK;
			valid_input = 1;
			ch = getch();

			switch(ch) {
				case '7': // attempt to move pc upper left
				case 'y':
					if (displayMenu == menu_map) {
						status = move_pc(-1, -1);
					}
					break;
				case '8': // attempt to move pc up
				case 'k':
					if (displayMenu == menu_map) {
						status = move_pc(-1, 0);
					}
					break;
				case '9': // attempt to move pc upper right
				case 'u':
					if (displayMenu == menu_map) {
						status = move_pc(-1, 1);
					}
					break;
				case '6': // attempt to move pc right
				case 'l':
					if (displayMenu == menu_map) {
						status = move_pc(0, 1);
					}
					break;
				case '3': // attempt to move pc lower right
				case 'n':
					if (displayMenu == menu_map) {
						status = move_pc(1, 1);
					}
					break;
				case '2': // attempt to move pc down
				case 'j':
					if (displayMenu == menu_map) {
						status = move_pc(1, 0);
					}
					break;
				case '1': // attempt to move pc lower left
				case 'b':
					if (displayMenu == menu_map) {
						status = move_pc(1, -1);
					}
					break;
				case '4': // attempt to move pc left
				case 'h':
					if (displayMenu == menu_map) {
						status = move_pc(0, -1);
					}
					break;
				case '>': // attempt to enter pokecenter or pokemart
					if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] == ter_center) {
						displayMenu = menu_pokecenter;
					} else if (world.cur_map->map[world.pc.pos[dim_y]][world.pc.pos[dim_x]] == ter_mart) {
						displayMenu = menu_pokemart;
						status = STATUS_OK;
					} else {
						// not on a valid spot
						status = STATUS_CENTER_ERROR;
					}
					break;
				case '<': // leave pokemart or pokecenter
					displayMenu = menu_map;
					skip_queue = 1;
					status = STATUS_OK;
					break;
				case '5': // rest for turn
				case ' ':
				case '.':
					status = STATUS_OK;
					break;
				case 't': // display trainer list
					displayMenu = menu_trainer_list;
					status = STATUS_OK;
					break;
				case KEY_UP: // scroll up trainer list
					if (displayMenu == menu_trainer_list && trainer_start_index > 0) {
						trainer_start_index--;
						status = STATUS_OK;
					} else {
						valid_input = 0;
						status = STATUS_BEGINNING;
					}
					break;
				case KEY_DOWN: // scroll down trainer list
					if (displayMenu == menu_trainer_list && trainer_start_index < world.num_trainers) {
						trainer_start_index++;
						status = STATUS_OK;
					} else {
						valid_input = 0;
						status = STATUS_END;
					}
					break;
				case 'v': // faster than pressing escape so I added this one
				case 27: // escape - return to character control from trainer list
					displayMenu = menu_map;
					skip_queue = 1;
					status = STATUS_OK;
					break;
				case 'Q': // quit game
					playing = 0;
					status = STATUS_OK;
					break;
				default:
					status = 1;
					break;
			}

			if (status != 0) {
				// invalid input
				valid_input = 0;

				// print reason for bad input
				if (status == STATUS_MOVE_ERROR) {
					// sprintf(statusMessage, "Error: ")
					mvaddstr(WINDOW_STATUS_TOP, 0, "Error: cant move here");
				} else if (status == STATUS_CENTER_ERROR) {
					// tried to enter pokemart/pokecenter not at location
					mvaddstr(WINDOW_STATUS_TOP, 0, "Error: cant enter pokemart/pokecenter from here");
				} else if (status == STATUS_BATTLE) {
					displayMenu = menu_battle;
				}

				refresh();
			} else {
				move(WINDOW_STATUS_TOP, 0);
				clrtoeol();
				refresh();
				// mvaddstr(WINDOW_STATUS_TOP, 0, "");
			}
		}
		
		switch (displayMenu) {
			case menu_map:
				if (!skip_queue) {
					new_map();

					if (world.pc.pos[dim_x] != pc_last_pos[dim_x] || world.pc.pos[dim_y] != pc_last_pos[dim_y]) { // update cost map if pc position changed
						dijkstra_map(world.cur_map, world.pc_cost_map, world.pc.pos, trainer_pc);
						dijkstra_map(world.cur_map, world.hiker_cost_map, world.pc.pos, trainer_hiker);
						dijkstra_map(world.cur_map, world.rival_cost_map, world.pc.pos, trainer_rival);
					}

					nextUp = heap_remove_min(&world.cur_map->trainer_queue);
					while (nextUp != &world.pc) {
						min_cost = get_next_move(nextUp, to);

						if (to[dim_x] == world.pc.pos[dim_x] && to[dim_y] == world.pc.pos[dim_y] && !nextUp->defeated) { // tried to move to pc pos
							displayMenu = menu_battle;
							battlingTrainer = nextUp;

							nextUp->nextMove = world.time + min_cost;
							world.time = nextUp->nextMove;
							heap_insert(&world.cur_map->trainer_queue, nextUp);
							break;
						}
						
						if (min_cost != INT_MAX) { // otherwise if min cost is not infinite
							nextUp->nextMove = world.time + min_cost;
							world.time = nextUp->nextMove;

							nextUp->pos[dim_x] = to[dim_x];
							nextUp->pos[dim_y] = to[dim_y];
							heap_insert(&world.cur_map->trainer_queue, nextUp);
						}
						nextUp = heap_remove_min(&world.cur_map->trainer_queue);
					}

					if (nextUp == &world.pc) { // pc may not be next up if the last trainer entered a battle
						pc_last_pos[dim_x] = world.pc.pos[dim_x];
						pc_last_pos[dim_y] = world.pc.pos[dim_y];

						// print_map_nc(world.cur_map);
						// refresh();

						world.pc.nextMove = world.time + 10;

						// mvaddstr(WINDOW_STATUS_TOP, 0, "Status: ");
						heap_insert(&world.cur_map->trainer_queue, &world.pc);
					}

				}
				clear();
				print_map_nc(world.cur_map);
				refresh();

				skip_queue = 0;

				break;
			
			case menu_trainer_list:
				clear();
				display_trainer_list(trainer_start_index);
				refresh();

				break;

			case menu_pokecenter:
			case menu_pokemart:
				clear();
				display_pokemart();
				// display_pokecenter();
				refresh();
				break;

			case menu_battle:
				clear();
				display_battle(battlingTrainer);
				refresh();

				break;
		}

	}

	endwin();

	delete_world();
	return 0;
}
