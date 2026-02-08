#pragma once

#include "subhook/subhook.c"
#include "imgui/imgui.h"

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include <dlfcn.h>
#include <sstream>

//OPENGL SWAP BUFFERS
subhook::Hook glXSwapBuffers_hook;

static std::vector<std::function<void()>> callback_funcs_ogl;

void register_opengl_loop_callback(std::function<void()> func)
{
    callback_funcs_ogl.push_back(func);
}

GLXContext my_context;
GLXContext game_context;
Display *display;
GLXDrawable current_drawable;

void CreateOwnContext()
{
    int screen = -1;
    glXQueryContext(display, game_context, GLX_SCREEN, &screen);
    int attribs[] = {GLX_FBCONFIG_ID, -1, None};
    int dummy;
    glXQueryContext(display, game_context, GLX_FBCONFIG_ID, &attribs[1]);
    GLXFBConfig *fb = glXChooseFBConfig(display, screen, attribs, &dummy);
    XVisualInfo *vis = glXGetVisualFromFBConfig(display, *fb);
    my_context = glXCreateContext(display, vis, 0, True);
}

bool SwitchOglContext()
{
    if (display == nullptr)
        display = glXGetCurrentDisplay();
    game_context = glXGetCurrentContext();
    current_drawable = glXGetCurrentDrawable();
    if (!my_context)
        CreateOwnContext();

    glXMakeCurrent(display, current_drawable, my_context);
    glEnable(GL_DEPTH_TEST);

    uint32_t w, h;
    glXQueryDrawable(display, current_drawable, GLX_WIDTH, &w);
    glXQueryDrawable(display, current_drawable, GLX_HEIGHT, &h);
    glViewport(0, 0, w, h);
    return true;
}

void glXSwapBuffers_callback(Display *dpl, GLXDrawable drawable)
{
    if (!SwitchOglContext())
        return;

    uint32_t w, h;
    glXQueryDrawable(display, current_drawable, GLX_WIDTH, &w);
    glXQueryDrawable(display, current_drawable, GLX_HEIGHT, &h);

    auto &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(w, h);
    //draw imgui stuff

    for (auto f : callback_funcs_ogl){
        try
        {
            f();
        }
        catch(const std::exception& e)
        {
            Log("Exception in glXSwapBuffers_callback while calling callback func" , e.what());
            std::cerr << e.what() << '\n';
        }
    }

    //end imgui stuff
    glXMakeCurrent(display, current_drawable, game_context); // undo SwitchOglContext

    auto trampolin = (decltype(glXSwapBuffers_callback) *)glXSwapBuffers_hook.GetTrampoline();
    trampolin(dpl, drawable);
}

void init_opengl_hook()
{
    // wait until glXGetProcAddressARB is loaded
    while (dlsym(RTLD_NEXT, "glXGetProcAddressARB") == 0)
    {
    }

    auto glXGetProcAddressARB = reinterpret_cast<void *(*)(const char *)>(dlsym(RTLD_NEXT, "glXGetProcAddressARB"));
    glXSwapBuffers_hook.Install((void *)glXGetProcAddressARB("glXSwapBuffers"), (void *)glXSwapBuffers_callback);
}
