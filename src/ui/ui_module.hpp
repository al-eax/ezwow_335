#pragma once

#include <string>
#include <vector>
#include "utils.hpp"

class ui_module{
    public:
        static std::vector<ui_module*> modules;

        std::string name;
        ui_module(const std::string& _name ) : name(_name) {
            modules.push_back(this);
        };
        
        virtual void on_tab_draw() {};
        virtual void on_screen_draw() {};
        virtual void on_game_loop() {};
        virtual void on_ingame() {};
        virtual void on_thread_loop() {};

        
};

std::vector<ui_module*> ui_module::modules = {};