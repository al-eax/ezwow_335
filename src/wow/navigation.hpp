#pragma once

#include "utils.hpp"
#include "addresses.hpp"
#include "AmeisenNavigation/AmeisenNavigation/ameisennavigation.h"
#include "wow/local_player.hpp"

const char *mmaps_folder = "TODO";

struct Navigation
{
    static auto get_map_id()
    {
        return read_address<int>(Addresses::address_map_id);
    }

    static auto get_path(Vector3d start, Vector3d end, int map_id = -1)
    {
        static AmeisenNavigation nav(mmaps_folder);
        std::vector<Vector3d> _path;
        int pathSize = 0;
        Vector3 path[MAX_PATH_LENGHT];
        if (map_id == -1)
            map_id = get_map_id();

        auto units = get_values(cache::units);
        std::vector<float *> pos;
        for (auto u : units)
        {
            if (u.reaction == unit_reaction_hated || u.reaction == unit_reaction_unfriendly)
                pos.push_back(&u.position.x);
        }

        if (nav.GetPath(map_id, {start.x, start.y, start.z}, {end.x, end.y, end.z}, path, &pathSize, pos))
        {
            for (int i = 0; i < pathSize; i++)
            {
                _path.push_back({path[i].x, path[i].y, path[i].z});
            }
        }
        return _path;
    }
};

struct path_walker
{
    std::vector<Vector3d> path;
    float required_dist = 5;
    bool finished = false;
    int current_path_index = 0;
    Vector3d last_player_position;
    bool auto_unstuck = true;

    path_walker(std::vector<Vector3d> _path = {})
    {
        set_path(_path);
    }

    void set_path(std::vector<Vector3d> _path)
    {
        path = _path;
        current_path_index = 0;
        finished = false;
    }

    virtual void update()
    {
        if (!finished)
        {
            while (path[current_path_index].distance(cache::local_player.position) < required_dist)
            {
                current_path_index++;
            }
            if (current_path_index < path.size())
            {
                walk_to_position(path[current_path_index]);
                finished = false;
                Log("pathwalker wp ", current_path_index, " of ", path.size());
            }
            else
            {
                finished = true;
                Log("pathwalker finished");
            }

            if (!is_walking())
            {
                Log("stuck!");
            }
        }
    }

    void stop_walking()
    {
        call_func_in_game_thread([this]()
                                 {
            WoWLocalPlayer lp;
            lp.click_to_move(lp.get_unit_position(), click_to_move_action::stop); });
    }

private:
    void walk_to_position(Vector3d position)
    {
        call_func_in_game_thread([position]()
                                 {
            WoWLocalPlayer lp;
            lp.click_to_move(position); });
    }

    bool is_walking()
    {
        bool result = false;
        call_func_in_game_thread([&result]()
                                 {
            WoWLocalPlayer lp;
            result = lp.get_speed() > 0; });
        return result;
    }
};

struct position_walker
{
    Vector3d position;
    path_walker walker;
    bool finished;

    void set_position(Vector3d position)
    {
        this->position = position;
        auto path = Navigation::get_path(cache::local_player.position, position);
        walker.set_path(path);
        finished = false;
    }

    void update()
    {
        if (!finished)
        {
            walker.update();
            finished = walker.finished;
        }
    }

    void stop_walking()
    {
        walker.stop_walking();
    }
};

struct target_walker
{
    uint64_t target_guid;
    int maximal_distance = 6;
    position_walker walker;
    bool finished;
    Vector3d last_target_position;

    void set_target_guid(uint64_t target)
    {
        target_guid = target;
        Log("set_target_guid");
        finished = false;
        last_target_position = cache::units[target_guid].position;
        walker.set_position(last_target_position);
    }

    void update()
    {
        if (!finished)
        {
            walker.update();
            finished = walker.finished;
            auto current_target_position = cache::units[target_guid].position;

            auto target_moved = last_target_position.distance(current_target_position) > maximal_distance;
            if (target_moved)
            {
                // Log("dist to last target pos, " , last_target_position.distance(current_target_position));
                set_target_guid(target_guid);
            }

            auto target_in_max_range = current_target_position.distance(cache::local_player.position) < maximal_distance;
            if (target_in_max_range)
            {
                stop_walking();
                finished = true;
            }
        }
        else
        {
            Log("target walker finished dist_to_target = ", cache::local_player.position.distance(cache::units[target_guid].position));
        }
    }

    void stop_walking()
    {
        walker.stop_walking();
    }
};