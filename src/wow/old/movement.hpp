#pragma once

#include <inttypes.h>
#ifndef __subhook__
#define __subhook__ 
#include "../third_party/subhook/subhook.c"
#endif
#include "utils.hpp"
#include <sstream>
#include "wow_objectmgr.hpp"
#include "local_player.hpp"

#define ctm_func_addr 0x00727400
#define map_id_addr 0x00AB63BC 
#define zone_id_addr 0x00BD080C

/*

void walk_to(float* position){
    auto ctm_func = reinterpret_cast<__thiscall uint (*)(wow_player*,ctm_action_type, uint64_t*, float*, float)>(ctm_func_addr);
	auto lp = get_local_player();
    uint64_t empty_guid = 0;
    ctm_func(lp,go_to, &empty_guid,position,0);
}


*/