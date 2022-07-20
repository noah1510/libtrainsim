#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <shared_mutex>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>

#include "frame.hpp"
#include "videoDecoder.hpp"

#if  __has_include("SDL2/SDL.h") && __has_include("SDL2/SDL_thread.h")
    #include <SDL2/SDL.h>
    #include <SDL2/SDL_thread.h>
#else
    #error "cannot include sdl2" 
#endif

namespace libtrainsim {
    namespace Video{

        /**
        * @brief This class is resposiblie for managing all the video material.
        * 
        */
        class videoManager{
            private:

                /**
                * @brief reset the singleton to the initial state
                * 
                */
                void reset();

                /**
                 * @brief init sdl2 and all of its components
                 */
                void initSDL2();
                
                SDL_Window* screen = nullptr;
                SDL_Renderer* sdl_renderer = nullptr;
                SDL_Texture* texture = nullptr;
                
                std::string currentWindowName = "";
                bool windowFullyCreated = false;
                std::shared_ptr<Frame> lastFrame;
                
                std::shared_mutex videoMutex;
                std::future<std::shared_ptr<libtrainsim::Video::Frame>> nextFrame;
                bool fetchingFrame = false;
                uint64_t nextFrameToGet = 0;
                
                /**
                 * @brief the decoder used to decode the video file into frames
                 */
                videoDecoder decode;
                
                /**
                * @brief the name of the window for the simulator
                * 
                */
                std::string windowName = "trainsim";

            public:
                /**
                * @brief Construct a new video object (must only be called by getInstance when necessary)
                * 
                */
                videoManager();

                /**
                * @brief Destroy the video object, on destruction everything will be reset.
                * 
                */
                ~videoManager();

                /**
                * @brief Load a video file into the video management.
                * 
                * @param uri The uri of the file.
                * @return true file sucessfully loaded
                * @return false error while loading file
                */
                bool load(const std::filesystem::path& uri);

                /**
                * @brief Get the File Path of the loaded video file
                * 
                * @return std::filesystem::path the filepath to the current video file
                */
                const std::filesystem::path& getFilePath() const;

                /**
                * @brief jumps to a given frame in the video and display it
                * 
                * @param frame_num the number of the frame that should be displayed next
                */
                void gotoFrame(uint64_t frame_num);

                /**
                * @brief Get the Width of the video in pixels
                * 
                * @return double 
                */
                double getWidth();

                /**
                * @brief Get the Height of the video in pixels
                * 
                * @return double 
                */
                double getHeight();

                /**
                * @brief Create a new window with a given name
                * 
                * @param windowName 
                */
                void createWindow(const std::string& windowName);

                /**
                * @brief just refresh the window contents without changing the displayed content.
                * This function has to be called to have working input on some backends.
                * 
                */
                void refreshWindow();
                
                /**
                * @brief check if the end of a video file is reached.
                * Use this function to stop the simulator if the video file is over.
                * The simulator will hang or crash once the end is reached and it still tries to render new frames.
                * 
                * @return true The end of the video file is reached
                * @return false The video file is not at the end yet
                */
                bool reachedEndOfFile();
                
        };
    }
}

