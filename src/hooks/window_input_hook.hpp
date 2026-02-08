#include "subhook/subhook.c"
#include "imgui/imgui.h"
#include "utils.hpp"

subhook::Hook handle_window_input_hook;
#define handle_window_input_funcaddr 0x00868d40

enum _input_type : int
{
    mouse_move = 10,
    mouse_up = 13,
    key_down = 7,
    key_pressed = 1,
    key_up = 8,
    mouse_down = 9,
};

enum _special_keys : int
{
    shift_down = 0,
    shift_up = 1,
    space = 32,
    delete_ = 521,
    backspace = 514,
    right_mouse_click = 1,
    left_mouse_click = 4,
};

void setup_keymap()
{
    auto &io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Delete] = ImGuiKey_Delete;
    io.KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;
    io.KeyMap[ImGuiKey_Space] = ImGuiKey_Space;
}

volatile bool input_reset = true;

void __cdecl func_HandleWindowInput_callback(_input_type input_type, int input_val, int mouse_x, int mouse_y)
{

    auto &io = ImGui::GetIO();
    setup_keymap();

    if (true)
    {
        // Log("input_type ", input_type, "  input_val", input_val);

        if (input_type == key_pressed && input_val >= 30 && input_val <= 122)
        {
            io.AddInputCharacterUTF16((uint16_t )input_val);
        }

        io.KeysDown[ImGuiKey_Delete] =      input_val == _special_keys::delete_       && input_type == key_down;
        io.KeysDown[ImGuiKey_Backspace] =   input_val == _special_keys::backspace     && input_type == key_down;
        io.KeysDown[ImGuiKey_Space] =       input_val == _special_keys::space         && input_type == key_down;
        
        //io.KeyShift =                       input_val == _special_keys::shift_down    && input_type == key_down;

        if (input_type == mouse_move)
            io.MousePos = ImVec2(mouse_x, mouse_y);

        if (input_type == mouse_down)
            io.MouseDown[0] = true;

        if (input_type == mouse_up)
            io.MouseDown[0] = false;
    }

    if (io.WantCaptureMouse)
        return;

    auto trampolin = (decltype(func_HandleWindowInput_callback) *)handle_window_input_hook.GetTrampoline();
    return trampolin(input_type, input_val, mouse_x, mouse_y);
}

void init_input_hook()
{
    handle_window_input_hook.Install((void *)handle_window_input_funcaddr, (void *)func_HandleWindowInput_callback);
}