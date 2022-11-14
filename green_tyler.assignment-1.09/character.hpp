#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include "utility.hpp"
#include "pokemon.hpp"

const int max_pokemon = 6;

class character {
public:
    pair_t pos, velocity;
    uint32_t next_move;
    std::vector<pokemon*> m_pokemon;
    size_t selected_pokemon;

    virtual bool can_battle() {
        for (pokemon* p : m_pokemon) {
            if (p->remaining_hp > 0) {
                return true;
            }
        }

        return false;
    }

    virtual ~character() {
        for (pokemon* p : m_pokemon) {
            delete p;
        }
    }
};

#endif
