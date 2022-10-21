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

void end_ncurses() {
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

    while (w.process_input());

    end_ncurses();

    return 0;
}
