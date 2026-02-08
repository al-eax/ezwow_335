#pragma once

#include <string>
#include <inttypes.h>
#include "utils.hpp"
#include "addresses.hpp"
#include "lua_functions.hpp"
#include "wow_client.hpp"

enum wow_object_type : int
{
    object_type_none,
    object_type_item,
    object_type_container,
    object_type_unit,
    object_type_player,
    object_type_gameobject,
    object_type_dynamic_object,
    object_type_corpse
};

enum unit_reaction
{
    unit_reaction_unknown = 0,
    unit_reaction_hated = 1,
    unit_reaction_hostile,
    unit_reaction_unfriendly,
    unit_reaction_neutral,
    unit_reaction_friendly,
    unit_reaction_honored,
    unit_reaction_revered,
    unit_reaction_exalted,
};

enum creature_type
{
    creature_type_none = 0,
    creature_type_beast = 1,
    creature_type_dragonkin = 2,
    creature_type_demon = 3,
    creature_type_elemental = 4,
    creature_type_giant = 5,
    creature_type_undead = 6,
    creature_type_humanoid = 7,
    creature_type_critter = 8,
    creature_type_mechanical = 9,
    creature_type_not_specified = 10,
    creature_type_totem = 11,
    creature_type_non_combat_pet = 12,
    creature_type_gas_cloud = 13,
};

std::string creature_type_str[] = {
    "none",
    "beast",
    "dragonkin",
    "demon",
    "elemental",
    "giant",
    "undead",
    "humanoid",
    "critter",
    "mechanical",
    "not_specified",
    "totem",
    "non_combat_pet",
    "gas_cloud",
};

std::string wow_obj_type_names[] = {
    "none",
    "item",
    "container",
    "unit",
    "player",
    "gameobject",
    "dynamic_object",
    "corpse"
};

struct UnitAura{
    std::string name;
    std::string rank;
    std::string icon;
    int count;
    std::string debuff_type;
    int duration;
    int expiration_time;
    std::string unit_caster;
    bool is_stealable;
    bool should_consolidate;
    int spell_id;
};

class WoWObject
{

public:
    uint32_t raw_object_address;

    WoWObject(uint32_t _raw_object_address = 0) : raw_object_address(_raw_object_address){};

    auto get_guid()
    {
        return read_address<uint64_t>(this->raw_object_address + Addresses::offset_object_guid);
    }

    std::string get_name()
    {
        auto my_guid = get_guid();
        std::string _name = " ";
        _name = invoke_stdcall<char *, Addresses::function_object_get_name_by_guid_ptr>(&my_guid);

        return _name;
    }

    auto get_object_type()
    {
        return read_address<wow_object_type>(this->raw_object_address + Addresses::offset_object_type);
    }

    auto get_next_object_address()
    {
        return read_address<uintptr_t>(this->raw_object_address + Addresses::offset_object_next_object_ptr);
    }
};

class WoWUnit : public WoWObject
{
public:
    WoWUnit(uint32_t _raw_object_address = 0) : WoWObject(_raw_object_address) {}

    auto get_speed()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto speed_str = Lua::execute("speed = GetUnitSpeed('mouseover')", "speed");
        if (!speed_str.empty())
        {
            return std::stof(speed_str) / 7.0f * 100.0f;
        }
        return 0.0f;
    }

    auto get_unit_position()
    {
        return read_address<Vector3d>(this->raw_object_address + Addresses::offset_unit_position);
    }

    void set_unit_position(Vector3d position)
    {
        write_address(this->raw_object_address + Addresses::offset_unit_position, position);
    }

    auto descriptor()
    {
        return read_address(this->raw_object_address + Addresses::offset_object_descriptor);
    }

    auto get_health()
    {
        uint32_t health;
        health = invoke_fastcall<uint32_t, Addresses::function_unit_get_health>(raw_object_address);

        return health;
    }

    auto get_health2()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto health_str = Lua::execute("health = UnitHealth('mouseover')", "health");
        return atoi(health_str.c_str());
    }

    auto get_health3()
    {
        return read_address<uint32_t>(descriptor() + Addresses::descriptor_UNIT_FIELD_HEALTH);
    }

    auto get_max_health()
    {
        return read_address<uint32_t>(descriptor() + Addresses::descriptor_UNIT_FIELD_MAXHEALTH);
    }

    auto get_level()
    {
        return read_address<uint32_t>(descriptor() + Addresses::descriptor_UNIT_FIELD_LEVEL);
    }

    uint32_t get_mana()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto mana_str = Lua::execute("mana = UnitMana('mouseover')", "mana");
        if (mana_str != "")
            return std::stoul(mana_str.c_str());
        return 0;
    }

    uint32_t get_max_mana()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto mana_str = Lua::execute("mana = UnitManaMax('mouseover')", "mana");
        if (mana_str != "")
            return std::stoul(mana_str.c_str());
        return 0;
    }

    auto get_reaction()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto reaction = Lua::execute("reaction = UnitReaction('player', 'mouseover')", "reaction");
        if (reaction != "")
            return (unit_reaction)std::stoi(reaction);
        return unit_reaction_unknown;
    }

    auto get_creature_type()
    {
        creature_type _type;
        return invoke_fastcall<creature_type, Addresses::function_unit_ger_creature_type>(raw_object_address);
    }

    bool is_casting()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto lua_str = R"(
            is_casting = 0
            spell, _, _, _, _, endTime = UnitCastingInfo('mouseover')
            if spell then
                is_casting = 1
            end
        )";
        auto is_casting_str = Lua::execute(lua_str, "is_casting");
        return is_casting_str == "1";
    }

    auto get_target_guid(){
        return read_address<uint64_t>(descriptor() + Addresses::descriptor_UNIT_FIELD_TARGET);
    }

    auto get_faction(){
        return read_address<uint32_t>(descriptor() + Addresses::descriptor_UNIT_FIELD_FACTIONTEMPLATE);
    }

    bool is_channeling()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto lua_str = R"(
            is_channeling = 0
            spell, _, _, _, endTime = UnitChannelInfo('mouseover')
            if spell then
                is_channeling = 1
            end
        )";
        auto is_channeling_str = Lua::execute(lua_str, "is_channeling");
        return is_channeling_str == "1";
    }

    auto get_class_name()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto lua_str = "_ , class_name = UnitClass('mouseover')";
        return Lua::execute("lua_str", "class_name");
    }

    bool is_in_combat()
    {
        auto lua_str = R"(
            in_combat = 0
            if UnitAffectingCombat('mouseover') then
                in_combat = 1
            end
        )";
        auto in_combat_str = Lua::execute(lua_str, "in_combat");
        return in_combat_str == "1";
    }

    auto get_auras(){
        WoWClient::set_mouse_over_guid(get_guid());
        std::vector<UnitAura> auras;
        for(int i = 0; i <= 40; i++){
            Lua::execute("name, rank, icon, count, debuffType, duration, expirationTime, unitCaster, isStealable, shouldConsolidate, spellId = UnitAura('mouseover'," + std::to_string(i) + ")");
            auras.push_back({
                Lua::get_text("name"),
                Lua::get_text("rank"),
                Lua::get_text("icon"),
                std::stoi(Lua::get_text("count")),
                Lua::get_text("debuffType"),
                std::stoi(Lua::get_text("duration")),
                std::stoi(Lua::get_text("expirationTime")),
                Lua::get_text("unitCaster"),
                Lua::get_text("isStealable") == "1",
                Lua::get_text("shouldConsolidate") == "1",
                std::stoi(Lua::get_text("spellId"))
            });
        }
        return auras;
    }
};

class WoWPlayer : public WoWUnit
{
public:
    WoWPlayer(uint32_t _raw_object_address = 0) : WoWUnit(_raw_object_address) {}

    bool is_ghost()
    {
        WoWClient::set_mouse_over_guid(get_guid());
        auto isghost_str = Lua::execute("isghost = UnitIsGhost('mouseover')", "isghost");
        if (!isghost_str.empty())
            return isghost_str == "1";
        return false;
    }
};
