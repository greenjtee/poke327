#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <ncurses.h>
#include <string>
#include <unistd.h>

#include "world.hpp"

#include "pokedex.hpp"
#include <random>

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

void display_battle(trainer *battling, player &pc)
{
    int i = 2;
    int move_num = 1;
    int pokemon_num = 1;
    char ch, move_ch, pokemon_ch, bag_ch;
    size_t pokemon_index = pc.selected_pokemon;
    size_t encounter_pokemon_index = battling->selected_pokemon;

    bool last_run_failed = false;
    bool encounter_last_move_missed = false;
    bool player_last_move_missed = false;
    bool encounter_defeated_player_pokemon = false;
    bool player_pokemon_defeated_encounter = false;
    bool player_moves_first = false;
    bool bad_move = false;

    int run_attempts = 0;
    int odds_escape = 0;

    move_t *player_move = nullptr;
    move_t *encounter_move = nullptr;
    pokemon *player_pokemon = pc.m_pokemon[pc.selected_pokemon];
    pokemon *encounter_pokemon = battling->m_pokemon[battling->selected_pokemon];

    if (!pc.can_battle() || !battling->can_battle())
    {
        return;
    }

    while (battling->can_battle() && pc.can_battle())
    {

        clear();
        if (last_run_failed)
        {
            move(map::map_y + 1, 0);
            printw("Run failed!");
        }
        else if (player_last_move_missed)
        {
            move(map::map_y + 1, 0);
            printw("Your last move missed!");
        }
        else if (encounter_last_move_missed)
        {
            move(map::map_y + 1, 0);
            printw("The encounter's last move missed!");
        }
        else if (encounter_defeated_player_pokemon)
        {
            move(map::map_y + 1, 0);
            printw("Your pokemon was defeated!");
        }
        else if (player_pokemon_defeated_encounter)
        {
            move(map::map_y + 1, 0);
            printw("The wild pokemon was defeated!");
            return;
        }
        else if (bad_move)
        {
            move(map::map_y + 1, 0);
            printw("Can't move, this pokemon is out of hp!");
        }

        last_run_failed = false;
        encounter_last_move_missed = false;
        player_last_move_missed = false;
        encounter_defeated_player_pokemon = false;
        player_pokemon_defeated_encounter = false;
        bad_move = false;

        i = 2;
        move_num = 1;
        pokemon_num = 1;
        pokemon_index = pc.selected_pokemon;

        run_attempts = 0;
        odds_escape = 0;

        player_move = nullptr;
        encounter_move = nullptr;

        if (pc.m_pokemon.size() == 0)
        {
            return;
        }

        move(2, 2);
        printw("%s", encounter_pokemon->type->identifier.c_str());
        move(3, 2);
        printw("  level: %d", encounter_pokemon->level);
        move(4, 2);
        printw("  hp: %d/%d", encounter_pokemon->remaining_hp, encounter_pokemon->hp());
        move(6, 2);
        printw("  moves");

        i = 7;
        for (move_t *m : encounter_pokemon->moves)
        {
            move(i++, 2);
            printw("    %d) %s", move_num++, m->identifier.c_str());
        }

        // list pokemon with number selection
        move(2, map::map_x / 2 + 1);
        printw("available pokemon");
        i = 3;
        pokemon_num = 1;
        for (pokemon *p : pc.m_pokemon)
        {
            move(i++, map::map_x / 2 + 1);
            printw("  %d) %s", pokemon_num++, p->type->identifier.c_str());
        }

        move(10, map::map_x / 2 + 1);
        printw("1) Potions = %d", pc.potions);
        move(11, map::map_x / 2 + 1);
        printw("2) Revives = %d", pc.revives);

        move(12, map::map_x / 2 + 1);
        printw("%s", pc.m_pokemon[pc.selected_pokemon]->type->identifier.c_str());
        move(13, map::map_x / 2 + 1);
        printw("  level: %d", pc.m_pokemon[pc.selected_pokemon]->level);
        move(14, map::map_x / 2 + 1);
        printw("  hp: %d/%d", pc.m_pokemon[pc.selected_pokemon]->remaining_hp, pc.m_pokemon[pc.selected_pokemon]->hp());
        move(16, map::map_x / 2 + 1);
        printw("  moves");

        move_num = 1;
        i = 17;
        for (move_t *m : pc.m_pokemon[pc.selected_pokemon]->moves)
        {
            move(i++, map::map_x / 2 + 1);
            printw("    %d) %s", move_num++, m->identifier.c_str());
        }

        move(0, 0);
        vline('|', map::map_y);

        move(0, map::map_x / 2 - 1);
        vline('|', map::map_y);

        move(0, map::map_x - 1);
        vline('|', map::map_y);

        move(0, 0);
        printw("- trainer battle ---------------------------------------------------------------");

        move(map::map_y - 1, 0);
        printw("--------------------------------------------------------------------------------");

        move(map::map_y, 0);
        printw("Select an action: 1) move 2) change pokemon 3) run 4) bag");

        // options are: make move, change pokemon, run
        refresh();
        ch = getchar();
        switch (ch)
        {
        case '1': // move
            if (pc.m_pokemon[pc.selected_pokemon]->remaining_hp <= 0)
            {
                bad_move = true;
                continue;
            }
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select a move.");

            refresh();
            move_ch = getchar();
            if (move_ch == '1')
            { // move 1
                player_move = pc.m_pokemon[pc.selected_pokemon]->moves[0];
            }
            else if (move_ch == '2')
            { // move 2
                if (pc.m_pokemon[pc.selected_pokemon]->moves.size() == 2)
                {
                    player_move = pc.m_pokemon[pc.selected_pokemon]->moves[1];
                }
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid move.");
                continue;
            }
            break;
        case '2': // change pokemon
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select a pokemon to switch to.");

            refresh();
            pokemon_ch = getchar();
            pokemon_index = pokemon_ch - '0';

            if (pokemon_index > 0 && pokemon_index <= pc.m_pokemon.size())
            {
                pc.selected_pokemon = pokemon_index - 1;
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid choice");
                continue;
            }
            player_move = nullptr;
            player_pokemon = pc.m_pokemon[pc.selected_pokemon];
            continue;
        case '3': // run
            run_attempts++;

            odds_escape = pc.m_pokemon[pc.selected_pokemon]->speed() * 32 / ((encounter_pokemon->speed() / 4) % 256) + 30 * run_attempts;

            if (rand() % 256 < odds_escape)
            { // trainer escaped
                return;
            }
            else
            {
                last_run_failed = true;
                continue;
            }
        case '4': // bag
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select an item to use.");

            refresh();
            bag_ch = getchar();

            if (bag_ch == '1')
            {
                if (pc.potions)
                {
                    pc.potions--;
                    player_pokemon->remaining_hp += 20;
                    if (player_pokemon->remaining_hp > player_pokemon->hp())
                    {
                        player_pokemon->remaining_hp = player_pokemon->hp();
                    }
                }
            }
            else if (bag_ch == '2')
            {
                if (pc.revives)
                {
                    pc.revives--;

                    refresh();
                    pokemon_ch = getchar();
                    pokemon_index = pokemon_ch - '0';

                    pc.m_pokemon[pokemon_index]->remaining_hp = pc.m_pokemon[pokemon_index]->hp() / 2;
                    pokemon_index = pc.selected_pokemon;

                    move(map::map_y + 1, 0);
                    clrtoeol();
                    printw("Revived pokemon to half health.");
                }
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid choice");
                continue;
            }

            break;
        default:
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Invalid selection.");
            continue;
        }

        if (encounter_pokemon->remaining_hp == 0 && battling->selected_pokemon < battling->m_pokemon.size() - 1)
        {
            battling->selected_pokemon++;
            encounter_pokemon_index = battling->selected_pokemon;
        }

        encounter_move = encounter_pokemon->moves[rand() % encounter_pokemon->moves.size()];

        // pc and pokemon have selected a move, higher priority goes first
        if (encounter_move && player_move)
        {
            if (encounter_move->priority > player_move->priority)
            { // encounter moves first
                player_moves_first = false;
            }
            else if (encounter_move->priority < player_move->priority)
            {
                player_moves_first = true;
            }
            else
            { // higher speed goes first
                if (encounter_pokemon->speed() > pc.m_pokemon[pc.selected_pokemon]->speed())
                {
                    player_moves_first = false;
                }
                else if (encounter_pokemon->speed() > pc.m_pokemon[pc.selected_pokemon]->speed())
                {
                    player_moves_first = true;
                }
                else
                { // choose random
                    if (rand() % 2)
                    {
                        player_moves_first = false;
                    }
                    else
                    {
                        player_moves_first = true;
                    }
                }
            }

            if (player_moves_first)
            {
                player_last_move_missed = !player_pokemon->attack_pokemon(encounter_pokemon, encounter_move);
                if (encounter_pokemon->remaining_hp <= 0)
                {
                    encounter_pokemon->remaining_hp = 0;
                    player_pokemon_defeated_encounter = true;
                    continue;
                }
                encounter_last_move_missed = !encounter_pokemon->attack_pokemon(player_pokemon, encounter_move);
                if (player_pokemon->remaining_hp <= 0)
                {
                    player_pokemon->remaining_hp = 0;
                    encounter_defeated_player_pokemon = true;
                    continue;
                }
            }
            else
            {
                encounter_last_move_missed = !encounter_pokemon->attack_pokemon(player_pokemon, encounter_move);
                if (player_pokemon->remaining_hp == 0)
                {
                    encounter_defeated_player_pokemon = true;
                    continue;
                }
                player_last_move_missed = !player_pokemon->attack_pokemon(encounter_pokemon, encounter_move);
                if (encounter_pokemon->remaining_hp == 0)
                {
                    player_pokemon_defeated_encounter = true;
                    continue;
                }
            }
        }
        else
        {
            encounter_last_move_missed = !encounter_pokemon->attack_pokemon(player_pokemon, encounter_move);
            if (player_pokemon->remaining_hp == 0)
            {
                encounter_defeated_player_pokemon = true;
                continue;
            }
        }
    }

    clear();
    if (!battling->can_battle())
    {
        // wild pokemon lost
        move(map::map_y + 1, 0);
        printw("The trainer was defeated!");
        battling->defeated = true;
    }
    else if (!pc.can_battle())
    {
        // wild pokemon won
        move(map::map_y + 1, 0);
        printw("Your pokemon were defeated!");
    }
}

void display_encounter(pokemon *encounter, player &pc)
{

    int i = 2;
    int move_num = 1;
    int pokemon_num = 1;
    char ch, move_ch, pokemon_ch, bag_ch;
    size_t pokemon_index = pc.selected_pokemon;

    bool last_run_failed = false;
    bool encounter_last_move_missed = false;
    bool player_last_move_missed = false;
    bool encounter_defeated_player_pokemon = false;
    bool player_pokemon_defeated_encounter = false;
    bool player_moves_first = false;
    bool caught_pokemon = false;
    bool bad_move = false;

    int run_attempts = 0;
    int odds_escape = 0;

    move_t *player_move = nullptr;
    move_t *encounter_move = nullptr;
    pokemon *player_pokemon = pc.m_pokemon[pc.selected_pokemon];

    if (!pc.can_battle())
    {
        return;
    }

    while (encounter->remaining_hp > 0 && pc.can_battle() && !caught_pokemon)
    {

        clear();
        if (last_run_failed)
        {
            move(map::map_y + 1, 0);
            printw("Run failed!");
        }
        else if (player_last_move_missed)
        {
            move(map::map_y + 1, 0);
            printw("Your last move missed!");
        }
        else if (encounter_last_move_missed)
        {
            move(map::map_y + 1, 0);
            printw("The encounter's last move missed!");
        }
        else if (encounter_defeated_player_pokemon)
        {
            move(map::map_y + 1, 0);
            printw("Your pokemon was defeated!");
        }
        else if (player_pokemon_defeated_encounter)
        {
            move(map::map_y + 1, 0);
            printw("The wild pokemon was defeated!");
            return;
        }
        else if (bad_move)
        {
            move(map::map_y + 1, 0);
            printw("Can't move, this pokemon is out of hp!");
        }

        last_run_failed = false;
        encounter_last_move_missed = false;
        player_last_move_missed = false;
        encounter_defeated_player_pokemon = false;
        player_pokemon_defeated_encounter = false;
        bad_move = false;

        i = 2;
        move_num = 1;
        pokemon_num = 1;
        pokemon_index = pc.selected_pokemon;

        run_attempts = 0;
        odds_escape = 0;

        player_move = nullptr;
        encounter_move = nullptr;

        if (pc.m_pokemon.size() == 0)
        {
            return;
        }

        move(2, 2);
        printw("%s", encounter->type->identifier.c_str());
        move(3, 2);
        printw("  level: %d", encounter->level);
        move(4, 2);
        printw("  hp: %d/%d", encounter->remaining_hp, encounter->hp());
        move(6, 2);
        printw("  moves");

        i = 7;
        for (move_t *m : encounter->moves)
        {
            move(i++, 2);
            printw("    %d) %s", move_num++, m->identifier.c_str());
        }

        // list pokemon with number selection
        move(2, map::map_x / 2 + 1);
        printw("available pokemon");
        i = 3;
        pokemon_num = 1;
        for (pokemon *p : pc.m_pokemon)
        {
            move(i++, map::map_x / 2 + 1);
            printw("  %d) %s", pokemon_num++, p->type->identifier.c_str());
        }

        move(9, map::map_x / 2 + 1);
        printw("1) PokeBalls = %d", pc.pokeballs);
        move(10, map::map_x / 2 + 1);
        printw("2) Potions = %d", pc.potions);
        move(11, map::map_x / 2 + 1);
        printw("3) Revives = %d", pc.revives);

        move(12, map::map_x / 2 + 1);
        printw("%s", pc.m_pokemon[pc.selected_pokemon]->type->identifier.c_str());
        move(13, map::map_x / 2 + 1);
        printw("  level: %d", pc.m_pokemon[pc.selected_pokemon]->level);
        move(14, map::map_x / 2 + 1);
        printw("  hp: %d/%d", pc.m_pokemon[pc.selected_pokemon]->remaining_hp, pc.m_pokemon[pc.selected_pokemon]->hp());
        move(16, map::map_x / 2 + 1);
        printw("  moves");

        move_num = 1;
        i = 17;
        for (move_t *m : pc.m_pokemon[pc.selected_pokemon]->moves)
        {
            move(i++, map::map_x / 2 + 1);
            printw("    %d) %s", move_num++, m->identifier.c_str());
        }

        move(0, 0);
        vline('|', map::map_y);

        move(0, map::map_x / 2 - 1);
        vline('|', map::map_y);

        move(0, map::map_x - 1);
        vline('|', map::map_y);

        move(0, 0);
        printw("- encounter --------------------------------------------------------------------");

        move(map::map_y - 1, 0);
        printw("--------------------------------------------------------------------------------");

        move(map::map_y, 0);
        printw("Select an action: 1) move 2) change pokemon 3) run 4) bag");

        // options are: make move, change pokemon, run
        refresh();
        ch = getchar();
        switch (ch)
        {
        case '1': // move
            if (pc.m_pokemon[pc.selected_pokemon]->remaining_hp <= 0)
            {
                bad_move = true;
                continue;
            }
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select a move.");

            refresh();
            move_ch = getchar();
            if (move_ch == '1')
            { // move 1
                player_move = pc.m_pokemon[pc.selected_pokemon]->moves[0];
            }
            else if (move_ch == '2')
            { // move 2
                if (pc.m_pokemon[pc.selected_pokemon]->moves.size() == 2)
                {
                    player_move = pc.m_pokemon[pc.selected_pokemon]->moves[1];
                }
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid move.");
                continue;
            }
            break;
        case '2': // change pokemon
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select a pokemon to switch to.");

            refresh();
            pokemon_ch = getchar();
            pokemon_index = pokemon_ch - '0';

            if (pokemon_index > 0 && pokemon_index <= pc.m_pokemon.size())
            {
                pc.selected_pokemon = pokemon_index - 1;
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid choice");
                continue;
            }
            player_move = nullptr;
            player_pokemon = pc.m_pokemon[pc.selected_pokemon];
            continue;
        case '3': // run
            run_attempts++;

            odds_escape = pc.m_pokemon[pc.selected_pokemon]->speed() * 32 / ((encounter->speed() / 4) % 256) + 30 * run_attempts;

            if (rand() % 256 < odds_escape)
            { // trainer escaped
                return;
            }
            else
            {
                last_run_failed = true;
                continue;
            }
        case '4': // bag
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Select an item to use.");

            refresh();
            bag_ch = getchar();

            if (bag_ch == '1')
            {
                if (pc.pokeballs)
                {
                    pc.pokeballs--;

                    if (pc.m_pokemon.size() < 6)
                    {
                        pc.m_pokemon.push_back(encounter);
                        caught_pokemon = true;
                        continue;
                    }
                }
            }
            else if (bag_ch == '2')
            {
                if (pc.potions)
                {
                    pc.potions--;
                    player_pokemon->remaining_hp += 20;
                    if (player_pokemon->remaining_hp > player_pokemon->hp())
                    {
                        player_pokemon->remaining_hp = player_pokemon->hp();
                    }
                }
            }
            else if (bag_ch == '3')
            {
                if (pc.revives)
                {
                    pc.revives--;

                    refresh();
                    pokemon_ch = getchar();
                    pokemon_index = pokemon_ch - '0';

                    pc.m_pokemon[pokemon_index]->remaining_hp = pc.m_pokemon[pokemon_index]->hp() / 2;
                    pokemon_index = pc.selected_pokemon;

                    move(map::map_y + 1, 0);
                    clrtoeol();
                    printw("Revived pokemon to half health.");
                }
            }
            else
            {
                move(map::map_y + 1, 0);
                clrtoeol();
                printw("Invalid choice");
                continue;
            }

            break;
        default:
            move(map::map_y + 1, 0);
            clrtoeol();
            printw("Invalid selection.");
            continue;
        }

        encounter_move = encounter->moves[rand() % encounter->moves.size()];

        // pc and pokemon have selected a move, higher priority goes first
        if (encounter_move && player_move)
        {
            if (encounter_move->priority > player_move->priority)
            { // encounter moves first
                player_moves_first = false;
            }
            else if (encounter_move->priority < player_move->priority)
            {
                player_moves_first = true;
            }
            else
            { // higher speed goes first
                if (encounter->speed() > pc.m_pokemon[pc.selected_pokemon]->speed())
                {
                    player_moves_first = false;
                }
                else if (encounter->speed() > pc.m_pokemon[pc.selected_pokemon]->speed())
                {
                    player_moves_first = true;
                }
                else
                { // choose random
                    if (rand() % 2)
                    {
                        player_moves_first = false;
                    }
                    else
                    {
                        player_moves_first = true;
                    }
                }
            }

            if (player_moves_first)
            {
                player_last_move_missed = !player_pokemon->attack_pokemon(encounter, encounter_move);
                if (encounter->remaining_hp <= 0)
                {
                    encounter->remaining_hp = 0;
                    player_pokemon_defeated_encounter = true;
                    continue;
                }
                encounter_last_move_missed = !encounter->attack_pokemon(player_pokemon, encounter_move);
                if (player_pokemon->remaining_hp <= 0)
                {
                    player_pokemon->remaining_hp = 0;
                    encounter_defeated_player_pokemon = true;
                    continue;
                }
            }
            else
            {
                encounter_last_move_missed = !encounter->attack_pokemon(player_pokemon, encounter_move);
                if (player_pokemon->remaining_hp == 0)
                {
                    encounter_defeated_player_pokemon = true;
                    continue;
                }
                player_last_move_missed = !player_pokemon->attack_pokemon(encounter, encounter_move);
                if (encounter->remaining_hp == 0)
                {
                    player_pokemon_defeated_encounter = true;
                    continue;
                }
            }
        }
        else
        {
            encounter_last_move_missed = !encounter->attack_pokemon(player_pokemon, encounter_move);
            if (player_pokemon->remaining_hp == 0)
            {
                encounter_defeated_player_pokemon = true;
                continue;
            }
        }
    }

    clear();
    if (encounter->remaining_hp == 0)
    {
        // wild pokemon lost
        move(map::map_y + 1, 0);
        printw("The wild pokemon was defeated!");
    }
    else if (!pc.can_battle())
    {
        // wild pokemon won
        move(map::map_y + 1, 0);
        printw("Your pokemon were defeated!");
    }
    else if (caught_pokemon)
    {
        // wild pokemon won
        move(map::map_y + 1, 0);
        printw("You caught the wild %s!", encounter->type->identifier.c_str());
    }
}

void display_bag(world &w)
{
    move(0, 0);
    printw("- bag --------------------------------------------------------------------------");

    move(3, 0);
    printw("-) PokeBalls = %d", w.pc.pokeballs);
    move(4, 0);
    printw("1) Potions   = %d", w.pc.potions);
    move(5, 0);
    printw("2) Revives   = %d", w.pc.revives);
    
    move(2, map::map_x / 4 + 1);
    printw("available pokemon");

    int i = 3;
    int pokemon_num = 1;
    for (pokemon *p : w.pc.m_pokemon)
    {
        move(i++, map::map_x / 4 + 1);
        printw("  %d) %s - hp: %d", pokemon_num++, p->type->identifier.c_str(), p->remaining_hp);
    }

    move(map::map_y - 1, 0);
    printw("--------------------------------------------------------------------------------");

    move(map::map_y, 0);
    printw("Choose an item, then a pokemon to use it on.");
    refresh();

    char item = getchar();
    char pokemon_ch;
    int pokemon_index;

    if (item == '1')
    {
        // potion
        pokemon_ch = getchar();
        pokemon_index = pokemon_ch - '0';

        if (pokemon_index > 0 && pokemon_index <= w.pc.m_pokemon.size())
        {
            w.pc.m_pokemon[pokemon_index - 1]->remaining_hp = w.pc.m_pokemon[pokemon_index - 1]->hp();
            if (w.pc.m_pokemon[pokemon_index - 1]->remaining_hp > w.pc.m_pokemon[pokemon_index - 1]->hp())
            {
                w.pc.m_pokemon[pokemon_index - 1]->remaining_hp = w.pc.m_pokemon[pokemon_index - 1]->hp();
            }
        }
    }
    else if (item == '2')
    {
        // revive
        pokemon_ch = getchar();
        pokemon_index = pokemon_ch - '0';

        if (pokemon_index > 0 && pokemon_index <= w.pc.m_pokemon.size() && w.pc.m_pokemon[pokemon_index - 1]->remaining_hp == 0)
        {
            w.pc.m_pokemon[pokemon_index - 1]->remaining_hp = w.pc.m_pokemon[pokemon_index - 1]->hp() / 2;
        }
    }
    
    i = 3;
    pokemon_num = 1;
    for (pokemon *p : w.pc.m_pokemon)
    {
        move(i++, map::map_x / 4 + 1);
        printw("  %d) %s - hp: %d", pokemon_num++, p->type->identifier.c_str(), p->remaining_hp);
    }

    refresh();
    sleep(2);
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
            clear();
            return starter_list[0];
        case '2':
            clear();
            return starter_list[1];
        case '3':
            clear();
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
    pokemon *pc_starting_pokemon = new pokemon(w.pdex, starting_pokemon);
    w.pc.m_pokemon.push_back(pc_starting_pokemon);

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
            for (pokemon *p : w.pc.m_pokemon)
            {
                p->remaining_hp = p->hp();
            }

            move(map::map_y + 1, 0);
            printw("Healed pokemon!");
            refresh();
            w.display_menu = menu_map;

            break;
        case menu_pokemart:
            w.pc.pokeballs = 2;
            w.pc.potions = 2;
            w.pc.revives = 2;

            move(map::map_y + 1, 0);
            printw("Restored player items!");
            refresh();
            w.display_menu = menu_map;

            break;
        case menu_battle:
            clear();
            display_battle(w.battling, w.pc);

            w.display_menu = menu_map;

            w.print_map();
            refresh();

            break;
        case menu_encounter:
            clear();
            display_encounter(w.encounter, w.pc);

            w.display_menu = menu_map;

            w.print_map();
            refresh();

            break;
        case menu_bag:
            clear();
            display_bag(w);
            clear();

            w.display_menu = menu_map;

            w.print_map();
            refresh();
            break;
        }
    }

    end_ncurses();

    return 0;
}
