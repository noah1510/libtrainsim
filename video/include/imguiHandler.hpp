#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

#include "video_config.hpp"

#include <glad/glad.h>
#include "imgui.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h")
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
    #include <SDL_image.h>
    #include <SDL_opengl.h>
#else
    #error "cannot include sdl2" 
#endif

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dimensions.hpp"

namespace libtrainsim{
    namespace Video{
        class imguiHandler{
          private:
            std::string glsl_version = "#version 410 core";
            SDL_GLContext gl_context;
            SDL_Window* window = nullptr;
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
              
            imguiHandler();
            ~imguiHandler();
              
            static imguiHandler& getInstance(){
                static imguiHandler instance{};
                return instance;
            }
            
            //lock the io while rendering a frame
            std::mutex IOLock;
            
            //the last size the viewport was set to
            dimensions lastViewportSize = {0,0};
            
            //set the viewport to the given size
            void setViewport(const dimensions& viewportSize);
            
            //force the viewport to be updated even if the viewport size is the same as the last
            bool forceViewportUpdate = true;
            
            void init_impl();
            void startRender_impl();
            void endRender_impl();
            void initFramebuffer_impl(unsigned int& FBO, unsigned int& texture, dimensions dims );
            void loadFramebuffer_impl ( unsigned int buf, dimensions dims );
            void updateRenderThread_impl();
            
            std::thread::id mainThreadID;
            
          public:
            static void init(){
                getInstance().init_impl();
            }
            
            static void startRender(){
                try{
                    errorOffThread();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Cannot start rendering a frame"));
                }
                getInstance().startRender_impl();
            }
            
            static void endRender(){
                try{
                    errorOffThread();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Cannot end rendering a frame"));
                }
                getInstance().endRender_impl();
            }
            
            static void initFramebuffer(unsigned int& FBO, unsigned int& texture, dimensions dims = dimensions{3840,2160}){
                getInstance().initFramebuffer_impl(FBO,texture,dims);
            }
            
            static void loadFramebuffer ( unsigned int buf, dimensions dims = dimensions{3840,2160} ){
                getInstance().loadFramebuffer_impl(buf, dims);
            }
            
            static void updateRenderThread(){
                getInstance().updateRenderThread_impl();
            }
            
            
            /**
             * @brief display a warning when opengl operations are done outside of the main thread
             */
            static void warnOffThread(){
                if(getMainTreadID() != std::this_thread::get_id()){
                    std::cerr << "creating a framebuffer outside of the main thread! This may work or not depending on the driver and os be careful with this!" << std::endl;
                }
            }
            
            /**
             * @brief throw an error if certain operations are done off thread.
             */
            static void errorOffThread(){
                if(getMainTreadID() != std::this_thread::get_id()){
                    std::cerr << "make sure to only call the render on the main thread or update the render thread" << std::endl;
                    throw std::runtime_error("make sure to only call the render on the main thread or update the render thread");
                }
            }
            
            static const std::thread::id& getMainTreadID(){
                return getInstance().mainThreadID;
            }
            
            static std::mutex& getIOLock(){
                return getInstance().IOLock;
            }
            
        };
    }
}
