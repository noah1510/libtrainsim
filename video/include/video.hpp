#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include "backends/openCVRenderer.hpp"
#include "backends/openCVWindowManager.hpp"
#include "frame.hpp"
#include "genericBackend.hpp"
#include "VideoBackends.hpp"

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
            video(Video::VideoBackendDefinition backend = getDefaultBackend());

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

            #ifdef HAS_SDL_SUPPORT
            void initSDL2();
            #endif

            /**
             * @brief the used video backend
             * 
             */
            Video::VideoBackendDefinition currentBackend;
            
            /**
             * @brief the implementation of the current backend
             */
            std::unique_ptr<Video::videoGeneric> currentBackend_impl;

            /**
             * @brief the name of the window for the simulator
             * 
             */
            std::string windowName = "trainsim";
            
            libtrainsim::Video::genericRenderer fallbackRenderer{};
            libtrainsim::Video::genericWindowManager fallbackWindow{fallbackRenderer};

            static void checkBackend_impl(){
                if(getInstance().currentBackend_impl == nullptr){

                    #ifdef HAS_OPENCV_SUPPORT
                        if(getInstance().currentBackend == Video::VideoBackends::openCV){
                            
                            getInstance().currentBackend_impl = std::make_unique<libtrainsim::Video::videoOpenCV>();

                            return;
                        }
                    #endif

                    #if defined(HAS_FFMPEG_SUPPORT) && defined(HAS_SDL_SUPPORT)
                        if(getInstance().currentBackend == Video::VideoBackends::ffmpeg_SDL2){
                            getInstance().initSDL2();
                            getInstance().currentBackend_impl = std::make_unique<libtrainsim::Video::videoFF_SDL>();

                            return;
                        }
                    #endif

                    std::make_unique<libtrainsim::Video::videoGeneric>(getInstance().fallbackWindow, getInstance().fallbackRenderer);
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
                return getInstance().load_impl(uri);
            }

            /**
             * @brief Get the File Path of the loaded video file
             * 
             * @return std::filesystem::path the filepath to the current video file
             */
            static const std::filesystem::path& getFilePath(){
                return getInstance().getFilePath_impl();
            }

            /**
             * @brief jumps to a given frame in the video and display it
             * 
             * @param frame_num the number of the frame that should be displayed next
             */
            static void gotoFrame(double frame_num){
                checkBackend_impl();
                getInstance().currentBackend_impl->gotoFrame(static_cast<uint64_t>(frame_num));
            }

            /**
             * @brief Get the Backend used at the moment.
             * 
             * @return VideoBackends the currently used backend
             */
            static Video::VideoBackendDefinition getBackend(){
                return getInstance().currentBackend;
            }

            /**
             * @brief Set the video backend of the video singleton
             * 
             * @param backend the new backend to be used
             */
            static void setBackend(Video::VideoBackendDefinition backend){
                getInstance().currentBackend = backend;
            }

            /**
             * @brief Get the Width of the video in pixels
             * 
             * @return double 
             */
            static double getWidth(){
                checkBackend_impl();
                return getInstance().currentBackend_impl->getWidth();
            }

            /**
             * @brief Get the Height of the video in pixels
             * 
             * @return double 
             */
            static double getHight(){
                checkBackend_impl();
                return getInstance().currentBackend_impl->getHight();
            }

            /**
             * @brief Get backend that will be used by default
             * 
             * @return VideoBackends the backend to be used by default
             */
            static Video::VideoBackendDefinition getDefaultBackend(){
                #ifdef HAS_FFMPEG_SUPPORT
                #ifdef HAS_SDL_SUPPORT
                return Video::VideoBackends::ffmpeg_SDL2;
                #endif
                #endif
                
                #ifdef HAS_OPENCV_SUPPORT
                return Video::VideoBackends::openCV;
                #endif

                return Video::VideoBackends::none;
            }

            /**
             * @brief Create a new window with a given name
             * 
             * @param windowName 
             */
            static void createWindow(const std::string& windowName){
                checkBackend_impl();
                getInstance().currentBackend_impl->createWindow(windowName);
            }

            /**
             * @brief just refresh the window contents without changing the displayed content.
             * This function has to be called to have working input on some backends.
             * 
             */
            static void refreshWindow(){
                checkBackend_impl();
                getInstance().currentBackend_impl->refreshWindow();
            }

            //opencv backend specifc opetions
            #ifdef HAS_OPENCV_SUPPORT

            /**
             * @brief Set the Backend of the opencv video capture
             * @warning this is useless if called after load 
             * 
             * @param newBackend 
             */
            static void setCVBackend(cv::VideoCaptureAPIs newBackend){
                checkBackend_impl();
                if(getInstance().currentBackend == Video::VideoBackends::openCV){
                    dynamic_cast<Video::videoOpenCV*>(getInstance().currentBackend_impl.get())->setBackend(newBackend);
                }
            }

            /**
             * @brief Get the Backend of the video capture
             * 
             * @return cv::VideoCaptureAPIs the video capture backend
             */
            static cv::VideoCaptureAPIs getCVBackend(){
                checkBackend_impl();
                if(getInstance().currentBackend == Video::VideoBackends::openCV){
                    return dynamic_cast<Video::videoOpenCV*>(getInstance().currentBackend_impl.get())->getBackend();
                }
                return cv::CAP_ANY;
            }

            #endif

    };
}

