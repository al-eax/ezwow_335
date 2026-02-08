#include <stdio.h>
#include <condition_variable>
#include <unistd.h>
#include <mutex>
#include <experimental/filesystem>
#include <chrono>



//#include "python3.6m/Python.h"

#include "hooks/game_loop_hook.hpp"
#include "hooks/opengl_hook.hpp"
#include "hooks/window_input_hook.hpp"

#include "cache.hpp"

#include "radar/radar.hpp"
#include "grind_bot/grind_bot_ui.hpp"
#include "ui/login_tab.hpp"
#include "ui/main_ui.hpp"
#include "third_party/mysql-modern-cpp/mysql+++/mysql+++.h"
#include "ui/db_tab.hpp"

#include <mysql/mysql.h>
#include <mysql/errmsg.h>

using namespace std::chrono_literals;

static auto login = login_module();
static auto radar = radar_module();
static auto bot = grind_bot_ui();
static auto test = db_module();

void thread_main(){
    std::this_thread::sleep_for(5s);
   
    init_gameloop_hook();
    main_ui::init();
    init_input_hook();
    init_opengl_hook();
    register_game_loop_callback(cache::update);

    while(true){
        try
        {
            main_ui::thread_loop_callback();
        }
        catch(const std::exception& e)
        {
            Log("Exception in main_ui::thread_loop_callback" , e.what());
            std::cerr << e.what() << '\n';
        }
        
    }
}

__attribute__((constructor))
static void init(void)
{
    static auto t = std::thread(thread_main);
}
