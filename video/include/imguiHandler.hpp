#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <iostream>

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
        //forward declarations
        class texture;
        class Shader;
        
        class imguiHandler{
          private:
            std::string glsl_version = "#version 460 core";
            SDL_GLContext gl_context;
            SDL_Window* window = nullptr;
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
              
            imguiHandler();
            ~imguiHandler();
              
            static imguiHandler& getInstance(){
                static imguiHandler instance{};
                return instance;
            }
            
            //all of the other buffers needed for the shaders
            unsigned int VBO = 0, VAO = 0, EBO = 0;
            
            std::shared_ptr<Shader> copyShader;
            bool shaderLoaded = false;
            
            int maxTextureUnits = 0;
            
            void init_impl();
            void startRender_impl();
            void endRender_impl();
            void initFramebuffer_impl(unsigned int& FBO, unsigned int& texture, dimensions dims );
            void loadFramebuffer_impl ( unsigned int buf, dimensions dims );
            void updateRenderThread_impl();
            void copy_impl(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture);
            void loadShaders_impl(const std::filesystem::path& shaderLocation);
            void bindVAO_impl();
            void drawRect_impl();
            
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
            
            static void initFramebuffer(unsigned int& FBO, unsigned int& texture, dimensions dims = dimensions{3840,2160}){
                getInstance().initFramebuffer_impl(FBO,texture,dims);
            }
            
            static void loadFramebuffer ( unsigned int buf, dimensions dims = dimensions{3840,2160} ){
                getInstance().loadFramebuffer_impl(buf, dims);
            }
            
            static void updateRenderThread(){
                getInstance().updateRenderThread_impl();
            }
            
            //copies the output buffer into the input buffer
            static void copy(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture = true){
                try{
                    getInstance().copy_impl(src, dest, loadTexture);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error copying textrue into frambuffer"));
                }
            }
            
            //load the shaders and other buffer objects
            static void loadShaders(const std::filesystem::path& shaderLocation){
                try{
                    getInstance().loadShaders_impl(shaderLocation);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error copying textrue into frambuffer"));
                }
            }
            
            //bind the basic VAO for opengl draw calls
            static void bindVAO(){
                try{
                    getInstance().bindVAO_impl();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error binding the VAO"));
                }
            }
            
            static unsigned int getMaxTextureUnits(){
                return static_cast<unsigned int>( getInstance().maxTextureUnits );
            }
            
            static void drawRect(){
                try{
                    getInstance().drawRect_impl();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error drawing a rectange"));
                }
            }
            
        };
    }
}
