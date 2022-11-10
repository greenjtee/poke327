#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <string>

#include "world.hpp"

#include "pokedex.hpp"

#define DEFAULT_NUM_TRAINERS 10

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

void display_encounter(pokemon *encounter)
{
    int i = 2;
    move(0, 0);
    printw("- encounter --------------------------------------------------------------------");
    move(i++, 0);
    printw("%s", encounter->type->identifier.c_str());
    move(i++, 0);
    printw("\tlevel: %d", encounter->level);
    move(i++, 0);
    printw("\thp: %d", encounter->hp());
    move(i++, 0);
    printw("\tattack: %d", encounter->attack());
    move(i++, 0);
    printw("\tdefense: %d", encounter->defense());
    move(i++, 0);
    printw("\tspeed: %d", encounter->speed());
    move(i++, 0);
    printw("\tspecial attack: %d", encounter->special_attack());
    move(i, 0);
    printw("\tspecial defense: %d", encounter->special_defense());
    i += 2;
    move(i++, 0);
    printw("\tmoves");
    for (move_t *m : encounter->moves)
    {
        move(i++, 0);
        printw("\t\t%s", m->identifier.c_str());
    }
    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");
}

// let the user choose from a list of starting pokemon
// return the index of the chosen pokemon
pokemon_t *display_select_first_pokemon(std::vector<pokemon_t *> starter_list)
{
    char ch;
    move(0, 0);
    printw("- Select your very own starting pokemon! ---------------------------------------");

    // list out some randomly selected pokemon
    move(2, 0);
    printw(" 1) %s", starter_list[0]->identifier.c_str());
    move(4, 0);
    printw(" 2) %s", starter_list[1]->identifier.c_str());
    move(6, 0);
    printw(" 3) %s", starter_list[2]->identifier.c_str());

    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");

    while (true)
    {
        ch = getch();

        switch (ch)
        {
        case '1':
            return starter_list[0];
        case '2':
            return starter_list[1];
        case '3':
            return starter_list[2];
        default:
            move(map::map_y - 2, 0);
            printw("Invalid choice, please select from 1 - 3");
            break;
        }
    }
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
    int argi;

    std::vector<std::string> paths;
    std::string homepath = getenv("HOME") + std::string("/.poke327/pokedex/pokedex/data/csv");
    std::string sharepath = "/share/cs327" + std::string("/pokedex/pokedex/data/csv");

    paths.push_back(homepath);
    paths.push_back(sharepath);

    uint8_t num_trainers = DEFAULT_NUM_TRAINERS;
    time_t seed = time(NULL);

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
    world w(num_trainers, paths);
    initialize_ncurses();

    // get valid starter pokemon
    std::vector<pokemon_t *> starter_pokemon_list;
    w.pdex.get_random_pokemon_list(3, starter_pokemon_list);

    pokemon_t *starting_pokemon = display_select_first_pokemon(starter_pokemon_list);
    pokemon *pc_starting_pokemon = new pokemon;
    pc_starting_pokemon->type = starting_pokemon;
    w.pc.m_pokemon.push_back(pc_starting_pokemon);

    pc_starting_pokemon->level = 0;
    w.pdex.get_pokemon_moves(pc_starting_pokemon);
    w.pdex.get_pokemon_species(pc_starting_pokemon);
    w.pdex.get_pokemon_stats(pc_starting_pokemon);
    pc_starting_pokemon->generate_base_stats();

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
        case menu_encounter:
            clear();
            display_encounter(w.encounter);
            refresh();
        }
    }

    end_ncurses();

    return 0;
}
