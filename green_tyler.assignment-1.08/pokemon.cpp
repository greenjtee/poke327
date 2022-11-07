#include "pokemon.hpp"

pokemon::pokemon() {
    this->type = nullptr;
    this->level = 0;

    this->hp = rand() % 16;
    this->attack = rand() % 16;
    this->defense = rand() % 16;
    this->speed = rand() % 16;
    this->special_attack = rand() % 16;
    this->special_defense = rand() % 16;

    this->base_hp = 0;
    this->base_attack = 0;
    this->base_defense = 0;
    this->base_speed = 0;
    this->base_special_attack = 0;
    this->base_special_defense = 0;

    this->gender = gender_t(rand() % 2);

    this->shiny = bool(rand() % 8192 == 0);
}

void pokemon::randomize_stats() {
    this->hp = rand() % 16;
    this->attack = rand() % 16;
    this->defense = rand() % 16;
    this->speed = rand() % 16;
    this->special_attack = rand() % 16;
    this->special_defense = rand() % 16;

    this->gender = gender_t(rand() % 2);

    this->shiny = bool(rand() % 8192 == 0);
}

void pokemon::generate_base_stats() {
    for (pokemon_stat_t* ps : this->stats) {
        switch (ps->stat_id) {
            case stat_hp:
                this->base_hp = ps->base_stat;
                break;
            case stat_attack:
                this->base_attack = ps->base_stat;
                break;
            case stat_defense:
                this->base_defense = ps->base_stat;
                break;
            case stat_special_attack:
                this->base_special_attack = ps->base_stat;
                break;
            case stat_special_defense:
                this->base_special_defense = ps->base_stat;
                break;
            case stat_speed:
                this->base_speed = ps->base_stat;
        }
    }
}

void pokemon::level_up() {
    this->hp                = ((base_hp + hp) * 2)              * level / 100 + level + 10;
    this->attack            = ((base_attack + attack) * 2)          * level / 100 + 5;
    this->defense           = ((base_defense + defense) * 2)         * level / 100 + 5;
    this->speed             = ((base_speed + speed) * 2)           * level / 100 + 5;
    this->special_attack    = ((base_special_attack + special_attack) * 2)  * level / 100 + 5;
    this->special_defense   = ((base_special_defense + special_defense) * 2) * level / 100 + 5;
}

pokemon::~pokemon() {
}
