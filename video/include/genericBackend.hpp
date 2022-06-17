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
        * It can be used without the need to inherit by passing references to a renderer and window manager.
        * To see how to use this class with inheritance look at the videoFF_SDL class.
        *
        */
        class videoGeneric {
        protected:
            ///The underlying renderer
            std::shared_ptr<genericRenderer> renderer;
            
            ///THe underlying window manager
            std::shared_ptr<genericWindowManager> window;
            
            videoGeneric();

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
            videoGeneric(std::shared_ptr<libtrainsim::Video::genericWindowManager> _window, std::shared_ptr<libtrainsim::Video::genericRenderer> _renderer);

            /**
             * @brief Get a reference to the underlying renderer
             * 
             * @return genericRenderer&  a reference to the renderer used in this backend
             */
            std::shared_ptr<libtrainsim::Video::genericRenderer> getRenderer();

            /**
             * @brief Get a reference to the underlying window manager
             * 
             * @return genericWindowManager& a reference to the window manager used in this backend
             */
            std::shared_ptr<libtrainsim::Video::genericWindowManager> getWindowManager();

            /**
            * @brief Load a video file into the video management.
            *
            * @param uri The uri of the file.
            * @return true file sucessfully loaded
            * @return false error while loading file
            */
            virtual bool load(const std::filesystem::path& uri);

            /**
            * @brief Create a Window with a given name
            *
            * @param windowName the name of the window
            */
            virtual void createWindow(const std::string& windowName);

            /**
            * @brief refresh the window but do not display something new
            *
            */
            virtual void refreshWindow();

            /**
            * @brief jump to the given frame number and refresh the window
            * @param frameNum the frame that will be displayed
            */
            virtual void gotoFrame(uint64_t frameNum);

            /**
            * @brief get the total number of frames for the loaded video file
            * @return the total number of frames
            *
            */
            virtual uint64_t getFrameCount();

            /**
            * @brief Get the Height of the video in pixels
            *
            * @return double
            */
            virtual double getHight();

            /**
            * @brief Get the Width of the video in pixels
            *
            * @return double
            */
            virtual double getWidth();

            /**
            * @brief get the currently loaded File.
            *
            */
            const std::filesystem::path& getLoadedFile() const;

        };
    }

}
