#pragma once

#include <inttypes.h>
#include <vector>
#include <stdio.h>
#include <optional>
#include <sstream>

#include "utils.hpp"
#include "wow_object.hpp"

class WoWObjectManager{

public:

    auto get_objects(wow_object_type type_filter = object_type_none){
        std::vector<WoWObject> objects;

        auto client_connection_address = read_address<uintptr_t>(Addresses::address_client_connection_ptr);
        if(client_connection_address == 0) return objects;

        auto object_manager_address = read_address<uintptr_t>(client_connection_address + Addresses::offset_object_manager);
        if(object_manager_address == 0) return objects;

        auto first_object_addr = read_address<uintptr_t>(object_manager_address + Addresses::offset_object_manager_first_object);
        if(first_object_addr == 0) return objects;

        auto corrent_obj = WoWObject(first_object_addr);
        while(corrent_obj.get_object_type() > 0 && corrent_obj.get_object_type() < 9){
            objects.push_back(corrent_obj);
            corrent_obj = WoWObject(corrent_obj.get_next_object_address());
        }

        if(type_filter == object_type_none)
            return objects;
        
        std::vector<WoWObject> filter_objects;
        for(auto obj : objects)
            if(obj.get_object_type() == type_filter)
                filter_objects.push_back(obj);
        return filter_objects;
    }

    auto get_local_player_guid(){
            return invoke_stdcall<uint64_t,Addresses::function_local_player_get_guid>();
    }
    
    WoWObject get_object_by_guid(uint64_t guid,bool* success = nullptr){
        for(auto& obj : get_objects())
        {
            if(obj.get_guid() == guid){
                if(success != nullptr) *success = true;
                return obj;
            }
        }
        if(success != nullptr) *success = false;
        return WoWObject(0);
    }

    std::optional<WoWObject> get_object_by_guid2(uint64_t guid){

        for(auto& obj : get_objects())
        {
            if(obj.get_guid() == guid){
                return obj;
            }
        }
        return std::nullopt;
    }

    auto get_players(){
        std::vector<WoWPlayer> players;
        for(auto pl : get_objects(object_type_player))
            players.push_back( WoWPlayer(pl.raw_object_address) );
        return players;
    }
    
    auto get_units(){
        std::vector<WoWUnit> players;
        for(auto pl : get_objects(object_type_unit))
            players.push_back( WoWUnit(pl.raw_object_address) );
        return players;
    }
};
