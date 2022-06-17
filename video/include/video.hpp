#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include "frame.hpp"
#include "genericBackend.hpp"
#include "backends/ffmpeg_sdl.hpp"

namespace libtrainsim {    

    /**
     * @brief This class is resposiblie for managing all the video material.
     * 
     */
    class video{
        private:
            /**
             * @brief Construct a new video object (must only be called by getInstance when necessary)
             * 
             */
            video();

            /**
             * @brief Destroy the video object, on destruction everything will be reset.
             * 
             */
            ~video(){
                reset();
            }

            /**
             * @brief reset the singleton to the initial state
             * 
             */
            void reset();

            /**
             * @brief Get the Instance of the singleton
             * 
             * @return video& a reference to the object
             */
            static video& getInstance(){
                static video instance;
                return instance;
            };

            /**
             * @brief The implementation of hello()
             * 
             */
            std::string hello_impl() const;

            /**
             * @brief The implementation of load look @load for details
             */
            bool load_impl(const std::filesystem::path& uri);

            ///The implementation for the getFilePath method
            const std::filesystem::path& getFilePath_impl() const;

            void initSDL2();
            
            /**
             * @brief the implementation of the current backend
             */
            std::unique_ptr<Video::videoGeneric> currentBackend_impl;

            /**
             * @brief the name of the window for the simulator
             * 
             */
            std::string windowName = "trainsim";
            
            std::shared_ptr<libtrainsim::Video::genericRenderer> fallbackRenderer = std::make_shared<libtrainsim::Video::genericRenderer>();
            std::shared_ptr<libtrainsim::Video::genericWindowManager> fallbackWindow = std::make_shared<libtrainsim::Video::genericWindowManager>(fallbackRenderer);

            static void checkBackend_impl(){
                if(getInstance().currentBackend_impl == nullptr){

                    try{
                        getInstance().initSDL2();
                        getInstance().currentBackend_impl = std::make_unique<libtrainsim::Video::videoFF_SDL>();

                        return;

                        std::make_unique<libtrainsim::Video::videoGeneric>(getInstance().fallbackWindow, getInstance().fallbackRenderer);
                    }catch(...){
                        std::throw_with_nested(std::runtime_error("could not create instance"));
                    }
                    
                }
            }

        public:
            /**
             * @brief This function is used to test if libtrainsim::video was correctly linked and just prints hello world
             * 
             */
            static std::string hello(){
                return getInstance().hello_impl();
            }

            /**
             * @brief Load a video file into the video management.
             * 
             * @param uri The uri of the file.
             * @return true file sucessfully loaded
             * @return false error while loading file
             */
            static bool load(const std::filesystem::path& uri){
                try{
                    return getInstance().load_impl(uri);
                }catch(...){
                    std::throw_with_nested(std::runtime_error("error loading track config"));
                }
            }

            /**
             * @brief Get the File Path of the loaded video file
             * 
             * @return std::filesystem::path the filepath to the current video file
             */
            static const std::filesystem::path& getFilePath(){
                try{
                    return getInstance().getFilePath_impl();
                }catch(...){
                    std::throw_with_nested(std::runtime_error("error getting file path"));
                }
            }

            /**
             * @brief jumps to a given frame in the video and display it
             * 
             * @param frame_num the number of the frame that should be displayed next
             */
            static void gotoFrame(double frame_num){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                getInstance().currentBackend_impl->gotoFrame(static_cast<uint64_t>(frame_num));
            }

            /**
             * @brief Get the Width of the video in pixels
             * 
             * @return double 
             */
            static double getWidth(){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                return getInstance().currentBackend_impl->getWidth();
            }

            /**
             * @brief Get the Height of the video in pixels
             * 
             * @return double 
             */
            static double getHight(){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                return getInstance().currentBackend_impl->getHight();
            }

            /**
             * @brief Create a new window with a given name
             * 
             * @param windowName 
             */
            static void createWindow(const std::string& windowName){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                getInstance().currentBackend_impl->createWindow(windowName);
            }

            /**
             * @brief just refresh the window contents without changing the displayed content.
             * This function has to be called to have working input on some backends.
             * 
             */
            static void refreshWindow(){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                getInstance().currentBackend_impl->refreshWindow();
            }
            
            /**
             * @brief check if the end of a video file is reached.
             * Use this function to stop the simulator if the video file is over.
             * The simulator will hang or crash once the end is reached and it still tries to render new frames.
             * 
             * @return true The end of the video file is reached
             * @return false The video file is not at the end yet
             */
            static bool reachedEndOfFile(){
                try{checkBackend_impl();} catch(...){std::throw_with_nested(std::runtime_error("error loading backend implementation"));};
                return getInstance().currentBackend_impl->getRenderer()->reachedEndOfFile();
            }
            
            
    };
}

