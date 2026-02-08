#pragma once

#include "wow/local_player.hpp"
#include "wow/wow_object.hpp"


struct combat_profile{
    
    std::string profile_name = "";
    int max_target_distance = 5;

    virtual void enter_fight( WoWLocalPlayer lp, WoWUnit target){}
    virtual void fight (WoWLocalPlayer lp ,WoWUnit target){}
    virtual void leave_fight( WoWLocalPlayer lp ,WoWUnit target){}
};


struct druid_combat_profile : public combat_profile{

    druid_combat_profile()  {
        this->profile_name = "simple druid combar class";
        this->max_target_distance = 6;
    }

    void enter_fight(WoWLocalPlayer lp,WoWUnit target){
       
    }

    void fight(WoWLocalPlayer lp, WoWUnit target){
        if(!lp.is_casting() || lp.is_channeling())
            lp.cast_spell("Zorn");
    }

    void leave_fight(WoWLocalPlayer lp,WoWUnit target){
      
    }

};