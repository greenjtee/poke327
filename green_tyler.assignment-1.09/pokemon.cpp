#include "pokemon.hpp"

pokemon::pokemon()
{
    this->type = nullptr;
    this->level = 1;

    this->hp_iv = rand() % 16;
    this->attack_iv = rand() % 16;
    this->defense_iv = rand() % 16;
    this->speed_iv = rand() % 16;
    this->special_attack_iv = rand() % 16;
    this->special_defense_iv = rand() % 16;

    this->gender = gender_t(rand() % 2);

    this->shiny = bool(rand() % 8192 == 0);

    this->base_hp = 0;
    this->base_attack = 0;
    this->base_defense = 0;
    this->base_speed = 0;
    this->base_special_attack = 0;
    this->base_special_defense = 0;
}

void pokemon::randomize_stats()
{
    this->hp_iv = rand() % 16;
    this->attack_iv = rand() % 16;
    this->defense_iv = rand() % 16;
    this->speed_iv = rand() % 16;
    this->special_attack_iv = rand() % 16;
    this->special_defense_iv = rand() % 16;

    this->gender = gender_t(rand() % 2);

    this->shiny = bool(rand() % 8192 == 0);
}

void pokemon::generate_base_stats()
{
    for (pokemon_stat_t *ps : this->stats)
    {
        switch (ps->stat_id)
        {
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

int32_t pokemon::hp()
{
    return ((base_hp + hp_iv) * 2) * level / 100 + level + 10;
}

int32_t pokemon::attack()
{
    return ((base_attack + attack_iv) * 2) * level / 100 + 5;
}

int32_t pokemon::defense()
{
    return ((base_defense + defense_iv) * 2) * level / 100 + 5;
}

int32_t pokemon::speed()
{
    return ((base_speed + speed_iv) * 2) * level / 100 + 5;
}

int32_t pokemon::special_attack()
{
    return ((base_special_attack + special_attack_iv) * 2) * level / 100 + 5;
}

int32_t pokemon::special_defense()
{
    return ((base_special_defense + special_defense_iv) * 2) * level / 100 + 5;
}

pokemon::~pokemon()
{
}
