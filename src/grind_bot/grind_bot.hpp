#pragma once

#include "wow/local_player.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/wow_client.hpp"
#include "state_scheduler.hpp"
#include "wow/navigation.hpp"
#include "grind_bot/combat.hpp"

#include "states/grind_units_state.hpp"
#include "states/infight_state.hpp"
#include "states/recover_state.hpp"
#include "states/player_dead_state.hpp"
#include "states/loot_state.hpp"
#include "states/sell_items_state.hpp"

#include <iostream>
#include <functional>
#include <chrono>


struct grind_bot_config{
    item_rarity sell_rarity = item_rarity_white;
    Vector3d grind_location;
    float grind_radius;
    Vector3d village_location;
};

struct grind_bot_scheduler : state_scheduler
{
    grind_bot_scheduler()
    {
        load_states({
            new grind_units_state(),
            new infight_state(),
            new loot_state(),
            new recover_state(),
            new dead_player_state(),
            new sell_items_state(),
        });
    }

    void schedule() override
    {
        state_scheduler::schedule();
        if (this->current_state == "")
            return;
        if (cache::local_player.health == 0 || cache::local_player.is_ghost)
        {
            state_map[current_state]->next_state = "dead_player_state";
        }
        else if(cache::local_player.is_in_combat){
            state_map[current_state]->next_state = "infight_state";
        }

        sleep(0.5);
    }

    void start()
    {
        state_scheduler::start("grind_units_state");
    }
};

