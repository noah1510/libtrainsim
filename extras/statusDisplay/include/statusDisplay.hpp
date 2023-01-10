#pragma once

#include "imguiHandler.hpp"
#include "unit_system.hpp"
#include "input_axis.hpp"
#include "helper.hpp"
#include "statusDisplayGraph.hpp"
#include "window.hpp"

namespace libtrainsim{
    namespace extras{
        class statusDisplay;
      
        //a class to change the settings of the status display
        class LIBTRAINSIM_EXPORT_MACRO statusDisplaySettings : public libtrainsim::Video::tabPage{
          private:
            void content() override;
            statusDisplay& display;
          public:
            statusDisplaySettings(statusDisplay& disp);
        };
        
        /**
         * @brief a window to display real time stats for the simulator
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO statusDisplay : public libtrainsim::Video::window{
          friend class statusDisplaySettings;
          private:
            
            //control if the latest value should be displayed
            bool displayLatestValue = true;
            
            //control if the graphs should be displayed
            bool displayGraphs = false;
            
            //control if the progress along the track should be shown
            bool displayProgress = true;
            
            //checks if a settings tab should be created and desetroyed by this class
            bool manageSettings;
            
            /**
             * @brief a vector with all of graphs that are displayed and if they should be displayed
             */
            std::vector<std::pair<statusDisplayGraph<100>, bool>> graphs;
            
            /**
             * @brief the names of all of the graphs that have to exist and cannot be deleted
             */
            std::vector<std::string> defaultGraphNames;
            
            //The position where the track begins
            sakurajin::unit_system::length beginPosition;
            
            //the current position along the track
            sakurajin::unit_system::length currentPosition;
            
            //the position where the track ends
            sakurajin::unit_system::length endPosition;
            
            /**
             * @brief draws the content of the window
             * 
             * This updates the contents of the statusDisplay window.
             * To actually display the window call the draw() function which
             * is inherited from the window class.
             */
            void content() override;
            
          public:
            /**
             * @brief create a new status display
             * 
             * @param _manageSettings if false is passed this class will not create a settings tab
             */
            statusDisplay(bool _manageSettings = true);
            
            /**
             * @brief destroy the status display
             * 
             * @note if a settings tab was added it will be removed in here
             */
            ~statusDisplay();
            
            /**
             * @brief add a new frametime which will be displayed as the latest value.
             */
            void appendFrametime(sakurajin::unit_system::time_si frametime);
            
            /**
             * @brief add a new rendertime which will be displayed as the latest value.
             */
            void appendRendertime(sakurajin::unit_system::time_si rendertime);
            
            /**
             * @brief update the current position of the train, to move the progress bar
             */
            void changePosition(sakurajin::unit_system::length newPosition);
            
            /**
             * @brief set the begin position of the track for the progress bar
             */
            void changeEndPosition(sakurajin::unit_system::length newEndPosition);
            
            /**
             * @brief set the end position of the track for the progress bar
             */
            void changeBeginPosition(sakurajin::unit_system::length newBeginPosition);
            
            /**
             * @brief add a new speedLevel which will be displayed as the latest value.
             */
            void setSpeedLevel(libtrainsim::core::input_axis newSpeedLevel);
            
            /**
             * @brief add a new acceleration which will be displayed as the latest value.
             */
            void setAcceleration(sakurajin::unit_system::acceleration newAcceleration);
            
            /**
             * @brief add a new velocity which will be displayed as the latest value.
             */
            void setVelocity(sakurajin::unit_system::speed newVelocity);
            
            /**
             * @brief create a new graph to keep track of any value on a graph
             * 
             * @warning this throws an exception if that graph already exists
             */
            void createCustomGraph(std::string graphName, std::string tooltipMessage);
            
            /**
             * @brief remove a custom graph
             * 
             * @warning this throws an exception if there is no graph with that name or
             * you try to remove one of the default graphs
             */
            void removeGraph(std::string graphName);
            
            /**
             * add a new value which will be displayed on graphName as the latest value.
             * 
             * @warning this throws an exception if the graph does not exist
             */
            void appendToGraph(std::string graphName, float value);
        };
    }
}
