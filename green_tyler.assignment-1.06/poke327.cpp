#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>

#include "world.hpp"

#define DEFAULT_NUM_TRAINERS 10

#define STATUS_OK 0
#define STATUS_CENTER_ERROR 2
#define STATUS_END 3
#define STATUS_BEGINNING 4

void display_trainer_list(world &world)
{
    uint8_t i, j = 1;
    int16_t x, y;
    char c;
    move(0, 0);
    printw("- trainers ---------------------------------------------------------------------");

    for (i = 0; i < map::map_y - 2 && (i + world.trainer_start_index) < world.num_trainers; i++)
    {
        clrtoeol();
        move(j++, 0);
        switch (world.cur_map()->trainers[i + world.trainer_start_index].type)
        {
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

        x = world.pc.pos[dim_x] - world.cur_map()->trainers[i + world.trainer_start_index].pos[dim_x];
        y = world.pc.pos[dim_y] - world.cur_map()->trainers[i + world.trainer_start_index].pos[dim_y];

        if (x < 0)
        {
            if (y < 0)
            {
                printw("%c, %d south and %d east", c, abs(y), abs(x));
            }
            else
            {
                printw("%c, %d north and %d east", c, abs(y), abs(x));
            }
        }
        else
        {
            if (y < 0)
            {
                printw("%c, %d south and %d west", c, abs(y), abs(x));
            }
            else
            {
                printw("%c, %d north and %d west", c, abs(y), abs(x));
            }
        }
    }
    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");
}

void display_pokemart()
{
    move(0, 0);
    printw("- pokemart/pokecenter ----------------------------------------------------------");
    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");
}

void display_battle(trainer *battling)
{
    move(0, 0);
    printw("- battle -----------------------------------------------------------------------");
    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");

    battling->defeated = true;
}

void initialize_ncurses()
{
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
}

void end_ncurses()
{
    endwin();
}

int main(int argc, char *argv[])
{
    uint8_t num_trainers = DEFAULT_NUM_TRAINERS;
    time_t seed = time(NULL);
    int argi;

    for (argi = 0; argi < argc; argi++)
    {
        if (!strcmp(argv[argi], "--numtrainers"))
        {
            // number of trainers in the map specified
            if (argc > (argi + 1))
            { // make sure the number was provided
                num_trainers = atoi(argv[argi + 1]);
                if (num_trainers == 0)
                {
                    std::cerr << "the number of trainers must be an integer greater than 0" << std::endl;
                    return 1;
                }
                std::cout << "number of trainers: " << num_trainers << std::endl;
            }
            else
            {
                std::cout << "number of trainers not given, using default: " << num_trainers << std::endl;
            }
        }

        if (!strcmp(argv[argi], "--seed"))
        {
            if (argc > (argi + 1))
            { // make sure the number was provided
                seed = atoi(argv[argi + 1]);
            }
            else
            {
                std::cerr << "seed not provided" << std::endl;
                return 1;
            }
        }
    }

    srand(seed);
    initialize_ncurses();

    world w(num_trainers);

    w.print_map();
    refresh();

    while (true)
    {
        if (!w.process_input())
        {
            break;
        }

        switch (w.display_menu)
        {
        case menu_map:
            if (!w.skip_queue)
            {
                w.next();
            }
            clear();
            w.print_map();
            refresh();

            w.skip_queue = false;
            break;

        case menu_trainer_list:
            clear();
            display_trainer_list(w);
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
            display_battle(w.battling);
            refresh();

            break;
        }
    }

    end_ncurses();

    return 0;
}
