#pragma once

#include <inttypes.h>
#include <functional>
#include <vector>
#include <atomic>
#include "utils.hpp"

#include "subhook/subhook.c"

#define game_loop_func_address 0x0047eff0
subhook::Hook game_loop_hook;

std::vector<std::function<void()>> __funcs_to_call_in_game_thread;
std::vector<std::function<void()>> callback_funcs;

static volatile bool reset = false;

void register_game_loop_callback(std::function<void()> func)
{
    callback_funcs.push_back(func);
}

void call_func_in_game_thread(std::function<void()> func, bool block_until_call = true)
{
    __funcs_to_call_in_game_thread.push_back(func);
    reset = false;
    if (block_until_call)
        while (!reset)
            sleep(0.01f); // required to read __funcs_to_call_in_game_thread
}

//  #define CALL_IN_GAME_TH(FUNC) call_in_game_thread_2<decltype(&FUNC), &FUNC>

int game_loop_callback(int p1, int *p2)
{
    for (auto f : __funcs_to_call_in_game_thread){
        try
        {
            f();
        }
        catch(const std::exception& e)
        {
            Log("Exception in game_loop_callback while calling __funcs_to_call_in_game_thread" , e.what());
            std::cerr << e.what() << '\n';
        }
    }
       
    __funcs_to_call_in_game_thread.clear();
    reset = true;
    for (auto f : callback_funcs){
        try
        {
            f();
        }
        catch(const std::exception& e)
        {
            Log("Exception in game_loop_callback while calling callback_funcs" , e.what());
            std::cerr << e.what() << '\n';
        }      

    }
    auto trampolin = (decltype(game_loop_callback) *)game_loop_hook.GetTrampoline();
    return trampolin(p1, p2);
}

void init_gameloop_hook()
{
    game_loop_hook.Install((void *)game_loop_func_address, (void *)game_loop_callback);
}
