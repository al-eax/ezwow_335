#pragma once

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "grind_bot/combat.hpp"
#include "cache.hpp"
#include "wow/navigation.hpp"


class recover_state : public state{

public:
    recover_state() : state("recover_state"){}

    void update(){
        float health_relative = cache::local_player.health / cache::local_player.max_health;
        float mana_relative = cache::local_player.mana / cache::local_player.max_mana;
        
        if( health_relative > 0.8 && mana_relative > 0.8 ){
            change_state("grind_units_state");
        }
    }
};