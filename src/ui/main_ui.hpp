#pragma once

#include <map>

#include "imgui/imgui.h"
#include "wow/wow_client.hpp"

#include "ui/ui_module.hpp"
#include "ui/imgui_impl_opengl2.h"
#include "hooks/game_loop_hook.hpp"

struct main_ui
{
private:
    static void draw()
    {
        if (ImGui::GetIO().WantCaptureMouse)
        {
            ImGui::SetNextWindowBgAlpha(0.8);
            ImGui::GetIO().MouseDrawCursor = true;
        }
        else
        {
            ImGui::SetNextWindowBgAlpha(0.4);
            ImGui::GetIO().MouseDrawCursor = false;
        }

        ImGui::Begin("EzWoW");
        ImGui::BeginTabBar("tab bar");
        {
            for (auto m : ui_module::modules)
            {
                if (ImGui::BeginTabItem(m->name.c_str()))
                {
                    m->on_tab_draw();
                    ImGui::EndTabItem();
                }
            }
        }
        ImGui::EndTabBar();

        ImGui::BeginChild("logs", ImVec2(0, 0), true);
        if (!logs.empty() && ImGui::Button("clear"))
            logs.clear();
        for (int i = logs.size(); i > 0; i--)
            ImGui::Text("%s", logs[i - 1].c_str());
        ImGui::EndChild();

        for (auto m : ui_module::modules)
            m->on_screen_draw();
    }

public:
    static void init()
    {
        ImGui::CreateContext();
        ImGui_ImplOpenGL2_Init();
        register_game_loop_callback(game_loop_callback);
        register_opengl_loop_callback(opengl_loop_callback);
    }

    static void opengl_loop_callback()
    {
        ImGui_ImplOpenGL2_NewFrame();
        ImGui::NewFrame();
        input_reset = true;
        draw();

        ImGui::End();
        ImGui::Render();
        ImGui::EndFrame();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    static void game_loop_callback()
    {

        for (auto m : ui_module::modules)
        {
            try
            {
                m->on_game_loop();
            }
            catch (const std::exception &e)
            {
                Log("Exception while calling on_game_loop from ", m->name, "ex ", e.what());
            }
        }
        if (WoWClient::is_world_loaded())
            for (auto m : ui_module::modules)
            {
                try
                {
                    m->on_ingame();
                }
                catch (const std::exception &e)
                {
                    Log("Exception while calling on_ingame from ", m->name, "ex ", e.what());
                }
            }
    }

    static void thread_loop_callback()
    {
        for (auto m : ui_module::modules)
        {
            for (auto m : ui_module::modules)
            {
                try
                {
                    m->on_thread_loop();
                }
                catch (const std::exception &e)
                {
                    Log("Exception while calling on_thread_loop from ", m->name, "ex ", e.what());
                }
            }
        }
    }
};