#pragma once 

#include "mysql-modern-cpp/mysql+++/mysql+++.h"
#include <vector>
#include "utils.hpp"

using namespace daotk::mysql;

struct db_vendor
{
    uint64_t guid;
    std::string name;
    std::string subname;
    Vector3d position;
};

enum quest_allowable_races{
    all = 0,
    horde = 690,
    alliance = 1101
};

struct db_quest{
    uint32_t id;
    uint32_t min_level;
    uint32_t max_level;
    uint32_t quest_level;
    quest_allowable_races allowable_race;
    std::string title;
    std::string description;
};

class database
{
    connection client;

public:
    database(bool connect = true) { if(connect) this->connect(); }
    ~database() { close(); }

    bool connect()
    {
        unsigned int port = 6603;
        if(client.open(connect_options("127.0.0.1", "root", "root", "world", 1000U, true, "", "", port, 0))){
            return true;
        }
        Log("unable to connect to database");
        return true;
    }

    bool is_connected()
    {
        return client.is_open();
    }

    void close()
    {
        client.close();
    }

    std::vector<db_vendor> get_vendors(int map_id)
    {
        std::vector<db_vendor> vendors;
        if(!is_connected()){
            Log("sql client not connected!");
            return vendors;  
        }

        auto result = client.query(
            R"(
                select DISTINCT c.guid , ct.name , ct.subname , c.position_x , c.position_y ,c.position_z
                from creature_template ct, creature c 
                where c.id = ct.entry AND  c.`map` = %d AND ct.npcflag & 0x00000080 > 0 AND NOT LOWER(ct.name) LIKE "[DND]%";
            )"
        ,map_id);

        for (auto &row : result.as_container<uint64_t, std::string, std::string, float, float, float>())
        {
            auto [guid, name, subname, posx, posy, posz] = row;
            vendors.push_back(db_vendor{guid, name, subname, Vector3d(posx, posy, posz)});
        }
        return vendors;
    }


};