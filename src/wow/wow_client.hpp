#pragma once

#include <string>

#include "lua_functions.hpp"

/*
charcreate
login
*/

enum game_state
{
    state_login,
    state_charselect,
    state_in_game,
    state_unknown,
};

class WoWClient
{

public:
    static std::string get_game_state_str()
    {
        return reinterpret_cast<const char *>(Addresses::address_game_state_string);
    }

    static auto get_game_state2()
    {
        return read_address(Addresses::address_game_state);
    }
    static auto is_world_loaded()
    {
        return read_address<bool>(Addresses::address_world_loaded);
    }
    static auto is_world_loading()
    {
        return read_address<bool>(Addresses::address_world_loading);
    }

    static auto get_game_state()
    {
        if (get_game_state_str() == "login")
            return state_login;
        if (get_game_state_str() == "charselect" && !is_world_loaded())
            return state_charselect;
        if (get_game_state_str() == "charselect" && is_world_loaded())
            return state_in_game;
        return state_unknown;
    }

    static void login(const std::string &username, const std::string &password)
    {
        auto lua_str = "DefaultServerLogin(\"" + username + "\",\"" + password + "\")";
        Lua::execute(lua_str);
    }

    static void set_realmlist(const std::string realmlist)
    {
        auto lua_str = "SetCVar('realmlist', '" + std::string(realmlist) + "');";
        Lua::execute(lua_str);
    }

    static void select_character(int index)
    {
        auto lua_str = "SelectCharacte(" + std::to_string(index) + ");";
        Lua::execute(lua_str);
    }

    static void enter_world()
    {
        auto lua_str = "EnterWorld();";
        Lua::execute(lua_str);
    }

    static void set_mouse_over_guid(uint64_t guid)
    {
        write_address(Addresses::address_mouse_over_guid_ptr, guid);
    }
};