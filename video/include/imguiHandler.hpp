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

extern "C" {
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
    #include <SDL_image.h>
    #include <SDL_opengl.h>
}

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
        
        /**
         * @brief the settings page for the style settings
         * 
         */
        class styleSettings : public tabPage{
          private:
            void displayContent() override;
          public:
            styleSettings();
        };
        
        /**
         * @brief the settings page for the basic settings
         */
        class basicSettings : public tabPage{
          private:
            void displayContent() override;
          public:
            basicSettings();
        };
        
        /**
         * @brief a class to handle of the general display code
         * 
         */
        class imguiHandler{
          friend class basicSettings;
          friend class styleSettings;
          private:
            //The glsl version that is required to be used by shaders
            std::string glsl_version = "#version 410 core";
            
            /**
             * @brief The (main) gl context
             * @warning this might be removed or at least changed in the future when more of the docking features are used
             */
            SDL_GLContext gl_context;
            
            /**
             * @brief The (main) sdl window
             * @warning this might be removed or at least changed in the future when more of the docking features are used
             */
            SDL_Window* window = nullptr;
            
            /**
             * @brief the background color of the main window
             * @warning this might be removed or at least changed in the future when more of the docking features are used
             */
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            
            /**
             * @brief control if the settings window is shown
             * 
             */
            bool displayImGUiSettings = false;
            
            /**
             * @brief create the imguiHandler
             */
            imguiHandler(std::string windowName);
            
            /**
             * @brief destroy the imguiHandler
             */
            ~imguiHandler();
              
            /**
             * @brief return access to the only instance
             * This turns this class into a singleton, a class with only one instance of it
             */
            static imguiHandler& getInstance(std::string windowName = "libtrainsim window"){
                static imguiHandler instance{windowName};
                return instance;
            }
            
            //all of the other buffers needed for the shaders
            unsigned int VBO = 0, VAO = 0, EBO = 0;
            
            /**
             * @brief the basic copy shader
             * 
             * This shader can copy one texture onto any fbo
             */
            std::shared_ptr<Shader> copyShader;
            
            /**
             * @brief the basic color draw shader
             * 
             * This shader allows drawing a static color onto any fbo
             */
            std::shared_ptr<Shader> drawShader;
            
            //true if the opengl shader components are fully loaded
            bool shaderLoaded = false;
            
            //The textures with specific amounts of darken
            std::array<std::shared_ptr<libtrainsim::Video::texture>,255> darkSteps;
            
            //a texture that does a displacement of 0
            std::shared_ptr<libtrainsim::Video::texture> displacement0;
            
            //the last size the viewport was set to
            dimensions lastViewportSize = {0,0};
            
            //set the viewport to the given size
            void setViewport(const dimensions& viewportSize);
            
            //force the viewport to be updated even if the viewport size is the same as the last
            bool forceViewportUpdate = true;
            
            //The maxmimum number of texture units in a fragment shader
            int maxTextureUnits = 0;
            
            //lock the io while rendering a frame
            std::mutex IOLock;
            
            //implementation for the init function
            void init_impl();
            //implementation for the start Render function
            void startRender_impl();
            //implementation for the end Render function
            void endRender_impl();
            //implementation for init framebuffer function
            void initFramebuffer_impl(unsigned int& FBO, unsigned int& texture, dimensions dims );
            //implementation for load framebuffer function
            void loadFramebuffer_impl ( unsigned int buf, dimensions dims, glm::vec4 clearColor );
            //implementation update render thread function
            void updateRenderThread_impl();
            //implementation copy function
            void copy_impl(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture, glm::mat4 transformation);
            //implementation load shaders function
            void loadShaders_impl(const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation);
            //implementation bindVAO function
            void bindVAO_impl();
            //implementation draw rect function
            void drawRect_impl();
            //implementation get darken texture function
            std::shared_ptr<texture> getDarkenTexture_impl(uint8_t strength);
            //implementation drawColor function
            void drawColor_impl(std::shared_ptr<texture> dest, glm::vec4 color);
            
            /**
             * @brief generate a darken texture with a given strength
             */
            void initDarkenTexture(uint8_t strength);
            
            /**
             * @brief true if the program should end
             */
            bool teminateProgram = false;
            
            /**
             * @brief the id of the main thread
             * @note when opengl functions are called on a different thread a warning or error will happen.
             * This variable keeps thrack of what thread is supposed to work. Use updateRenderThread() to change this.
             */
            std::thread::id mainThreadID;
            
            /**
             * @brief all of the tabs for the settings window
             */
            std::vector<std::shared_ptr<tabPage>> settingsTabs;
            
            //the default size that FBOs should have when being created
            dimensions defaultFBOSize = {3840,2160};
            
          public:
            /**
             * @brief make sure the imguiHandler is loaded
             * @note if any other function is called before this, nothing will happen
             * @param windowName The name the main window should be created with
             */
            static void init(std::string windowName = "libtrainsim window"){
                getInstance(windowName).init_impl();
            }
            
            /**
             * @brief start rendering a frame
             * @warning do not try to draw imgui stuff before calling this once.
             * @details This starts the rendering of a new frame. After calling this function
             *          it is allowed to freely call ImGui and openGL functions.
             * @throws std::runtime_error If this is called outside of the main thread
             */
            static void startRender(){
                try{
                    errorOffThread();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Cannot start rendering a frame"));
                }
                getInstance().startRender_impl();
            }
            
            /**
             * @brief finish rendering a frame
             * @warning do not try to draw imgui stuff after calling this once.
             * @details This finishes the rendering of a new frame. After calling this function
             *          the buffers will be swapped and the most recent frame displayed.
             * @throws std::runtime_error If this is called outside of the main thread
             */
            static void endRender(){
                try{
                    errorOffThread();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Cannot end rendering a frame"));
                }
                getInstance().endRender_impl();
            }
            
            /**
             * @brief initialize a framebuffer with a given texture attached to it
             */
            static void initFramebuffer(unsigned int& FBO, unsigned int& texture, dimensions dims = dimensions{3840,2160}){
                try{
                    getInstance().initFramebuffer_impl(FBO,texture,dims);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Cannot init framebuffer"));
                }
            }
            
            /**
             * @brief make a given framebuffer active
             */
            static void loadFramebuffer ( unsigned int buf, dimensions dims = dimensions{3840,2160}, glm::vec4 clearColor = {0,0,0,0} ){
                getInstance().loadFramebuffer_impl(buf, dims, clearColor);
            }
            
            /**
             * @brief switch the opengl context to a different thread
             */
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
            
            /**
             * @brief geth the id of the current main render thread
             */
            static const std::thread::id& getMainTreadID(){
                return getInstance().mainThreadID;
            }
            
            /**
             * @brief returns true if the program should end
             */
            static bool shouldTerminate(){
                return getInstance().teminateProgram;
            }
            
            /**
             * @brief get access to a mutex to prevent sending mouse events during the render process
             */
            static std::mutex& getIOLock(){
                return getInstance().IOLock;
            }
            
            //copies the source buffer into the destination buffer
            static void copy(std::shared_ptr<texture> src, std::shared_ptr<texture> dest, bool loadTexture = true, glm::mat4 transformation = glm::mat4(1.0f)){
                try{
                    getInstance().copy_impl(src, dest, loadTexture, transformation);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error copying textrue into frambuffer"));
                }
            }
            
            /**
             * @brief inits opengl buffers and load shaders from filesystem
             * 
             */
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
            //get a darken amount between 0 and 255
            static std::shared_ptr<texture> getDarkenTexture(uint8_t strength = 20){
                return getInstance().getDarkenTexture_impl(strength);
            }
            
            //add a new settings tab
            static void addSettingsTab(std::shared_ptr<tabPage> newTab){
                for(auto& tab:getInstance().settingsTabs){
                    if(tab->getName() == newTab->getName()){
                        throw std::invalid_argument{"a tab with this name already exists in the settings"};
                    }
                }
                getInstance().settingsTabs.emplace_back(newTab);
            }
            
            //remove a settings page from the settings window
            static void removeSettingsTab(std::string_view tabName){
                auto i = getInstance().settingsTabs.begin();
                i+=2;
                while(i != getInstance().settingsTabs.end()){
                    if((*i)->getName() == tabName){
                        getInstance().settingsTabs.erase(i);
                        return;
                    }
                    i++;
                }
            }
            
            //get the size a new FBo should have by default
            static dimensions getDefaultFBOSize(){
                return getInstance().defaultFBOSize;
            }
            
        };
    }
}
