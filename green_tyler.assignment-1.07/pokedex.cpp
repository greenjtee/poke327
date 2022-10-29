#include "pokedex.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

void convert_and_store(uint32_t &a, std::string &s)
{
    if (s.size())
        a = stoi(s);
    else
        a = INT_MAX;
}

void better_print(uint32_t a) {
    if (a != INT_MAX) {
        printf("%5d ", a);
    } else {
        printf("      ");
    }
}

pokedex::pokedex()
{
}

// id,identifier,species_id,height,weight,base_experience,order,is_default
bool pokedex::parse_pokemon(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    pokemon_t *p;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (std::getline(file_stream, line))
    {
        p = new pokemon_t;
        std::stringstream ss(line);

        std::string id;
        std::string identifier;
        std::string species_id;
        std::string height;
        std::string weight;
        std::string base_experience;
        std::string order;
        std::string is_default;

        // id
        std::getline(ss, id, ',');
        std::getline(ss, identifier, ',');
        std::getline(ss, species_id, ',');
        std::getline(ss, height, ',');
        std::getline(ss, weight, ',');
        std::getline(ss, base_experience, ',');
        std::getline(ss, order, ',');
        std::getline(ss, is_default, ',');

        convert_and_store(p->id, id);
        p->identifier = identifier;
        convert_and_store(p->species_id, species_id);
        convert_and_store(p->height, height);
        convert_and_store(p->weight, weight);
        convert_and_store(p->base_experience, base_experience);
        convert_and_store(p->order, order);
        convert_and_store(p->is_default, is_default);

        this->pokemon.push_back(p);
    }

    return true;
}

void pokedex::print_pokemon()
{
    for (pokemon_t *p : pokemon)
    {
        better_print(p->id);
        printf("%-30s", p->identifier.c_str());
        better_print(p->species_id);
        better_print(p->height);
        better_print(p->weight);
        better_print(p->base_experience);
        better_print(p->order);
        better_print(p->is_default);
        printf("\n");
    }
}

// id,identifier,generation_id,type_id,power,pp,accuracy,priority,target_id,damage_class_id,effect_id,effect_chance,contest_type_id,contest_effect_id,super_contest_effect_id
bool pokedex::parse_moves(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    move_t *m;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (std::getline(file_stream, line))
    {
        m = new move_t;
        std::stringstream ss(line);

        std::string id;
        std::string identifier;
        std::string generation_id;
        std::string type_id;
        std::string power;
        std::string pp;
        std::string accuracy;
        std::string priority;
        std::string target_id;
        std::string damage_class_id;
        std::string effect_id;
        std::string effect_chance;
        std::string contest_type_id;
        std::string contest_effect_id;
        std::string super_contest_effect_id;

        // id
        std::getline(ss, id, ',');
        std::getline(ss, identifier, ',');
        std::getline(ss, generation_id, ',');
        std::getline(ss, type_id, ',');
        std::getline(ss, power, ',');
        std::getline(ss, pp, ',');
        std::getline(ss, accuracy, ',');
        std::getline(ss, target_id, ',');
        std::getline(ss, damage_class_id, ',');
        std::getline(ss, effect_id, ',');
        std::getline(ss, effect_chance, ',');
        std::getline(ss, contest_type_id, ',');
        std::getline(ss, contest_effect_id, ',');
        std::getline(ss, super_contest_effect_id, ',');

        convert_and_store(m->id, id);
        m->identifier = identifier;
        convert_and_store(m->generation_id, generation_id);
        convert_and_store(m->type_id, type_id);
        convert_and_store(m->power, power);
        convert_and_store(m->pp, pp);
        convert_and_store(m->accuracy, accuracy);
        convert_and_store(m->target_id, target_id);
        convert_and_store(m->damage_class_id, damage_class_id);
        convert_and_store(m->effect_id, effect_id);
        convert_and_store(m->effect_chance, effect_chance);
        convert_and_store(m->contest_type_id, contest_type_id);
        convert_and_store(m->contest_effect_id, contest_effect_id);
        convert_and_store(m->super_contest_effect_id, super_contest_effect_id);

        this->moves.push_back(m);
    }

    return true;
}

void pokedex::print_moves()
{
    for (move_t *m : moves)
    {
        better_print(m->id);
        printf("%-40s", m->identifier.c_str());
        better_print(m->generation_id);
        better_print(m->type_id);
        better_print(m->power);
        better_print(m->pp);
        better_print(m->accuracy);
        better_print(m->target_id);
        better_print(m->damage_class_id);
        better_print(m->effect_id);
        better_print(m->effect_chance);
        better_print(m->contest_type_id);
        better_print(m->contest_effect_id);
        better_print(m->super_contest_effect_id);
        printf("\n");
    }
}

bool pokedex::parse_pokemon_moves(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    pokemon_move_t *m;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (std::getline(file_stream, line))
    {
        m = new pokemon_move_t;
        std::stringstream ss(line);

        std::string pokemon_id;
        std::string version_group_id;
        std::string move_id;
        std::string pokemon_move_method_id;
        std::string level;
        std::string order;

        // id
        std::getline(ss, pokemon_id, ',');
        std::getline(ss, version_group_id, ',');
        std::getline(ss, move_id, ',');
        std::getline(ss, pokemon_move_method_id, ',');
        std::getline(ss, level, ',');
        std::getline(ss, order, ',');


        convert_and_store(m->pokemon_id, pokemon_id);
        convert_and_store(m->version_group_id, version_group_id);
        convert_and_store(m->move_id, move_id);
        convert_and_store(m->pokemon_move_method_id, pokemon_move_method_id);
        convert_and_store(m->level, level);
        convert_and_store(m->order, order);

        this->pokemon_moves.push_back(m);
    }

    return true;
}

void pokedex::print_pokemon_moves()
{
    for (pokemon_move_t *m : pokemon_moves)
    {
        better_print(m->pokemon_id);
        better_print(m->version_group_id);
        better_print(m->move_id);
        better_print(m->pokemon_move_method_id);
        better_print(m->level);
        better_print(m->order);
        printf("\n");
    }
}

bool pokedex::parse_pokemon_species(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool pokedex::parse_experience(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool pokedex::parse_type_names(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool pokedex::parse_pokemon_stats(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool pokedex::parse_stats(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

bool pokedex::parse_pokemon_types(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;

    if (!file_stream.is_open())
    {
        return false;
    }

    while (getline(file_stream, line))
    {
        std::cout << line << std::endl;
    }

    return true;
}

pokedex::~pokedex()
{
    for (pokemon_t *p : pokemon)
    {
        delete p;
    }

    for (move_t *m : moves)
    {
        delete m;
    }
}