#pragma once

#include <inttypes.h>
#include <optional>
#include <sstream>

#include "wow_objectmgr.hpp"
#include "lua_functions.hpp"
#include "wow_object.hpp"
#include "utils.hpp"

enum click_to_move_action : uint32_t
{
    face_guid = 1,
    go_to = 4,
    loot = 6,
    attack = 10,
    stop = 3,
};

class WoWLocalPlayer : public WoWPlayer
{

public:
    WoWLocalPlayer()
    {
        auto lp_guid = WoWObjectManager().get_local_player_guid();
        if (lp_guid != 0)
            raw_object_address = WoWObjectManager().get_object_by_guid2(lp_guid)->raw_object_address;
    }

    auto get_target()
    {
        return WoWObjectManager().get_object_by_guid2(get_target_guid());
    }

    uint64_t get_target_guid()
    {
        return read_address<uint64_t>(Addresses::address_current_target_guid);
    }

    void set_target(uint64_t guid)
    {
        invoke_stdcall<void, Addresses::function_local_player_set_target_guid>(guid);
    }

    void cast_spell(const std::string &spell_name)
    {
        auto lua_str = "CastSpellByName('" + spell_name + "')";
        Lua::execute(lua_str);
    }

    void interact_target()
    {
        auto lua_str = "InteractUnit(\"target\")";
        Lua::execute(lua_str);
    }

    void interact_mouse_over(uint64_t guid = 0)
    {
        if (guid != 0)
        {
            WoWClient::set_mouse_over_guid(guid);
        }
        auto lua_str = "InteractUnit(\"mouseover\")";
        Lua::execute(lua_str);
    }

    void enable_click_to_move(bool enable)
    {
        std::string str_enable = enable ? "1" : "0";
        auto lua_str = "SetCVar('AutoInteract', '" + str_enable + "');";
        Lua::execute(lua_str);
    }

    void click_to_move(Vector3d position, click_to_move_action action = go_to, uint64_t guid = 0, float precision = 0)
    {
        invoke_thiscall<uint32_t, Addresses::function_local_player_click_to_move>(this->raw_object_address,
                                                                                  action,
                                                                                  &guid, &position,
                                                                                  precision);
    }

    auto get_last_target_guid()
    {
        return read_address<uint64_t>(Addresses::address_last_target_guid);
    }

    void release_spirit()
    {
        // https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-emulator-servers/wow-emu-exploits-bugs/310438-honor-exploits-tons-of-honor-working-all-servers.html
        auto lua_str = "RepopMe();";
        Lua::execute(lua_str);
    }

    auto get_corpse_position()
    {
        return read_address<Vector3d>(Addresses::address_local_player_corpse_position);
    }

    void retrieve_corpse()
    {
        // https://www.ownedcore.com/forums/world-of-warcraft/world-of-warcraft-emulator-servers/wow-emu-exploits-bugs/310438-honor-exploits-tons-of-honor-working-all-servers.html
        Lua::execute("RetrieveCorpse()");
    }

};

