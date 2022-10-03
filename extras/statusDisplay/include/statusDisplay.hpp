#pragma once

#include "imguiHandler.hpp"

#include "time.hpp"
#include "length.hpp"
#include "speed.hpp"
#include "acceleration.hpp"
#include "prefix.hpp"

#include <array>

#include "input_axis.hpp"
#include "helper.hpp"
#include "statusDisplayGraph.hpp"

namespace libtrainsim{
    namespace extras{
        class statusDisplay{
          private:
            
            /**
             * @brief a vector with all of graphs that are displayed
             */
            std::vector<statusDisplayGraph<100>> graphs;
            
            /**
             * @brief the names of all of the graphs that have to exist and cannot be deleted
             */
            std::vector<std::string> defaultGraphNames;
            
            //The position where the track begins
            sakurajin::unit_system::base::length beginPosition;
            
            //the current position along the track
            sakurajin::unit_system::base::length currentPosition;
            
            //the position where the track ends
            sakurajin::unit_system::base::length endPosition;
            
          public:
            statusDisplay();
            
            ~statusDisplay();
            
            void update();
            
            void appendFrametime(sakurajin::unit_system::base::time_si frametime);
            
            void appendRendertime(sakurajin::unit_system::base::time_si rendertime);
            
            void changePosition(sakurajin::unit_system::base::length newPosition);
            
            void changeEndPosition(sakurajin::unit_system::base::length newEndPosition);
            void changeBeginPosition(sakurajin::unit_system::base::length newBeginPosition);
            
            void setSpeedLevel(libtrainsim::core::input_axis newSpeedLevel);
            
            void setAcceleration(sakurajin::unit_system::common::acceleration newAcceleration);
            
            void setVelocity(sakurajin::unit_system::common::speed newVelocity);
            
            void createCustomGraph(std::string graphName, std::string tooltipMessage);
            void removeGraph(std::string graphName);
            void appendToGraph(std::string graphName, float value);
        };
    }
}
