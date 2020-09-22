#include <memory>
#include <string>
#include <filesystem>

#if __has_include("opencv2/opencv.hpp")
    #include "opencv2/opencv.hpp"
#elif __has_include("opencv4/opencv2/opencv.hpp")
    #include "opencv4/opencv2/opencv.hpp"
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
            video(void);

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

            /**
             * @brief This variable saves the state of the video capture device aka the video that is rendered.
             * 
             */
            std::unique_ptr<cv::VideoCapture> videoCap;

            ///The implementation for the getFPS method
            double getFPS_impl();

            ///The implementation for the getNextFrame method
            const cv::Mat getNextFrame_impl(); 

            ///The implementation for the getPosMsec method
            double getPosMsec_impl();

            ///The implementation for the getPosFrames method
            double getPosFrames_impl();

            ///The implementation for the getWidth method
            double getWidth_impl();

            ///The implementation for the getHight method
            double getHight_impl();

            ///The implementation for the getFrameCount method
            double getFrameCount_impl();

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
             * @brief get the framerate of the video file
             * 
             * @return double 
             */
            static double getFPS(){
                return getInstance().getFPS_impl();
            }

            /**
             * @brief Get the current Position in the file in milliseconds
             * 
             * @return double 
             */
            static double getPosMsec(){
                return getInstance().getPosMsec_impl();
            }

            /**
             * @brief Get the current Position in the file in frames
             * 
             * @return double 
             */
            static double getPosFrames(){
                return getInstance().getPosFrames_impl();
            }

            /**
             * @brief Get the Width of the video in pixels
             * 
             * @return double 
             */
            static double getWidth(){
                return getInstance().getWidth_impl();
            }

            /**
             * @brief Get the Height of the video in pixels
             * 
             * @return double 
             */
            static double getHight(){
                return getInstance().getHight_impl();
            }

            /**
             * @brief Get the total number of frame the video has.
             * 
             * @return double 
             */
            static double getFrameCount(){
                return getInstance().getFrameCount_impl();
            }

            /**
             * @brief Retrieve the next frame to display it. 
             * If no video is loaded or there is no new frame, an empty frame will be returned.
             * You should check the returned frame with the method .empty(), which will return true if the frame is empty.
             * 
             * @return const cv::Mat The next frame of the video
             */
            static const cv::Mat getNextFrame(){
                return getInstance().getNextFrame_impl();
            }


    };
}

