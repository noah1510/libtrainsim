#include <string>
#include <filesystem>

#include "opencv4/opencv2/opencv.hpp"

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
            void hello_impl() const;

            /**
             * @brief The implementation of load look @load for details
             */
            bool load_impl(const std::filesystem::path& uri);

            /**
             * @brief This is that path to the currently loaded file.
             * 
             */
            std::filesystem::path loadedFile;   

        public:
            /**
             * @brief This function is used to test if libtrainsim::video was correctly linked and just prints hello world
             * 
             */
            static void hello(){
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

    };
}

