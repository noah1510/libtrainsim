#pragma once

#include <exception>
#include <stdexcept>
#include <string>

#include "imgui.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>

#if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h")
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
    #include <SDL_opengl.h>
#else
    #error "cannot include sdl2" 
#endif

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace libtrainsim{
    namespace Video{
        class imguiHandler{
          private:
            std::string glsl_version = "#version 130";
            SDL_GLContext gl_context;
            SDL_Window* window = nullptr;
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
              
            imguiHandler();
            ~imguiHandler();
              
            static imguiHandler& getInstance(){
                static imguiHandler instance{};
                return instance;
            }
            
            void init_impl();
            void startRender_impl();
            void endRender_impl();
            
          public:
            static void init(){
                getInstance().init_impl();
            }
            
            static void startRender(){
                getInstance().startRender_impl();
            }
            
            static void endRender(){
                getInstance().endRender_impl();
            }
            
        };
    }
}
