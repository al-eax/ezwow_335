#pragma once

#include <bits/stdc++.h>

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "grind_bot/combat.hpp"
#include "cache.hpp"
#include "wow/navigation.hpp"


class grind_units_state : public state{
    
    uint64_t unit_to_grind;
    druid_combat_profile combat;

    uint64_t get_closest_target_guid()
    {
        auto units = get_values( cache::units );

        auto possible_targets = filter(units , [](unit_dump u) {
            auto level_diff = u.level - cache::local_player.level;
            return (u.reaction == unit_reaction_hostile || u.reaction == unit_reaction_unfriendly) 
                    && u.health > 0
                    //&& level_diff > - 4 && level_diff <= 0 // unit level is 4 level lower than player level
                    ;
        });

        std::sort(possible_targets.begin(), possible_targets.end(), [](auto u1, auto u2){
            return cache::local_player.position.distance(u1.position) < cache::local_player.position.distance(u2.position);
        });

        if(!possible_targets.empty())
            return possible_targets.front().guid;
        return 0;
    }

    void start_fight(uint64_t unit_to_grind){
        call_func_in_game_thread([unit_to_grind,this](){
                WoWLocalPlayer lp;
                auto target = WoWObjectManager().get_object_by_guid2(unit_to_grind);
                if(target){
                    lp.click_to_move(lp.get_unit_position(), click_to_move_action::face_guid, unit_to_grind);
                    combat.fight( lp, WoWUnit(target->raw_object_address));
                }
            });
    }

public:
    target_walker walker;
    grind_units_state() : state("grind_units_state") {}

    void enter(void*){
        unit_to_grind = 0;
        if(cache::local_player.inventory_free_slots == 0){
                change_state("sell_items_state");
                return;
        }
    }

    void update(){
        
        if(unit_to_grind == 0){
            unit_to_grind = get_closest_target_guid();
            if(unit_to_grind == 0){
                Log("found no target, get_closest_target_guid returned 0");
                return;
            }
            walker.set_target_guid(unit_to_grind);
            
            call_func_in_game_thread([this](){
                WoWLocalPlayer lp;
                lp.set_target(unit_to_grind);
            });
        }

        walker.update();

        if(walker.finished){
            sleep(1);            
            start_fight( unit_to_grind );
            sleep(1);
            call_func_in_game_thread([this](){ cache::update();  });
            sleep(1);
            if(cache::local_player.is_in_combat){
                change_state( "infight_state" );
            }else{
                unit_to_grind = 0;
            }
        }
    }

    void leave(){
        if(!walker.finished){

            call_func_in_game_thread([this](){
                WoWLocalPlayer lp;
                lp.click_to_move( lp.get_unit_position() , click_to_move_action::stop);
            });
        }
    }

};