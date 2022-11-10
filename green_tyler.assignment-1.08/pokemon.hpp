#ifndef POKEMON_HPP
#define POKEMON_HPP

#include "pokedex.hpp"

typedef enum gender {
    gender_male,
    gender_female
} gender_t;

typedef enum stat_enum {
    stat_hp = 1,
    stat_attack,
    stat_defense,
    stat_special_attack,
    stat_special_defense,
    stat_speed,
    stat_accuracy,
    stat_evasion
} stat_enum_t;

class pokemon {
public:
    pokemon();
    void level_up();
    void generate_base_stats();
    void randomize_stats();

    ~pokemon();

    pokemon_t* type;
    pokemon_species_t* species;

    int32_t level;
    int32_t hp, attack, defense, speed, special_attack, special_defense;
    int32_t base_hp, base_attack, base_defense, base_speed, base_special_attack, base_special_defense;
    gender_t gender;

    std::vector<move_t *> moves;
    std::vector<pokemon_stat_t *> stats;

    bool shiny;
};

#endif
