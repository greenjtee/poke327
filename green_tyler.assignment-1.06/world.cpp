#include "world.hpp"

#include <cstdlib>
#include <ncurses.h>
#include <limits.h>

world::world(uint8_t num_trainers) : pc(5, 5)
{
    this->display_menu = menu_map;
    this->trainer_start_index = 0;
    this->num_trainers = num_trainers;
    this->cur_idx[dim_x] = this->cur_idx[dim_y] = this->world_size / 2;
    this->skip_queue = false;

    this->new_map();
    this->generate_cost_maps();
}

int world::new_map()
{
    int n, s, e, w;
    int d, p;

    map *m = this->cur_map();

    if (m)
    {
        return 0;
    }

    if (!this->cur_idx[dim_y])
    {
        n = -1;
    }
    else if (this->get_map(this->cur_idx[dim_y] - 1, this->cur_idx[dim_x]))
    {
        n = this->get_map(this->cur_idx[dim_y] - 1, this->cur_idx[dim_x])->s;
    }
    else
    {
        n = 1 + rand() % (map::map_x - 2);
    }
    if (this->cur_idx[dim_y] == world_size - 1)
    {
        s = -1;
    }
    else if (this->get_map(this->cur_idx[dim_y] + 1, this->cur_idx[dim_x]))
    {
        s = this->get_map(this->cur_idx[dim_y] + 1, this->cur_idx[dim_x])->n;
    }
    else
    {
        s = 1 + rand() % (map::map_x - 2);
    }
    if (!this->cur_idx[dim_x])
    {
        w = -1;
    }
    else if (this->get_map(this->cur_idx[dim_y], this->cur_idx[dim_x] - 1))
    {
        w = this->get_map(this->cur_idx[dim_y], this->cur_idx[dim_x] - 1)->e;
    }
    else
    {
        w = 1 + rand() % (map::map_y - 2);
    }
    if (this->cur_idx[dim_x] == world_size - 1)
    {
        e = -1;
    }
    else if (this->get_map(this->cur_idx[dim_y], this->cur_idx[dim_x] + 1))
    {
        e = this->get_map(cur_idx[dim_y], this->cur_idx[dim_x] + 1)->w;
    }
    else
    {
        e = 1 + rand() % (map::map_y - 2);
    }

    d = (abs(this->cur_idx[dim_x] - (world_size / 2)) +
         abs(this->cur_idx[dim_y] - (world_size / 2)));
    p = d > 200 ? 5 : (50 - ((45 * d) / 200));

    this->set_map(new map(n, s, e, w, d, p, this->num_trainers, this->pc.pos), this->cur_idx[dim_y], this->cur_idx[dim_x]);
    heap_insert(&this->cur_map()->player_queue, &this->pc);
    return 0;
}

bool world::process_input()
{
    bool valid_input = false;
    int status;

    int flyX, flyY;

    wchar_t c;

    while (!valid_input)
    {
        status = STATUS_OK;
        valid_input = true;

        c = getch();

        switch (c)
        {
        case '7': // attempt to move pc upper left
        case 'y':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(-1, -1, *this);
                this->generate_cost_maps();
            }
            break;
        case '8': // attempt to move pc up
        case 'k':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(-1, 0, *this);
                this->generate_cost_maps();
            }
            break;
        case '9': // attempt to move pc upper right
        case 'u':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(-1, 1, *this);
                this->generate_cost_maps();
            }
            break;
        case '6': // attempt to move pc right
        case 'l':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(0, 1, *this);
                this->generate_cost_maps();
            }
            break;
        case '3': // attempt to move pc lower right
        case 'n':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(1, 1, *this);
                this->generate_cost_maps();
            }
            break;
        case '2': // attempt to move pc down
        case 'j':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(1, 0, *this);
                this->generate_cost_maps();
            }
            break;
        case '1': // attempt to move pc lower left
        case 'b':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(1, -1, *this);
                this->generate_cost_maps();
            }
            break;
        case '4': // attempt to move pc left
        case 'h':
            if (this->display_menu == menu_map)
            {
                status = this->pc.move(0, -1, *this);
                this->generate_cost_maps();
            }
            break;
        case '>': // attempt to enter pokecenter or pokemart
            if (this->cur_map()->get_terrain(this->pc.pos[dim_y], this->pc.pos[dim_x]) == ter_center)
            {
                this->display_menu = menu_pokecenter;
            }
            else if (this->cur_map()->get_terrain(this->pc.pos[dim_y], this->pc.pos[dim_x]) == ter_mart)
            {
                this->display_menu = menu_pokemart;
                status = STATUS_OK;
            }
            else
            {
                // not on a valid spot
                status = STATUS_CENTER_ERROR;
            }
            break;
        case '<': // leave pokemart or pokecenter
            this->display_menu = menu_map;
            this->skip_queue = true;
            status = STATUS_OK;
            break;
        case '5': // rest for turn
        case ' ':
        case '.':
            status = STATUS_OK;
            // this->pc.next_move += 10;
            break;
        case 't': // display trainer list
            this->display_menu = menu_trainer_list;
            status = STATUS_OK;
            break;
        case KEY_UP: // scroll up trainer list
            if (this->display_menu == menu_trainer_list && this->trainer_start_index > 0)
            {
                trainer_start_index--;
                status = STATUS_OK;
            }
            else
            {
                valid_input = 0;
                status = STATUS_BEGINNING;
            }
            break;
        case KEY_DOWN: // scroll down trainer list
            if (this->display_menu == menu_trainer_list && this->trainer_start_index < this->num_trainers)
            {
                this->trainer_start_index++;
                status = STATUS_OK;
            }
            else
            {
                valid_input = 0;
                status = STATUS_END;
            }
            break;
        case 'v': // faster than pressing escape so I added this one
        case 27:  // escape - return to character control from trainer list
            this->display_menu = menu_map;
            this->skip_queue = true;
            this->battling = nullptr;
            status = STATUS_OK;
            break;
        case 'Q': // quit game
            return false;
            break;
        case 'f': // fly
            mvaddstr(WINDOW_STATUS_TOP, 0, "Enter coordinates: ");
            refresh();
            echo();
            scanw("%d %d", &flyX, &flyY);
            noecho();
            flyX += 200;
            flyY += 200;
            if (flyX < 0 || flyX >= world_size || flyY < 0 || flyY >= world_size)
            {
                mvaddstr(WINDOW_STATUS_TOP, 0, "Coordinates out of bounds");
                // refresh();
            }
            else
            {
                this->cur_idx[dim_x] = flyX;
                this->cur_idx[dim_y] = flyY;

                this->new_map();
                this->pc.next_move = this->cur_map()->time;

                // find a place to put the pc
                this->place_pc();
            }
            break;

        default:
            status = STATUS_DEFAULT;
            break;
        }

        if (status != 0)
        {
            // invalid input
            valid_input = 0;

            // print reason for bad input
            if (status == STATUS_MOVE_ERROR)
            {
                // sprintf(statusMessage, "Error: ")
                mvaddstr(WINDOW_STATUS_TOP, 0, "Error: cant move here");
            }
            else if (status == STATUS_CENTER_ERROR)
            {
                // tried to enter pokemart/pokecenter not at location
                mvaddstr(WINDOW_STATUS_TOP, 0, "Error: cant enter pokemart/pokecenter from here");
            }
            else if (status == STATUS_BATTLE)
            {
                this->display_menu = menu_battle;
                valid_input = 1;
            }

            refresh();
        }
        else
        {
            move(WINDOW_STATUS_TOP, 0);
            clrtoeol();
            refresh();
            // mvaddstr(WINDOW_STATUS_TOP, 0, "");
        }
    }

    return true;
}

// simulates a time step of the world until the next pc turn
bool world::next()
{
    trainer *next_player = (trainer *)heap_remove_min(&this->cur_map()->player_queue);
    int min_cost;
    pair_t to;

    while (next_player != (trainer *)&this->pc)
    {
        min_cost = next_player->get_next_move(*this, to);

        if (to[dim_x] == this->pc.pos[dim_x] && to[dim_y] == this->pc.pos[dim_y] && !next_player->defeated)
        { // tried to move to pc pos
            this->display_menu = menu_battle;
            this->battling = next_player;

            next_player->next_move = this->cur_map()->time + min_cost;
            this->cur_map()->time = next_player->next_move;
            heap_insert(&this->cur_map()->player_queue, next_player);
            break;
        }

        if (min_cost != INT_MAX)
        { // otherwise if min cost is not infinite
            next_player->next_move = this->cur_map()->time + min_cost;
            this->cur_map()->time = next_player->next_move;

            next_player->pos[dim_x] = to[dim_x];
            next_player->pos[dim_y] = to[dim_y];
            heap_insert(&this->cur_map()->player_queue, next_player);
        }
        next_player = (trainer *)heap_remove_min(&this->cur_map()->player_queue);
    }

    if (next_player == (trainer *)&this->pc)
    {
        // pc may not be next up if the last trainer entered a battle
        // pc_last_pos[dim_x] = world.pc.pos[dim_x];
        // pc_last_pos[dim_y] = world.pc.pos[dim_y];

        // print_map_nc(world.cur_map);
        // refresh();
        this->pc.next_move = this->cur_map()->time + this->pc_cost_map[this->pc.pos[dim_y]][this->pc.pos[dim_x]].cost;
        heap_insert(&this->cur_map()->player_queue, &this->pc);
    }

    return true;
}

void world::generate_cost_maps()
{
    this->cur_map()->dijkstra_map(this->pc_cost_map, this->pc.pos, trainer_pc);
    this->cur_map()->dijkstra_map(this->hiker_cost_map, this->pc.pos, trainer_pc);
    this->cur_map()->dijkstra_map(this->rival_cost_map, this->pc.pos, trainer_pc);
}

void world::place_pc()
{
    uint8_t i, j;
    for (i = 0; i < map::map_y; i++)
    {
        for (j = 0; j < map::map_x; j++)
        {
            if (this->cur_map()->get_terrain(i, j) == ter_clearing) {
                this->pc.pos[dim_y] = i;
                this->pc.pos[dim_x] = j;
                return;
            }
        }
    }

    // no good places to put pc
}

world::~world()
{
    uint16_t i, j;
    for (i = 0; i < world_size; i++)
    {
        for (j = 0; j < world_size; j++)
        {
            delete this->maps[i][j];
        }
    }
}
