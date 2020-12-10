#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include "frame.hpp"
#include "backends/generic.hpp"

#ifdef HAS_OPENCV_SUPPORT
#include "backends/opencv.hpp"
#endif

#ifdef HAS_FFMPEG_SUPPORT
    #ifdef HAS_SDL_SUPPORT
    #include "backends/ffmpeg_sdl.hpp"
    #endif
#endif


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
            video(VideoBackends backend = getDefaultBackend());

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

            /**
             * @brief the used video backend
             * 
             */
            VideoBackends currentBackend;
            
            /**
             * @brief the implementation of the current backend
             */
            std::unique_ptr<backend::videoGeneric> currentBackend_impl;

            /**
             * @brief the name of the window for the simulator
             * 
             */
            std::string windowName = "trainsim";
            
            static void checkBackend_impl(){
                if(getInstance().currentBackend_impl == nullptr){
                    switch(getInstance().currentBackend){
                        case(none):
                            getInstance().currentBackend_impl = std::make_unique<libtrainsim::backend::videoGeneric>();
                            break;
                        #ifdef HAS_OPENCV_SUPPORT
                        case(opencv):
                            getInstance().currentBackend_impl = std::make_unique<libtrainsim::backend::videoOpenCV>();
                            break;
                        #endif
                        #ifdef HAS_FFMPEG_SUPPORT
                        case(ffmpeg):
                        #ifdef HAS_SDL_SUPPORT
                        case(ffmpeg_sdl):
                            getInstance().currentBackend_impl = std::make_unique<libtrainsim::backend::videoFF_SDL>();
                            break;
                        #endif
                            break;
                        #endif
                        default:
                            return;
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
                getInstance().currentBackend_impl->gotoFrame(frame_num);
            }

            /**
             * @brief Get the Backend used at the moment.
             * 
             * @return VideoBackends the currently used backend
             */
            static VideoBackends getBackend(){
                return getInstance().currentBackend;
            }

            /**
             * @brief Set the video backend of the video singleton
             * 
             * @param backend the new backend to be used
             */
            static void setBackend(VideoBackends backend){
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
            static VideoBackends getDefaultBackend(){
                #ifdef HAS_FFMPEG_SUPPORT
                #ifdef HAS_SDL_SUPPORT
                return ffmpeg_sdl;
                #endif
                #endif
                
                #ifdef HAS_OPENCV_SUPPORT
                return opencv;
                #endif

                return none;
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
                if(getInstance().currentBackend == opencv){
                    ((backend::videoOpenCV*)getInstance().currentBackend_impl.get())->setBackend(newBackend);
                }
            }

            /**
             * @brief Get the Backend of the video capture
             * 
             * @return cv::VideoCaptureAPIs the video capture backend
             */
            static cv::VideoCaptureAPIs getCVBackend(){
                checkBackend_impl();
                if(getInstance().currentBackend == opencv){
                    return ((backend::videoOpenCV*)getInstance().currentBackend_impl.get())->getBackend();
                }
                return cv::CAP_ANY;
            }

            #endif

    };
}

