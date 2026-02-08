#pragma once

#include "state_scheduler.hpp"
#include "wow/wow_object.hpp"
#include "wow/wow_objectmgr.hpp"
#include "wow/local_player.hpp"
#include "cache.hpp"
#include "wow/navigation.hpp"
#include "db/database.hpp"

#include <algorithm>

struct sell_items_state : state
{
    sell_items_state() : state("sell_items_state"){};

    std::vector<db_vendor> vendors;
    position_walker walker;
    target_walker t_walker;
    db_vendor current_vendor;

    void enter(void *) override
    {
        vendors = sort_vendors();
        if (vendors.empty())
        {
            change_state("recover_state");
            return;
        }

        walker.set_position(vendors.back().position);
        Log("walk to ventor ", vendors.back().name, vendors.back().subname, " at ", vendors.back().position.to_str());
    }

    uint64_t get_unit_closest_to_point(Vector3d point)
    {
        auto units = get_values(cache::units);
        std::sort(units.begin(), units.end(), [point](unit_dump u1, unit_dump u2) { return u1.position.distance(point) < u2.position.distance(point); });
        if (units.empty())
            return 0;
        return units.front().guid;
    }

    void update() override
    {
        walker.update();

        if (walker.finished)
        {
            auto vendor_guid = get_unit_closest_to_point(vendors.back().position);
            if ( vendor_guid  != 0 && cache::units[vendor_guid].position.distance(vendors.back().position) < 1)
            {
                Log("found Vendor " , cache::units[vendor_guid].name , " mapped to vendor " , vendors.back().name);
                sell_grey_and_white_items(vendor_guid);
                change_state("recover_state");
                return;
            }
            else
            {
                Log("unable to find vendor ", vendors.back().name, " search next cendor");
                vendors.pop_back();
                walker.set_position(vendors.back().position);
                Log("walk to ventor ", vendors.back().name, vendors.back().subname, " at ", vendors.back().position.to_str());
            }
        }
    }

private:
    void sell_grey_and_white_items(uint64_t vendor_guid)
    {

        call_func_in_game_thread([vendor_guid]() {
            WoWLocalPlayer().set_target(vendor_guid);
        });
        Log("set target to vendor");

        sleep(2);

        call_func_in_game_thread([]() {
            WoWLocalPlayer().interact_target();
        });
        Log("interact done");
        sleep(2);

        call_func_in_game_thread([]() {
            auto items = WoWInventory().get_items();
            auto gray_items = filter(items, [](auto i) { return i.rarity <= item_rarity_white; });
            for (auto i : gray_items)
                i.use();
        });
        Log("sell grey items done");
    }

    std::vector<db_vendor> sort_vendors()
    {
        database db;
        auto vendors = db.get_vendors(Navigation::get_map_id());
        std::sort(vendors.begin(), vendors.end(), [](db_vendor d1, db_vendor d2) {
            return cache::local_player.position.distance(d1.position) > cache::local_player.position.distance(d2.position);
        });
        Log("found ", vendors.size(), " vendors");
        return vendors;
    }
};
