#include "pokedex.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>

void convert_and_store(int32_t &a, std::string &s)
{
    if (s.size()) {
        a = stoi(s);
    }
    else
    {
        a = INT_MAX;
    }
}

void better_print(int32_t a)
{
    if (a != INT_MAX)
    {
        printf("%d", a);
    }
}

pokedex::pokedex(std::vector<std::string> &paths)
{
    for (auto path : paths) {
        parse_pokemon(std::string(path).append("/pokemon.csv"));
        if (this->pokemon.size() == 0) {
            continue;
        }
        parse_moves(std::string(path).append("/moves.csv"));
        parse_pokemon_moves(std::string(path).append("/pokemon_moves.csv"));
        parse_pokemon_species(std::string(path).append("/pokemon_species.csv"));
        parse_experience(std::string(path).append("/experience.csv"));
        parse_type_names(std::string(path).append("/type_names.csv"));
        parse_pokemon_stats(std::string(path).append("/pokemon_stats.csv"));
        parse_stats(std::string(path).append("/stats.csv"));
        parse_pokemon_types(std::string(path).append("/pokemon_types.csv"));
    }

    if (this->pokemon.size() == 0) {
        std::cout << "No valid paths given" << std::endl;
    }
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
        printf(",");
        printf("%s", p->identifier.c_str());
        printf(",");
        better_print(p->species_id);
        printf(",");
        better_print(p->height);
        printf(",");
        better_print(p->weight);
        printf(",");
        better_print(p->base_experience);
        printf(",");
        better_print(p->order);
        printf(",");
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
        std::getline(ss, priority, ',');
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
        convert_and_store(m->priority, priority);
        convert_and_store(m->accuracy, accuracy);
        convert_and_store(m->target_id, target_id);
        convert_and_store(m->damage_class_id, damage_class_id);
        convert_and_store(m->effect_id, effect_id);
        convert_and_store(m->effect_chance, effect_chance);
        convert_and_store(m->contest_type_id, contest_type_id);
        convert_and_store(m->contest_effect_id, contest_effect_id);
        convert_and_store(m->super_contest_effect_id, super_contest_effect_id);

        this->m_moves.push_back(m);
    }

    return true;
}

void pokedex::print_moves()
{
    for (move_t *m : m_moves)
    {
        better_print(m->id);
        printf(",");
        printf("%s", m->identifier.c_str());
        printf(",");
        better_print(m->generation_id);
        printf(",");
        better_print(m->type_id);
        printf(",");
        better_print(m->power);
        printf(",");
        better_print(m->pp);
        printf(",");
        better_print(m->priority);
        printf(",");
        better_print(m->accuracy);
        printf(",");
        better_print(m->target_id);
        printf(",");
        better_print(m->damage_class_id);
        printf(",");
        better_print(m->effect_id);
        printf(",");
        better_print(m->effect_chance);
        printf(",");
        better_print(m->contest_type_id);
        printf(",");
        better_print(m->contest_effect_id);
        printf(",");
        better_print(m->super_contest_effect_id);
        printf("\n");
    }
}

// pokemon_id,version_group_id,move_id,pokemon_move_method_id,level,order
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

        this->m_pokemon_moves.push_back(m);
    }

    return true;
}

void pokedex::print_pokemon_moves()
{
    for (pokemon_move_t *m : m_pokemon_moves)
    {
        better_print(m->pokemon_id);
        printf(",");
        better_print(m->version_group_id);
        printf(",");
        better_print(m->move_id);
        printf(",");
        better_print(m->pokemon_move_method_id);
        printf(",");
        better_print(m->level);
        printf(",");
        better_print(m->order);
        printf("\n");
    }
}

// id,identifier,generation_id,evolves_from_species_id,evolution_chain_id,color_id,shape_id,habitat_id,gender_rate,capture_rate,base_happiness,
// is_baby,hatch_counter,has_gender_differences,growth_rate_id,forms_switchable,is_legendary,is_mythical,order,conquest_order
bool pokedex::parse_pokemon_species(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    pokemon_species_t *s;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        s = new pokemon_species_t;
        std::stringstream ss(line);
        std::string id, identifier, generation_id, evolves_from_species_id, evolution_chain_id,
            color_id, shape_id, habitat_id, gender_rate, capture_rate, base_happiness,
            is_baby, hatch_counter, has_gender_differences, growth_rate_id, forms_switchable,
            is_legendary, is_mythical, order, conquest_order;

        std::getline(ss, id, ',');
        std::getline(ss, identifier, ',');
        std::getline(ss, generation_id, ',');
        std::getline(ss, evolves_from_species_id, ',');
        std::getline(ss, evolution_chain_id, ',');
        std::getline(ss, color_id, ',');
        std::getline(ss, shape_id, ',');
        std::getline(ss, habitat_id, ',');
        std::getline(ss, gender_rate, ',');
        std::getline(ss, capture_rate, ',');
        std::getline(ss, base_happiness, ',');
        std::getline(ss, is_baby, ',');
        std::getline(ss, hatch_counter, ',');
        std::getline(ss, has_gender_differences, ',');
        std::getline(ss, growth_rate_id, ',');
        std::getline(ss, forms_switchable, ',');
        std::getline(ss, is_legendary, ',');
        std::getline(ss, is_mythical, ',');
        std::getline(ss, order, ',');
        std::getline(ss, conquest_order, ',');

        convert_and_store(s->id, id);
        s->identifier = identifier;
        convert_and_store(s->generation_id, generation_id);
        convert_and_store(s->evolves_from_species_id, evolves_from_species_id);
        convert_and_store(s->evolution_chain_id, evolution_chain_id);
        convert_and_store(s->color_id, color_id);
        convert_and_store(s->shape_id, shape_id);
        convert_and_store(s->habitat_id, habitat_id);
        convert_and_store(s->gender_rate, gender_rate);
        convert_and_store(s->capture_rate, capture_rate);
        convert_and_store(s->base_happiness, base_happiness);
        convert_and_store(s->is_baby, is_baby);
        convert_and_store(s->hatch_counter, hatch_counter);
        convert_and_store(s->has_gender_differences, has_gender_differences);
        convert_and_store(s->growth_rate_id, growth_rate_id);
        convert_and_store(s->forms_switchable, forms_switchable);
        convert_and_store(s->is_legendary, is_legendary);
        convert_and_store(s->is_mythical, is_mythical);
        convert_and_store(s->order, order);
        convert_and_store(s->conquest_order, conquest_order);

        this->m_pokemon_species.push_back(s);
    }

    return true;
}

void pokedex::print_pokemon_species()
{
    for (pokemon_species_t *s : m_pokemon_species)
    {
        better_print(s->id);
        printf(",");
        printf("%s", s->identifier.c_str());
        printf(",");
        better_print(s->generation_id);
        printf(",");
        better_print(s->evolves_from_species_id);
        printf(",");
        better_print(s->evolution_chain_id);
        printf(",");
        better_print(s->color_id);
        printf(",");
        better_print(s->shape_id);
        printf(",");
        better_print(s->habitat_id);
        printf(",");
        better_print(s->gender_rate);
        printf(",");
        better_print(s->capture_rate);
        printf(",");
        better_print(s->base_happiness);
        printf(",");
        better_print(s->is_baby);
        printf(",");
        better_print(s->hatch_counter);
        printf(",");
        better_print(s->has_gender_differences);
        printf(",");
        better_print(s->growth_rate_id);
        printf(",");
        better_print(s->forms_switchable);
        printf(",");
        better_print(s->is_legendary);
        printf(",");
        better_print(s->is_mythical);
        printf(",");
        better_print(s->order);
        printf(",");
        better_print(s->conquest_order);
        printf("\n");
    }
}

bool pokedex::parse_experience(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    experience_t *e;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        e = new experience_t;
        std::stringstream ss(line);
        std::string growth_rate_id, level, experience;

        std::getline(ss, growth_rate_id, ',');
        std::getline(ss, level, ',');
        std::getline(ss, experience, ',');

        convert_and_store(e->growth_rate_id, growth_rate_id);
        convert_and_store(e->level, level);
        convert_and_store(e->experience, experience);

        this->m_experience.push_back(e);
    }

    return true;
}

void pokedex::print_experience()
{
    for (experience_t *e : m_experience)
    {
        better_print(e->growth_rate_id);
        printf(",");
        better_print(e->level);
        printf(",");
        better_print(e->experience);
        printf("\n");
    }
}

bool pokedex::parse_type_names(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    type_name_t *t;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        t = new type_name_t;
        std::stringstream ss(line);
        std::string type_id, local_language_id, name;

        std::getline(ss, type_id, ',');
        std::getline(ss, local_language_id, ',');
        std::getline(ss, name, ',');

        convert_and_store(t->type_id, type_id);
        convert_and_store(t->local_language_id, local_language_id);
        t->name = name;

        this->m_type_names.push_back(t);
    }

    return true;
}

void pokedex::print_type_names()
{
    for (type_name_t *t : m_type_names)
    {
        better_print(t->type_id);
        printf(",");
        better_print(t->local_language_id);
        printf(",");
        printf("%s", t->name.c_str());
        printf("\n");
    }
}

bool pokedex::parse_pokemon_stats(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    pokemon_stat_t *s;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        s = new pokemon_stat_t;
        std::stringstream ss(line);
        std::string pokemon_id, stat_id, base_stat, effort;

        std::getline(ss, pokemon_id, ',');
        std::getline(ss, stat_id, ',');
        std::getline(ss, base_stat, ',');
        std::getline(ss, effort, ',');

        convert_and_store(s->pokemon_id, pokemon_id);
        convert_and_store(s->stat_id, stat_id);
        convert_and_store(s->base_stat, base_stat);
        convert_and_store(s->effort, effort);

        this->m_pokemon_stats.push_back(s);
    }

    return true;
}

void pokedex::print_pokemon_stats()
{
    for (pokemon_stat_t *s : m_pokemon_stats)
    {
        better_print(s->pokemon_id);
        printf(",");
        better_print(s->stat_id);
        printf(",");
        better_print(s->base_stat);
        printf(",");
        better_print(s->effort);
        printf("\n");
    }
}

bool pokedex::parse_stats(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    stat_t *s;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        s = new stat_t;
        std::stringstream ss(line);
        std::string id, damage_class_id, identifier, is_battle_only, game_index;

        std::getline(ss, id, ',');
        std::getline(ss, damage_class_id, ',');
        std::getline(ss, identifier, ',');
        std::getline(ss, is_battle_only, ',');
        std::getline(ss, game_index, ',');

        convert_and_store(s->id, id);
        convert_and_store(s->damage_class_id, damage_class_id);
        s->identifier = identifier;
        convert_and_store(s->is_battle_only, is_battle_only);
        convert_and_store(s->game_index, game_index);

        this->m_stats.push_back(s);
    }

    return true;
}

void pokedex::print_stats()
{
    for (stat_t *s : m_stats)
    {
        better_print(s->id);
        printf(",");
        better_print(s->damage_class_id);
        printf(",");
        printf("%s,", s->identifier.c_str());
        better_print(s->is_battle_only);
        printf(",");
        better_print(s->game_index);
        printf("\n");
    }
}

bool pokedex::parse_pokemon_types(std::string &filename)
{
    std::ifstream file_stream(filename);
    std::string line;
    pokemon_type_t *t;

    if (!file_stream.is_open())
    {
        return false;
    }

    // skip header
    std::getline(file_stream, line);

    while (getline(file_stream, line))
    {
        t = new pokemon_type_t;
        std::stringstream ss(line);
        std::string pokemon_id, type_id, slot;

        std::getline(ss, pokemon_id, ',');
        std::getline(ss, type_id, ',');
        std::getline(ss, slot, ',');

        convert_and_store(t->pokemon_id, pokemon_id);
        convert_and_store(t->type_id, type_id);
        convert_and_store(t->slot, slot);

        this->m_pokemon_types.push_back(t);
    }

    return true;
}

void pokedex::print_pokemon_types()
{
    for (pokemon_type_t *t : m_pokemon_types)
    {
        better_print(t->pokemon_id);
        printf(",");
        better_print(t->type_id);
        printf(",");
        better_print(t->slot);
        printf("\n");
    }
}

pokedex::~pokedex()
{
    for (pokemon_t *p : pokemon)
    {
        delete p;
    }

    for (move_t *m : m_moves)
    {
        delete m;
    }

    for (pokemon_move_t *m : m_pokemon_moves)
    {
        delete m;
    }

    for (pokemon_species_t *s : m_pokemon_species)
    {
        delete s;
    }

    for (experience_t *e : m_experience)
    {
        delete e;
    }

    for (type_name_t *t : m_type_names)
    {
        delete t;
    }

    for (pokemon_stat_t *s : m_pokemon_stats)
    {
        delete s;
    }

    for (stat_t *s : m_stats)
    {
        delete s;
    }

    for (pokemon_type_t *t : m_pokemon_types)
    {
        delete t;
    }
}