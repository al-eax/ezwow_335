#pragma once

#include "ui_tab.hpp"
#include "../third_party/subhook/subhook.c"

#include "../core/wow_client.hpp"
#include "../core/local_player.hpp"
#include "../core/wow_objectmgr.hpp"

subhook::Hook click_to_move_hook;
subhook::Hook fly_hook;

#define fly_hook_addr 0X00761b00

int __thiscall
fly_hook_callback(int param_1, float *param_1_00, float param_2, float param_3, float param_4)
{
    return 1;
}

float click_to_move_position[3];
bool click_to_move_called = false;

static uint __thiscall _click_to_move_callback(int *, int action, uint64_t *guid, float *pos, float precision)
{
    Log("click_to_move_called");

    click_to_move_called = true;
    for (int i = 0; i < 3; i++)
        click_to_move_position[i] = pos[i];
}

class hack_tab : public ui_tab
{
private:
    bool chbx_move_target_by_right_click = false;
    bool chbx_wall_climb = false;

public:
    hack_tab() : ui_tab("hacks") {}

    void on_tab_draw()
    {
        ImGui::Checkbox("move target by right click", &chbx_move_target_by_right_click);
        ImGui::Checkbox("wall climb", &chbx_wall_climb);
    }

    void on_ingame()
    {
        WoWLocalPlayer lp;
        if (!WoWLocalPlayer::get_instance(lp))
            return;

        if (chbx_move_target_by_right_click)
        {

            if (!click_to_move_hook.IsInstalled())
            {
                click_to_move_hook.Install((void *)Addresses::function_local_player_click_to_move, (void *)_click_to_move_callback);
                lp.enable_click_to_move(true);
                Log("added ctm hook");
            }
            if (click_to_move_called)
            {
                click_to_move_called = false;
                WoWObject target;
                if (!lp.get_target(target))
                    return;
                if (target.get_object_type() == object_type_unit || target.get_object_type() == object_type_player)
                {
                    Log("Move ", target.get_name(), " to ", Vector3d(click_to_move_position).to_str());
                    WoWUnit(target.raw_object_address).set_unit_position(Vector3d(click_to_move_position));
                }
            }
        }
        else
        {
            if (click_to_move_hook.IsInstalled())
            {
                click_to_move_hook.Remove();
                Log("removed ctm hook");
            }
        }

        if (chbx_wall_climb)
        {
            if (!fly_hook.IsInstalled())
            {
                fly_hook.Install((void *)fly_hook_addr, (void *)fly_hook_callback);
                Log("enable fly hack hook");
            }
        }
        else
        {
            if (fly_hook.IsInstalled())
            {
                fly_hook.Remove();
                Log("disable fly hack hook");
            }
        }
    }
};
