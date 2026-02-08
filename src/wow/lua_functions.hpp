#pragma once

#include "utils.hpp"
#include <iostream>
#include "addresses.hpp"
#include "hooks/game_loop_hook.hpp"

/*
#define do_lua_string_func_address 0x00819210
auto wowfunc_do_lua_string = reinterpret_cast<void (__cdecl *)(const char *,const char*,const char*)>(do_lua_string_func_address);


void lua_login(const std::string& username, const std::string& password){
    auto lua_str = "DefaultServerLogin(\"" + username + "\",\"" + password + "\")";
    wowfunc_do_lua_string(lua_str.c_str(),nullptr,nullptr);
}

void lua_cast_spell(const std::string& spellname){
    auto lua_str = "CastSpellByName(\"" + spellname +  "\")";
    wowfunc_do_lua_string(lua_str.c_str(),nullptr,nullptr);
}

void lua_interact_target(){
    auto lua_str = "InteractUnit(\"target\")";
    wowfunc_do_lua_string(lua_str,nullptr,nullptr);
}

void lua_interact_mouse_over(){
    auto lua_str = "InteractUnit(\"mouseover\")";
    wowfunc_do_lua_string(lua_str,nullptr,nullptr);
}

void lua_set_realmlist(const std::string& realmlist){
    auto lua_str = "SetCVar('realmlist', '" + std::string(realmlist) + "');";
    wowfunc_do_lua_string( lua_str.c_str() ,nullptr,nullptr);
}

#define wowfunc_lua_get_text_addr 0x00819d40
auto wowfunc_lua_get_text =  reinterpret_cast<char * __cdecl (*)(const char *frame_name,int param_2,int param_3)>(wowfunc_lua_get_text_addr);
std::string lua_get_text(const std::string& var_name){
    std::string result = "";
    result += wowfunc_lua_get_text(var_name.c_str(),-1,0);
    return result;
}


#define wowfunc_lua_get_localized_text_addr 0x007225e0
auto wowfunc_lua_get_localized_text = reinterpret_cast<char* (__thiscall *) (wow_object* , const char*, int)>(wowfunc_lua_get_localized_text_addr);
std::string lua_get_localized_text(const std::string& param ){
    std::string result = wowfunc_lua_get_localized_text((wow_object*)get_local_player(),param.c_str(),-1);
    return result;
}

std::string lua_execute(const std::string& lua_code, const std::string& var_to_read){
    wowfunc_do_lua_string(lua_code.c_str(),lua_code.c_str(),nullptr);
    return lua_get_text(var_to_read);
}

void lua_enable_ctm(bool enable){
    std::string str_enable = enable ? "1" : "0";
    auto lua_str = "SetCVar('AutoInteract', '" + str_enable + "');";
    wowfunc_do_lua_string(lua_str.c_str(),nullptr,nullptr);
}
*/

class Lua
{

public:
    static std::string get_text(const std::string &var_name)
    {
        std::string ret_val;
        ret_val = invoke_cdecl<const char *, Addresses::function_lua_get_text>(var_name.c_str(), -1, 0);
        return ret_val;
    }


    static std::string execute(const std::string &lua_code, const std::string &var_name = "")
    {
        std::string ret_val = "";
        invoke_cdecl<uintptr_t, Addresses::function_lua_do_string>(lua_code.c_str(), lua_code.c_str(), nullptr);
        if (!var_name.empty())
            ret_val = get_text(var_name);
        return ret_val;
    }

    static void display_chat_message(std::string msg){
        execute("DEFAULT_CHAT_FRAME:AddMessage('" + msg + ")");
    }
};