#pragma once

#include <set>

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "grind_bot/combat.hpp"

#include "cache.hpp"
#include "utils.hpp"

class infight_state : public state
{
    druid_combat_profile combat;
    std::set<uint64_t> attacked_unit_guids;
    std::vector<uint64_t> targets_to_loot;
    uint64_t last_attacker;
    target_walker walker;

    uint64_t get_attacker_guid_by_lowest_hp()
    {
        auto units_targeting_me = filter( get_values(cache::units) , [](unit_dump u){
            return u.target_guid == cache::local_player.guid && u.reaction != unit_reaction_friendly && u.health > 0;
        });
        
        std::sort(units_targeting_me.begin(), units_targeting_me.end(), [](unit_dump u1, unit_dump u2){
            return u1.health < u2.health;
        });

        if(units_targeting_me.empty()) return 0;
        return units_targeting_me.front().guid;
    }

    void fight_attacker(uint64_t attacker_guid)
    {
        call_func_in_game_thread([this, attacker_guid]() {
            WoWLocalPlayer lp;
            bool success = false;
            auto target = WoWObjectManager().get_object_by_guid2(attacker_guid);
            if(target){
                lp.set_target(attacker_guid);
                combat.fight(lp, WoWUnit(target->raw_object_address));
            }
        });
    }

    void face_target(uint64_t target)
    {
        call_func_in_game_thread([target]() {
            WoWLocalPlayer lp;
            lp.click_to_move( cache::units[target].position , click_to_move_action::face_guid, target);
        });
    }

public:
    infight_state() : state("infight_state") {}

    void enter(void *)
    {
        attacked_unit_guids.clear();
        targets_to_loot.clear();
    }

    void update()
    {
        auto attacker_guid = get_attacker_guid_by_lowest_hp();
        if (attacker_guid != 0)
        {
            //Log("fight_attacker guid: " , attacker_guid);
            if (last_attacker != attacker_guid)
            {
                Log("attack target ", cache::units[attacker_guid].name , " ",attacker_guid );
                last_attacker = attacker_guid;
                sleep(0.5);
                attacked_unit_guids.insert(attacker_guid);
                walker.maximal_distance = combat.max_target_distance;
                walker.set_target_guid(attacker_guid);
            }
            if(cache::local_player.is_casting){
                walker.update();
            }

            if(walker.finished){
                face_target(attacker_guid);
                sleep(0.5);
                fight_attacker(attacker_guid);
            }
        }
        else
        {
            change_state("loot_state", &targets_to_loot);
            return;
        }
    }

    void leave()
    {
        for (auto attacked_unit : attacked_unit_guids)
        {
            if (cache::units[attacked_unit].health == 0)
                targets_to_loot.push_back(attacked_unit);
        }
    }
};