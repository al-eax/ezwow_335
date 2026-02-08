#pragma once

#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "wow/wow_inventory.hpp"

#include <map>

struct unit_dump
{
    wow_object_type type;
    uint64_t guid;
    std::string name;
    uint32_t health;
    uint32_t max_health;
    uint32_t level;
    unit_reaction reaction;
    Vector3d position;
    creature_type cr_type;
    uint32_t mana;
    uint64_t target_guid;
    float speed;
    bool is_in_combat;
    
    std::string to_str(){
        std::stringstream ss;
        ss << wow_obj_type_names[type] << " '" << name << "'\n"
            << "  health " << health << "/" << max_health << "\n"
            << "  mana " << mana << "\n"
            << "  level " << level << "\n"
            << "  reaction type " << reaction << "\n"
            << "  position " << position.to_str() << "\n"
            << "  creature type " << cr_type << "\n";
        return ss.str();
    }
};

struct local_player_dump{
    uint64_t guid;
    uint64_t target_guid;
    uint64_t last_target_guid;
    uint32_t health;
    uint32_t max_health;
    Vector3d position;
    bool is_casting;
    bool is_channeling;
    uint32_t mana;
    Vector3d corpse_position;
    bool is_ghost;
    bool is_in_combat;
    float speed;
    uint32_t max_mana;
    int inventory_free_slots;
    int level;
};



struct cache{
    static std::map<uint64_t,unit_dump> units;
    static std::map<uint64_t,unit_dump> players;
    static local_player_dump local_player;
    
    static void update(){
        static int __ticks = 0;
        
        if(++__ticks == 30){
            units.clear();
            players.clear();
        }

        if(WoWClient::get_game_state() != game_state::state_in_game) return;

        for(auto u : WoWObjectManager().get_units())
            units[u.get_guid()] = {
                u.get_object_type(),
                u.get_guid(), 
                u.get_name(), 
                u.get_health(),
                u.get_max_health(),
                u.get_level(),
                u.get_reaction(),
                u.get_unit_position(),
                u.get_creature_type(),
                u.get_mana(),
                u.get_target_guid(),
                u.get_speed(),
            };

        for(auto u : WoWObjectManager().get_players())
            players[u.get_guid()] = {
                u.get_object_type(),
                u.get_guid(), 
                u.get_name(), 
                u.get_health(),
                u.get_max_health(),
                u.get_level(),
                u.get_reaction(),
                u.get_unit_position(),
                u.get_creature_type(),
                u.get_mana(),
                u.get_target_guid(),
            };

        WoWLocalPlayer lp;

        local_player = {
            lp.get_guid(),
            lp.get_target_guid(),
            lp.get_last_target_guid(),
            lp.get_health(),
            lp.get_max_health(),
            lp.get_unit_position(),
            lp.is_casting(),
            lp.is_channeling(),
            lp.get_mana(),
            lp.get_corpse_position(),
            lp.is_ghost(),
            lp.is_in_combat(),
            lp.get_speed(),
            lp.get_max_mana(),
            WoWInventory().get_free_slots(),
            lp.get_level(),
        };
    }
};

std::map<uint64_t,unit_dump> cache::units = {};
std::map<uint64_t,unit_dump> cache::players = {};
local_player_dump cache::local_player = local_player_dump();