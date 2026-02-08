#pragma once

#include <inttypes.h>
#include <math.h>
#include <sstream>
#include "../third_party/simple-matrix/src/matrix.hpp"
#include "utils.hpp"
#include "../third_party/imgui/imgui.h"
#include "addresses.hpp"

bool world_to_screen(float px, float py, float pz, float &screen_x, float &screen_y)
{
    struct wow_cam_vtable
    {
        uint32_t dontcare;
        __attribute__((stdcall)) void (*get_forward_vector)(float *); //(1)
    };
    struct wow_cam
    {
        wow_cam_vtable *v_table;
        float unknown;
        float position[3];
        float view_mat[9];
        float z_near_plane;
        float z_far_plane;
        float fov;
        float ratio;
        float win_width;
        float win_height;
    };

    int screen_width = ImGui::GetIO().DisplaySize.x;
    int screen_height = ImGui::GetIO().DisplaySize.y;

    auto wowfunc_get_camera_object = reinterpret_cast<wow_cam *(*)()>(Addresses::function_get_camera_ptr);
    auto cam = wowfunc_get_camera_object();
    if (cam == nullptr)
        return false;

    float difference[] = {px - cam->position[0],
                          py - cam->position[1],
                          pz - cam->position[2]};

    double vals[9];
    for (int i = 0; i < 9; i++)
        vals[i] = cam->view_mat[i];
    auto view_mat = simple_matrix::matrix(3, 3, vals);

    float product =
        difference[0] * view_mat.get(0, 0) +
        difference[1] * view_mat.get(0, 1) +
        difference[2] * view_mat.get(0, 2);

    if (product < 0)
        return false;

    auto inverse = view_mat.invert();

    float view[] = {
        float(inverse.get(0, 0) * difference[0] + inverse.get(1, 0) * difference[1] + inverse.get(2, 0) * difference[2]),
        float(inverse.get(0, 1) * difference[0] + inverse.get(1, 1) * difference[1] + inverse.get(2, 1) * difference[2]),
        float(inverse.get(0, 2) * difference[0] + inverse.get(1, 2) * difference[1] + inverse.get(2, 2) * difference[2])};

    float camera[] = {-view[1], -view[2], view[0]};
    float Deg2Rad = M_PI / 180.0f;

    float FOV = cam->fov;
    float gameScreen[] = {screen_width / 2.0f, screen_height / 2.0f};
    float aspect[] = {gameScreen[0] / tan(((FOV * 50.0f) / 2.0f) * Deg2Rad), gameScreen[1] / tan(((FOV * 35.0f) / 2.0f) * Deg2Rad)};

    float screenPos[] = {gameScreen[0] + camera[0] * aspect[0] / camera[2], gameScreen[1] + camera[1] * aspect[1] / camera[2]};

    if (screenPos[0] < 0 || screenPos[0] > screen_width || screenPos[1] < 0 || screenPos[1] > screen_height)
        return false;

    screen_x = screenPos[0];
    screen_y = screenPos[1];

    return true;
}

bool world_to_screen(float *p, float &x, float &y)
{
    return world_to_screen(p[0], p[1], p[2], x, y);
}

bool world_to_screen(float *p, float *screen_pos)
{
    return world_to_screen(p[0], p[1], p[2], screen_pos[0], screen_pos[1]);
}

bool world_to_screen(Vector3d p, float *screen_pos)
{
    return world_to_screen(p.x, p.y, p.z, screen_pos[0], screen_pos[1]);
}

bool world_to_screen(Vector3d p, float& x, float& y)
{
    return world_to_screen(p.x, p.y, p.z, x, y);
}