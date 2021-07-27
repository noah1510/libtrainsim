#pragma once

#include "frame.hpp"
#include <filesystem>
#include <string>
#include "genericRenderer.hpp"
#include "genericWindowManager.hpp"

namespace libtrainsim{
    /**
    * @brief This namespace contains all the video_backend components of libtrainsim.
    * @note All classes in this namespace are only intended for the use by libtrainsim.
    * 
    */
    namespace Video {
        /**
        * @brief This is a generic class every specific video backend can extent from.
        * It specifies the way every backend should interact with the video sigleton.
        *
        */
        class videoGeneric {
        protected:
            ///The underlying renderer
            genericRenderer& renderer;
            
            ///THe underlying window manager
            genericWindowManager& window;

        public:

            /**
            * @brief Destroy the generic video object
            *
            */
            virtual ~videoGeneric();
            
            /**
             * @brief Construct a new generic backend from a renderer and window manager
             * 
             * @param _window the window manager to be used in this backend
             * @param _renderer the renderer to be used in this backend
             */
            videoGeneric(libtrainsim::Video::genericWindowManager& _window, libtrainsim::Video::genericRenderer& _renderer);

            /**
             * @brief Get a reference to the underlying renderer
             * 
             * @return genericRenderer&  a reference to the renderer used in this backend
             */
            genericRenderer& getRenderer();

            /**
             * @brief Get a reference to the underlying window manager
             * 
             * @return genericWindowManager& a reference to the window manager used in this backend
             */
            genericWindowManager& getWindowManager();

            /**
            * @brief Load a video file into the video management.
            *
            * @param uri The uri of the file.
            * @return true file sucessfully loaded
            * @return false error while loading file
            */
            bool load(const std::filesystem::path& uri);

            /**
            * @brief Create a Window with a given name
            *
            * @param windowName the name of the window
            */
            void createWindow(const std::string& windowName);

            /**
            * @brief refresh the window but do not display something new
            *
            */
            void refreshWindow();

            /**
            * @brief jump to the given frame number and refresh the window
            * @param frameNum the frame that will be displayed
            */
            void gotoFrame(uint64_t frameNum);

            /**
            * @brief get the total number of frames for the loaded video file
            * @return the total number of frames
            *
            */
            uint64_t getFrameCount();

            /**
            * @brief Get the Height of the video in pixels
            *
            * @return double
            */
            double getHight();

            /**
            * @brief Get the Width of the video in pixels
            *
            * @return double
            */
            double getWidth();

            /**
            * @brief get the currently loaded File.
            *
            */
            const std::filesystem::path& getLoadedFile() const;

        };
    }

}
