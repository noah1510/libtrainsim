#pragma once

#include <memory>
#include <string>
#include <filesystem>

#include "frame.hpp"

#ifdef HAS_OPENCV_SUPPORT
#include "backends/opencv.hpp"
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

            /**
             * @brief This is that path to the currently loaded file.
             * 
             */
            std::filesystem::path loadedFile;

            ///The implementation for the getFilePath method
            std::filesystem::path getFilePath_impl() const;

            /**
             * @brief the used video backend
             * 
             */
            VideoBackends currentBackend;

            /**
             * @brief the name of the window for the simulator
             * 
             */
            std::string windowName = "trainsim";

            #ifdef HAS_OPENCV_SUPPORT
            /**
             * @brief the backend of the video singleton if opencv is used as backend.
             * 
             */
            std::unique_ptr<backend::videoOpenCV> backendCV;

            #define GETCV() getInstance().backendCV

            #endif

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
            static std::filesystem::path getFilePath(){
                return getInstance().getFilePath_impl();
            }

            /**
             * @brief jumps to a given frame in the video
             * 
             * @param frame_num the number of the frame that should be displayed next
             */
            static void gotoFrame(double frame_num){
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    GETCV()->setVideoProperty(cv::CAP_PROP_POS_FRAMES, frame_num);
                }
                #endif
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
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    return GETCV()->getVideoProperty(cv::CAP_PROP_FRAME_WIDTH);
                }
                #endif

                return 0.0;
            }

            /**
             * @brief Get the Height of the video in pixels
             * 
             * @return double 
             */
            static double getHight(){
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    return GETCV()->getVideoProperty(cv::CAP_PROP_FRAME_HEIGHT);
                }
                #endif

                return 0.0;
            }

            /**
             * @brief Get backend that will be used by default
             * 
             * @return VideoBackends the backend to be used by default
             */
            static VideoBackends getDefaultBackend(){
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
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    GETCV()->createWindow(windowName);
                }
                #endif
            }

            /**
             * @brief just refresh the window contents without changing the displayed content
             * 
             */
            static void refreshWindow(){
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    GETCV()->refreshWindow();
                }
                #endif
            }

            /**
             * @brief update the contents of the window and then refreshes the the window
             * 
             */
            static void updateWindow(){
                #ifdef HAS_OPENCV_SUPPORT
                if(getInstance().currentBackend == opencv){
                    if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                    GETCV()->updateWindow();    
                }
                #endif
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
                if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                GETCV()->setBackend(newBackend);
            }

            /**
             * @brief Get the Backend of the video capture
             * 
             * @return cv::VideoCaptureAPIs the video capture backend
             */
            static cv::VideoCaptureAPIs getCVBackend(){
                if(GETCV() == nullptr){GETCV() = std::make_unique<libtrainsim::backend::videoOpenCV>(); };
                return GETCV()->getBackend();
            }

            #endif

    };
}

