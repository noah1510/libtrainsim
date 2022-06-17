#pragma once

#include "genericRenderer.hpp"

namespace libtrainsim {
    namespace Video{
        /**
         * @brief This is a generic window manager.
         * The window manager is used to create a window, provide control support and display the frames from the renderer.
         * Most of the virtual functions need to be implemented for each child, since they are specific to the used library.
         * 
         */
        class genericWindowManager{
        protected:
            std::shared_ptr<genericRenderer> renderer;
            
            /**
            * @brief the frame that was displayed last.
            *
            */
            std::shared_ptr<libtrainsim::Frame> lastFrame;
        
            /**
            * @brief the name of the current window
            *
            */
            std::string currentWindowName = "";
        
            /**
            * @brief display a new frame on the window
            *
            * @param newFrame the new frame to be displayed
            */
            virtual void displayFrame(std::shared_ptr<libtrainsim::Frame> newFrame);
            
            bool windowFullyCreated = false;
                
        public:
            genericWindowManager(std::shared_ptr<genericRenderer> _renderer);
            
            virtual ~genericWindowManager();
            
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
            void gotoFrame(uint64_t frameNum);
            
        };
    }
}
