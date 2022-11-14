#include "pokemon.hpp"

#include <random>
#include <climits>

std::default_random_engine generator;

pokemon::pokemon(pokedex &pdex, pokemon_t *type)
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

    this->type = type;

    pdex.get_pokemon_moves(this);
    pdex.get_pokemon_species(this);
    pdex.get_pokemon_stats(this);

    this->generate_base_stats();
    this->remaining_hp = hp();
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

bool pokemon::attack_pokemon(pokemon *p, move_t *m)
{
    std::uniform_int_distribution<int> distribution(85, 100);
    int rando = distribution(generator);
    int critical = 1;
    int stab = 1;
    int type = 1;

    if (m->power == INT_MAX) { // move does not do damage
        return true;
    }

    if (rand() % 255 < (this->base_speed / 2)) {
        critical = 1.5;
    }

    if (rand() % 100 < m->accuracy)
    { // hits
        p->remaining_hp -= (((2 * this->level / 5 + 2) * m->power * this->attack() / p->defense() / 50) * critical * rando * stab * type);
        if (p->remaining_hp < 0) {
            p->remaining_hp = 0;
        }

        return true;
    }
    else
    { // misses
        return false;
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
