#ifndef POKEDEX_HPP
#define POKEDEX_HPP

#include <cinttypes>
#include <string>
#include <vector>
#include <cstdio>

typedef struct pokemon
{
    int32_t id;
    std::string identifier;
    int32_t species_id;
    int32_t height;
    int32_t weight;
    int32_t base_experience;
    int32_t order;
    int32_t is_default;
} pokemon_t;

typedef struct move
{
    int32_t id;
    std::string identifier;
    int32_t generation_id;
    int32_t type_id;
    int32_t power;
    int32_t pp;
    int32_t accuracy;
    int32_t priority;
    int32_t target_id;
    int32_t damage_class_id;
    int32_t effect_id;
    int32_t effect_chance;
    int32_t contest_type_id;
    int32_t contest_effect_id;
    int32_t super_contest_effect_id;
} move_t;

typedef struct pokemon_move
{
    int32_t pokemon_id;
    int32_t version_group_id;
    int32_t move_id;
    int32_t pokemon_move_method_id;
    int32_t level;
    int32_t order;
} pokemon_move_t;

typedef struct pokemon_species
{
    int32_t id;
    std::string identifier;
    int32_t generation_id;
    int32_t evolves_from_species_id;
    int32_t evolution_chain_id;
    int32_t color_id;
    int32_t shape_id;
    int32_t habitat_id;
    int32_t gender_rate;
    int32_t capture_rate;
    int32_t base_happiness;
    int32_t is_baby;
    int32_t hatch_counter;
    int32_t has_gender_differences;
    int32_t growth_rate_id;
    int32_t forms_switchable;
    int32_t is_legendary;
    int32_t is_mythical;
    int32_t order;
    int32_t conquest_order;
} pokemon_species_t;

typedef struct experience
{
    int32_t growth_rate_id;
    int32_t level;
    int32_t experience;
} experience_t;

typedef struct type_name
{
    int32_t type_id;
    int32_t local_language_id;
    std::string name;
} type_name_t;

typedef struct pokemon_stat
{
    int32_t pokemon_id;
    int32_t stat_id;
    int32_t base_stat;
    int32_t effort;
} pokemon_stat_t;

typedef struct stat
{
    int32_t id;
    int32_t damage_class_id;
    std::string identifier;
    int32_t is_battle_only;
    int32_t game_index;
} stat_t;

typedef struct pokemon_type
{
    int32_t pokemon_id;
    int32_t type_id;
    int32_t slot;
} pokemon_type_t;

class pokedex
{

private:
    std::vector<pokemon_t *> m_pokemon;
    std::vector<move_t *> m_moves;
    std::vector<pokemon_move_t *> m_pokemon_moves;
    std::vector<pokemon_species_t *> m_pokemon_species;
    std::vector<experience_t *> m_experience;
    std::vector<type_name_t *> m_type_names;
    std::vector<pokemon_stat_t *> m_pokemon_stats;
    std::vector<stat_t *> m_stats;
    std::vector<pokemon_type_t *> m_pokemon_types;

public:
    pokedex(std::vector<std::string> &paths);

    // returns true if success, false otherwise
    bool parse_pokemon(std::string &filename);
    bool parse_moves(std::string &filename);
    bool parse_pokemon_moves(std::string &filename);
    bool parse_pokemon_species(std::string &filename);
    bool parse_experience(std::string &filename);
    bool parse_type_names(std::string &filename);
    bool parse_pokemon_stats(std::string &filename);
    bool parse_stats(std::string &filename);
    bool parse_pokemon_types(std::string &filename);

    void print_pokemon();
    void print_moves();
    void print_pokemon_moves();
    void print_pokemon_species();
    void print_experience();
    void print_type_names();
    void print_pokemon_stats();
    void print_stats();
    void print_pokemon_types();

    ~pokedex();
};

#endif