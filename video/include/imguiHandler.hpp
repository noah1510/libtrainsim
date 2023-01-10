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

#include "defaultShader.hpp"
#include "dimensions.hpp"
#include "video_settings.hpp"

namespace libtrainsim{
    namespace Video{
        //forward declarations
        class texture;
        class Shader;
        
        /**
         * @brief a class to handle of the general display code
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO imguiHandler{
          friend class ::libtrainsim::Video::basicSettings;
          friend class ::libtrainsim::Video::styleSettings;
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
            SDL_Window* sdl_window = nullptr;
            
            /**
             * @brief the background color of the main window
             * @warning this might be removed or at least changed in the future when more of the docking features are used
             */
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            
            //The settings window
            std::shared_ptr<settingsWindow> settingsWin = nullptr;
            
            /**
             * @brief create the imguiHandler
             */
            imguiHandler(std::string windowName);
            
            /**
             * load the shaders and init opengl.
             * This function is now called as part of the constructor, so the opengl functions are
             * always available.
             */
            void loadShaders();
            
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
            
            //The textures with specific amounts of darken
            std::array<std::shared_ptr<libtrainsim::Video::texture>,255> darkSteps;
            
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
            //implementation bindVAO function
            void bindVAO_impl();
            //implementation draw rect function
            void drawRect_impl();
            //implementation get darken texture function
            std::shared_ptr<texture> getDarkenTexture_impl(uint8_t strength);
            //implementation drawColor function
            void drawColor_impl(std::shared_ptr<texture> dest, glm::vec4 color);
            
            void registerWindow_impl(window* _win);
            void unregisterWindow_impl(window* _win);
            
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
            
            //the default size that FBOs should have when being created
            dimensions defaultFBOSize = {3840,2160};
            
            //all the windows registered with the imguiHandler
            //std::vector<std::shared_ptr<libtrainsim::Video::window>> registeredWindows;
            std::vector<libtrainsim::Video::window*> registeredWindows;
            
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
            
            //fill a texture with a given color
            static void drawColor(std::shared_ptr<texture> dest, glm::vec4 color){
                try{
                    getInstance().drawColor_impl(dest, color);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("Error filling with colored"));
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
                try{
                    getInstance().settingsWin->addSettingsTab(newTab);
                }catch(...){
                    std::throw_with_nested(std::invalid_argument("Error adding new settings tab"));
                }
            }
            
            //remove a settings page from the settings window
            static void removeSettingsTab(std::string_view tabName){
                try{
                    getInstance().settingsWin->removeSettingsTab(tabName);
                }catch(...){
                    std::throw_with_nested(std::invalid_argument("Error removing settings tab"));
                }
            }
            
            //get the size a new FBo should have by default
            static dimensions getDefaultFBOSize(){
                return getInstance().defaultFBOSize;
            }
            
            /**
             * @brief check if an opengl error happened
             * 
             * @throws std::runtime_error when there was an openGL error
             */
            static void glErrorCheck(){
                auto errorCode = glGetError();
                if(errorCode != GL_NO_ERROR){
                    auto error = decodeGLError(errorCode);
                    throw std::runtime_error{"Got an openGL error: " + error};
                }
            }
            
            //register a window with the imgui hander
            static void registerWindow(window* _win){
                getInstance().registerWindow_impl(_win);
            }
            
            //have a window no longer registered
            static void unregisterWindow(window* _win){
                getInstance().unregisterWindow_impl(_win);
            }
            
            //get a string describing a GL error in more detail from an error code.
            //@note this does not handle GL_NO_ERROR check for that before calling this function
            static std::string decodeGLError(int errorCode) noexcept{
                std::string error;
                switch (errorCode){
                    #ifdef GL_INVALID_ENUM
                    case GL_INVALID_ENUM:                   error = "INVALID_ENUM"; break;
                    #endif
                    #ifdef GL_INVALID_VALUE
                    case GL_INVALID_VALUE:                  error = "INVALID_VALUE"; break;
                    #endif
                    #ifdef GL_INVALID_OPERATION
                    case GL_INVALID_OPERATION:              error = "INVALID_OPERATION"; break;
                    #endif
                    #ifdef GL_STACK_OVERFLOW
                    case GL_STACK_OVERFLOW:                 error = "STACK_OVERFLOW"; break;
                    #endif
                    #ifdef GL_STACK_UNDERFLOW
                    case GL_STACK_UNDERFLOW:                error = "STACK_UNDERFLOW"; break;
                    #endif
                    #ifdef GL_OUT_OF_MEMORY
                    case GL_OUT_OF_MEMORY:                  error = "OUT_OF_MEMORY"; break;
                    #endif
                    #ifdef GL_INVALID_FRAMEBUFFER_OPERATION
                    case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION"; break;
                    #endif
                    default:                                error = "UNKNOWN_ERROR_TYPE"; break;
                }
                
                return error;
            }
            
            //get a string describing a framebuffer status in more detail from an error code.
            //@note this does not handle GL_FRAMEBUFFER_COMPLETE check for that before calling this function
            static std::string decodeGLFramebufferStatus(int errorCode) noexcept{
                std::string error;
                switch (errorCode){
                    #ifdef GL_FRAMEBUFFER_UNDEFINED
                    case GL_FRAMEBUFFER_UNDEFINED:                      error = "FRAMEBUFFER_UNDEFINED"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
                    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:          error = "FRAMEBUFFER_INCOMPLETE_ATTACHMENT"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
                    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:  error = "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
                    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:         error = "FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
                    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:         error = "FRAMEBUFFER_INCOMPLETE_READ_BUFFER"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_UNSUPPORTED
                    case GL_FRAMEBUFFER_UNSUPPORTED:                    error = "FRAMEBUFFER_UNSUPPORTED"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
                    case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:         error = "FRAMEBUFFER_INCOMPLETE_MULTISAMPLE"; break;
                    #endif
                    #ifdef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
                    case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:       error = "FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS"; break;
                    #endif
                    default:                                            error = "UNKNOWN_ERROR_TYPE"; break;
                }
                
                return error;
            }
            
        };
    }
}
