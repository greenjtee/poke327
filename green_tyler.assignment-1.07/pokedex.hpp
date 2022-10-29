#ifndef POKEDEX_HPP
#define POKEDEX_HPP

#include <cinttypes>
#include <string>
#include <vector>
#include <cstdio>

typedef struct pokemon {
    uint32_t id;
    std::string identifier;
    uint32_t species_id;
    uint32_t height;
    uint32_t weight;
    uint32_t base_experience;
    uint32_t order;
    uint32_t is_default;
} pokemon_t;

typedef struct move {
    uint32_t id;
    std::string identifier;
    uint32_t generation_id;
    uint32_t type_id;
    uint32_t power;
    uint32_t pp;
    uint32_t accuracy;
    uint32_t priority;
    uint32_t target_id;
    uint32_t damage_class_id;
    uint32_t effect_id;
    uint32_t effect_chance;
    uint32_t contest_type_id;
    uint32_t contest_effect_id;
    uint32_t super_contest_effect_id;
} move_t;

typedef struct pokemon_move {
    uint32_t pokemon_id;
    uint32_t version_group_id;
    uint32_t move_id;
    uint32_t pokemon_move_method_id;
    uint32_t level;
    uint32_t order;
} pokemon_move_t;

typedef struct pokemon_species {
    uint32_t id;
    std::string identifier;
    uint32_t generation_id;
    uint32_t evolves_from_species_id;
    uint32_t evolution_chain_id;
    uint32_t color_id;
    uint32_t shape_id;
    uint32_t habitat_id;
    uint32_t gender_rate;
    uint32_t capture_rate;
    uint32_t base_happiness;
    uint32_t is_baby;
    uint32_t hatch_counter;
    uint32_t has_gender_differences;
    uint32_t growth_rate_id;
    uint32_t forms_switchable;
    uint32_t is_legendary;
    uint32_t is_mythical;
    uint32_t order;
    uint32_t conquest_order;
} pokemon_species_t;

typedef struct experience {
    uint32_t growth_rate_id;
    uint32_t level;
    uint32_t experience;
} experience_t;

typedef struct type_name {
    uint32_t type_id;
    uint32_t local_language_id;
    std::string name;
} type_name_t;

typedef struct pokemon_stat {
    uint32_t pokemon_id;
    uint32_t stat_id;
    uint32_t base_stat;
    uint32_t effort;
} pokemon_stat_t;

typedef struct stat {
 uint32_t id;
 uint32_t damage_class_id;
 std::string identifier;
 uint32_t is_battle_only;
 uint32_t game_index;
} stat_t;

typedef struct pokemon_type {
    uint32_t pokemon_id;
    uint32_t type_id;
    uint32_t slot;
} pokemon_type_t;

class pokedex {

private:
    std::vector<pokemon_t*> pokemon;
    std::vector<move_t*> moves;
    std::vector<pokemon_move_t*> pokemon_moves;
    std::vector<pokemon_species_t*> pokemon_species;
    std::vector<experience_t*> experience;
    std::vector<type_name_t*> type_names;
    std::vector<pokemon_stat_t*> pokemon_stats;
    std::vector<stat_t*> stats;
    std::vector<pokemon_type_t*> pokemon_types;

public:
    pokedex();

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