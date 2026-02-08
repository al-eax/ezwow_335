#pragma once


#include "ui_module.hpp"
#include "wow/lua_functions.hpp"

#include "imgui/imgui.h"
#include "wow/wow_client.hpp"
#include "wow/wow_inventory.hpp"

class db_module : public ui_module{
private:

    bool btn_exec = false;
    char sql_query[200];
public:
    db_module() : ui_module("database"){}

    void on_tab_draw() {
        ImGui::InputText("sql",sql_query,sizeof(sql_query));
        btn_exec |= ImGui::Button("exec");
        
    }

    void on_thread_loop() {
        if(btn_exec){
            btn_exec = false;
            call_func_in_game_thread( [](){
                auto freeslots = WoWInventory().get_free_slots();
                auto items = WoWInventory().get_items();
                std::cout << "free slots " << freeslots << "\n";
                auto grey_items = filter(items, [](auto i){return i.rarity == item_rarity_grey;});
                Log("grey_items " , grey_items.size());
                for(auto i : grey_items) i.use();
            });
        }
    }
};

