#include "ui/ui_module.hpp"
#include "wow/wow_camera.hpp"
#include "cache.hpp"
#include "db/database.hpp"
#include "wow/navigation.hpp"

#include "imgui/imgui.h"

#include <bits/stdc++.h>
#include <vector>
#include <map>

std::map<uint64_t, unit_dump> item_cache;

class radar_module : public ui_module
{
private:
    bool chbx_draw_all_units;
    bool chbx_draw_all_players;
    bool chbx_draw_name;
    bool chbx_draw_distance;
    bool chbx_draw_health;
    bool chbx_draw_creature_type;
    bool chbx_draw_vendors;
    int count = 0;

    void render2d()
    {
    }
    void render3d(unit_dump item)
    {
        if (item.guid == cache::local_player.guid)
            return;

        auto draw = ImGui::GetBackgroundDrawList();
        float x, y;
        if (!world_to_screen(item.position, x, y))
            return;

        ImColor color;
        bool is_enemy = item.reaction != unit_reaction_friendly && item.reaction != unit_reaction_neutral;
        if (is_enemy)
            color = ImColor(1.0f, 0.0f, 0.0f);
        else
            color = ImColor(0.0f, 0.0f, 1.0f);

        if (chbx_draw_name)
        {
            draw->AddText({x, y}, color, item.name.c_str());
            y += 10;
        }
        if (chbx_draw_health)
        {
            //draw->AddRectFilled( {x,y} ,{x+100,y + 10}, color );
            //draw->AddRectFilled( {x+1,y+2} ,{x+1 + 100*( (float)item.health / (float)item.max_health ), y+8 }, IM_COL32_WHITE );
            draw->AddText({x + 1, y + 1}, color, (std::to_string(item.health) + "/" + std::to_string(item.max_health)).c_str());
            y += 10;
        }
        if (true)
        {
            draw->AddText({x, y}, color, ("lvl:" + std::to_string(item.level)).c_str());
            y += 10;
        }
        if (chbx_draw_distance)
        {
            draw->AddText({x, y}, color, ("distance:" + std::to_string(item.position.distance(cache::local_player.position))).c_str());
            y += 10;
        }
        if (chbx_draw_creature_type)
        {
            draw->AddText({x, y}, color, creature_type_str[item.cr_type].c_str());
            y += 10;
        }
    }

    void render_vendors()
    {
        for (auto v : vendors)
        {
            auto draw = ImGui::GetBackgroundDrawList();
            float x, y;
            if (!world_to_screen( v.position , x, y))
                continue;
            draw->AddText({x, y}, IM_COL32_WHITE, v.name.c_str());
        }
    }

public:
    std::vector<db_vendor> vendors;

    radar_module() : ui_module("radar")
    {}

    void on_tab_draw()
    {
        ImGui::Checkbox("draw all units", &chbx_draw_all_units);
        ImGui::Checkbox("draw all player", &chbx_draw_all_players);
        ImGui::Checkbox("draw name", &chbx_draw_name);
        ImGui::Checkbox("draw distance", &chbx_draw_distance);
        ImGui::Checkbox("draw health", &chbx_draw_health);
        ImGui::Checkbox("draw creature type", &chbx_draw_creature_type);
        ImGui::Checkbox("draw vendors", &chbx_draw_vendors);
    }

    void on_screen_draw()
    {
        if (chbx_draw_all_players)
            for (auto item : cache::players)
                render3d(item.second);

        if (chbx_draw_all_units)
            for (auto u : cache::units)
                render3d(u.second);

        if(chbx_draw_vendors){
            if(vendors.empty()){
                database db;
                vendors = db.get_vendors(Navigation::get_map_id());
            }
            render_vendors();
        }
    }
};
