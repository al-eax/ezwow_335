#pragma once

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "cache.hpp"
#include "wow/navigation.hpp"

class dead_player_state : public state
{

public:
    position_walker walker;

    dead_player_state() : state("dead_player_state") {}

    void enter(void *)
    {
        if (!cache::local_player.is_ghost)
        {
            call_func_in_game_thread([this]() {
                WoWLocalPlayer lp;
                lp.release_spirit();
            });
            sleep(5);
        }
        walker.set_position(cache::local_player.corpse_position);
    }

    void update()
    {
        walker.update();
        if (walker.finished)
        {
            call_func_in_game_thread([this]() {
                WoWLocalPlayer lp;
                lp.retrieve_corpse();
            });
            sleep(2);
            change_state("recover_state");
            return;
        }
        else
        {
            if (cache::local_player.speed == 0)
            {
                //walker.set_position(cache::local_player.corpse_position);
            }
            else if (cache::local_player.speed < 1)
            {
                //jump
            }
        }
    }
};
