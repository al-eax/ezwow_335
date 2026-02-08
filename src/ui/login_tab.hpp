#pragma once

#include "ui_module.hpp"
#include "wow/lua_functions.hpp"

#include "imgui/imgui.h"
#include "wow/wow_client.hpp"

class login_module : public ui_module
{
private:
    bool btn_login_pressed = false;
    bool btn_select_char = false;
    bool btn_test = false;

    char username[256] = "";
    char password[256] = "";

    char realmlist[256] = "";

public:
    login_module() : ui_module("login") {}

    void on_tab_draw()
    {
        ImGui::InputText("username", username, sizeof(username));
        ImGui::InputText("password", password, sizeof(password));
        ImGui::InputText("realmlist", realmlist, sizeof(realmlist));
        btn_login_pressed |= ImGui::Button("login");
        btn_select_char |= ImGui::Button("select char");
        btn_test |= ImGui::Button("test");
    }

    void on_thread_loop()
    {
        if (btn_login_pressed)
        {
            call_func_in_game_thread([this]()
                                     {
                WoWClient::set_realmlist(realmlist);
                WoWClient::login(username,password); });

            btn_login_pressed = false;
            Log("login", 123);
        }
        if (btn_select_char)
        {
            call_func_in_game_thread([this]()
                                     {
                WoWClient::select_character(0);
                WoWClient::enter_world(); });
            btn_select_char = false;
        }
        if (btn_test)
        {
            btn_test = false;
        }
    }
};
