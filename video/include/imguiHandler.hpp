#pragma once

#include <exception>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <iostream>
#include <mutex>
#include <thread>

#include "video_config.hpp"

extern "C" {
    #include <glad/glad.h>
}

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h") && __has_include("SDL_image.h") && __has_include("SDL_opengl.h")
    extern "C" {
        #include <SDL2/SDL.h>
        #include <SDL2/SDL_thread.h>
        #include <SDL_image.h>
        #include <SDL_opengl.h>
    }
#else
    #error "cannot include sdl2" 
#endif

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "dimensions.hpp"
#include "tabPage.hpp"

namespace libtrainsim{
    namespace Video{
        //forward declarations
        class texture;
        class Shader;
        const std::array<unsigned int, 5> darkenStrengths{0,20,40,60,80};
        
        //the settings page for the detailed style settings
        class styleSettings : public tabPage{
        public:
            styleSettings();
            void displayContent() override;
        };
        
        //the settings page for the basic style settings
        class basicSettings : public tabPage{
        public:
            basicSettings();
            void displayContent() override;
        };
        
        //a class to handle all of the general display code
        class imguiHandler{
          friend class basicSettings;
          private:
            std::string glsl_version = "#version 410 core";
            SDL_GLContext gl_context;
            SDL_Window* window = nullptr;
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            bool displayImGUiSettings = false;
              
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
            
            //The textures with specific amounts of darken
            std::vector<std::shared_ptr<libtrainsim::Video::texture>> darkSteps;
            
            //a texture that does a displacement of 0
            std::shared_ptr<libtrainsim::Video::texture> displacement0;
            
            //the last size the viewport was set to
            dimensions lastViewportSize = {0,0};
            
            //set the viewport to the given size
            void setViewport(const dimensions& viewportSize);
            
            //force the viewport to be updated even if the viewport size is the same as the last
            bool forceViewportUpdate = true;
            
            int maxTextureUnits = 0;
            
            std::mutex IOLock;
            //lock the io while rendering a frame
            
            void init_impl();
            void startRender_impl();
            void endRender_impl();
            void initFramebuffer_impl(unsigned int& FBO, unsigned int& texture, dimensions dims );
            void loadFramebuffer_impl ( unsigned int buf, dimensions dims );
            void updateRenderThread_impl();
            void copy_impl(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture, glm::mat4 transformation);
            void loadShaders_impl(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation);
            void bindVAO_impl();
            void drawRect_impl();
            std::shared_ptr<texture> getDarkenTexture_impl(unsigned int strength);
            
            bool teminateProgram = false;
            
            std::thread::id mainThreadID;
            
            std::vector<std::unique_ptr<tabPage>> settingsTabs;
            
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
            
            static bool shouldTerminate(){
                return getInstance().teminateProgram;
            }
            
            static std::mutex& getIOLock(){
                return getInstance().IOLock;
            }
            
            //copies the output buffer into the input buffer
            static void copy(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture = true, glm::mat4 transformation = glm::mat4(1.0f)){
                try{
                    getInstance().copy_impl(src, dest, loadTexture, transformation);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error copying textrue into frambuffer"));
                }
            }
            
            //load the shaders and other buffer objects
            static void loadShaders(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation){
                try{
                    getInstance().loadShaders_impl(shaderLocation, textureLocation);
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
            
            //get the maxmimum number of textures in a texture unit
            static unsigned int getMaxTextureUnits(){
                return static_cast<unsigned int>( getInstance().maxTextureUnits );
            }
            
            //draw a rectange using the bound shader, FBO and texture
            static void drawRect(){
                try{
                    getInstance().drawRect_impl();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error drawing a rectange"));
                }
            }
            
            //get access to the copy shader to do more complex operations manually
            static std::shared_ptr<Shader> getCopyShader(){
                return getInstance().copyShader;
            }
            
            //get access to the texture to darken a texture
            static std::shared_ptr<texture> getDarkenTexture(unsigned int strength = 20){
                return getInstance().getDarkenTexture_impl(strength);
            }
            
        };
    }
}
