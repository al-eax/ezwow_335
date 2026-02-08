#pragma once

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "cache.hpp"
#include "wow/navigation.hpp"

#include <set>

class loot_state : public state
{
    std::vector<uint64_t> *units_to_loot;
    uint64_t current_unit_to_loot;
    target_walker walker;

    void loot_unit(uint64_t unit_guid)
    {
        sleep(2);

        call_func_in_game_thread([unit_guid] {
            WoWLocalPlayer().set_target(unit_guid);
        });

        Log("set target done: ", unit_guid ,cache::units[unit_guid].name);
        sleep(2);
        
        call_func_in_game_thread([unit_guid] {
            WoWLocalPlayer().interact_target();
        });
        Log("interact target done");
        sleep(2);
    }

    uint64_t search_unit_to_loot()
    {
        if (units_to_loot->empty())
            return 0;
        uint64_t guid = units_to_loot->back();
        units_to_loot->pop_back();
        while (cache::units[guid].health > 0 && !units_to_loot->empty())
        {
            guid = units_to_loot->back();
            units_to_loot->pop_back();
        }
        return guid;
    }

public:
    loot_state() : state("loot_state") {}

    void enter(void *param)
    {
        if (param == nullptr)
            Log("ERROR!!! , param == null");
        units_to_loot = (std::vector<uint64_t> *)(param);
        current_unit_to_loot = 0;
    }

    void update()
    {
        if (current_unit_to_loot == 0)
        {
            current_unit_to_loot = search_unit_to_loot();
            if (current_unit_to_loot != 0)
            {
                Log("walkt to corpse ", current_unit_to_loot);
                walker.set_target_guid(current_unit_to_loot);
            }
            else
            {
                change_state("recover_state");
                return;
            }
        }

        walker.update();

        if (walker.finished)
        {
            Log("loot corpse ", current_unit_to_loot);
            loot_unit(current_unit_to_loot);
            current_unit_to_loot = 0;
        }
    }
};