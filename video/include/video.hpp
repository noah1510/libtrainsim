#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <shared_mutex>
#include <thread>
#include <future>
#include <iostream>
#include <chrono>
#include <vector>

#include "video_reader.hpp"

#include "imguiHandler.hpp"
#include "shader.hpp"
#include "helper.hpp"
#include "texture.hpp"

#include "unit_system.hpp"

namespace libtrainsim {
    namespace Video{

        /**
        * @brief This class is resposiblie for managing all the video material.
        * 
        */
        class videoManager{
            private:
                
                //the buffer and texture for the creation of the rgb video image
                std::shared_ptr<texture> outputBuffer;
                
                /**
                 * @brief the shader used to render the video into a texture
                 */
                std::shared_ptr<Shader> displayShader = nullptr;
                
                /**
                 * @brief the name this window currently has
                 */
                std::string currentWindowName = "";
                
                //true if the simulator window is fully started
                bool windowFullyCreated = false;
                
                //a mutex to secure this class for multithreading
                std::shared_mutex videoMutex;
                
                /**
                 * @brief the decoder used to decode the video file into frames
                 */
                std::unique_ptr<videoReader> decode = nullptr;
                
                //update the output Texture
                void updateOutput();
                
                //all of the textures that are displayed on the output texture 
                std::vector< std::shared_ptr<texture> > displayTextures;

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
                void load(const std::filesystem::path& uri);

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
                * @param windowName the name the window is going to have
                * @param shaderLocation The folder where all of the shader files are stored
                */
                void createWindow(const std::string& windowName, const std::filesystem::path& shaderLocation, const std::filesystem::path& textureLocation);

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
                
                /**
                 * @brief adds a texture to be rendered on top of the video
                 */
                void addTexture(std::shared_ptr<texture> newTexture);
                
                /**
                 * @brief remove a texture from being rendered
                 */
                void removeTexture(const std::string& textureName);
                
                //the the rendertimes of the video
                std::optional< std::vector<sakurajin::unit_system::time_si> > getNewRendertimes();
                
        };
    }
}

