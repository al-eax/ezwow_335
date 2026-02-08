#include "ui/ui_module.hpp"
#include "grind_bot.hpp"
#include "wow/wow_camera.hpp"

class grind_bot_ui : public ui_module
{
    bool btn_start = false;
    bool btn_stop = false;
    bool btn_spot = false;
    Vector3d target_pos;
    grind_bot_scheduler bot;

    void draw_path(std::vector<Vector3d> path, ImColor color = IM_COL32_WHITE){
         for(int i = 1; i < path.size(); i++){
                float x1,y1,x2,y2;
                if(world_to_screen(path[i-1],x1,y1) && world_to_screen(path[i],x2,y2))
                    ImGui::GetBackgroundDrawList()->AddLine( {x1,y1}, {x2,y2},color);
            }
    }

public:
    grind_bot_ui() : ui_module("grind bot") {}

    void on_tab_draw()
    {
        ImGui::Text("Current state: %s", bot.current_state.c_str());
        btn_start |= ImGui::Button("start");
        btn_spot |= ImGui::Button("set grind spot");
        btn_stop |= ImGui::Button("stop");
    }

    void on_screen_draw()
    {
        if(bot.current_state == "sell_items_state"){
            draw_path(((sell_items_state*)bot.state_map["sell_items_state"])->walker.walker.path);
            draw_path(((sell_items_state*)bot.state_map["sell_items_state"])->t_walker.walker.walker.path);
        }

        if(bot.current_state == "grind_units_state"){
            auto gu = (grind_units_state*)bot.state_map["grind_units_state"];
            draw_path( gu->walker.walker.walker.path );
        }

        if(bot.current_state == "dead_player_state"){
            auto pd = (dead_player_state*)bot.state_map["dead_player_state"];
            draw_path(pd->walker.walker.path);
        }

        auto target_guid = cache::local_player.target_guid;
        if(target_guid != 0 && cache::units.count(target_guid)>0){
            float x,y;
            if(world_to_screen(cache::units[target_guid].position,x,y)){
                ImGui::GetBackgroundDrawList()->AddText( {x,y}, IM_COL32_WHITE, cache::units[target_guid].name.c_str());
            }
        }
    }

    void on_thread_loop()
    {

        if (WoWClient::get_game_state() != game_state::state_in_game)
            return;

        if (btn_start)
        {
            btn_start = false;
            bot.start();
        }
        if (btn_stop)
        {
            btn_stop = false;
            bot.stop();
        }

        bot.schedule();
        
    }

    void on_game_loop() {}
    void on_ingame()
    {
        
    }
};
