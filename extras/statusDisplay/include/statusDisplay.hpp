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
            ImVec4 textColor{0.7,1,0.7,1};
            bool my_tool_active = false;
            
            std::vector<statusDisplayGraph<100>> graphs; 
            
            ImGuiIO& io();
            
            sakurajin::unit_system::base::length currentPosition;
            sakurajin::unit_system::base::length endPosition;
            
            sakurajin::unit_system::common::acceleration currentAcceleration;
            sakurajin::unit_system::common::speed currentVelocity;
            
            libtrainsim::core::input_axis currentSpeedLevel;
            
          public:
            statusDisplay();
            
            ~statusDisplay();
            
            void update();
            
            void appendFrametime(sakurajin::unit_system::base::time_si frametime);
            
            void appendRendertime(sakurajin::unit_system::base::time_si rendertime);
            
            void changePosition(sakurajin::unit_system::base::length newPosition);
            
            void changeEndPosition(sakurajin::unit_system::base::length newEndPosition);
            
            void setSpeedLevel(libtrainsim::core::input_axis newSpeedLevel);
            
            void setAcceleration(sakurajin::unit_system::common::acceleration newAcceleration);
            
            void setVelocity(sakurajin::unit_system::common::speed newVelocity);
            
            void createCustomGraph(std::string graphName, std::string tooltipMessage);
            void removeGraph(std::string graphName);
            void appendToGraph(std::string graphName, float value);
        };
    }
}
