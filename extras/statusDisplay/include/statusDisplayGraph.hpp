#pragma once

#include "imguiHandler.hpp"
#include "helper.hpp"

namespace libtrainsim{
    namespace extras{
        /**
         * @brief an abstraction over the ImGui graphs for use with the statusDisplay
         * This class provides an easy abstraction over the PlotLines function to
         * handle all of the common graph display needs
         * 
         * @tparam VALUE_COUNT The number of samples this graph should have
         */
        template<size_t VALUE_COUNT>
        class statusDisplayGraph{
        private:
            /**
             * @brief the internal array to store the data values that are displayed
             */
            std::array<float, VALUE_COUNT> values;
            
            /**
             * @brief the name of the graph 
             */
            std::string name;
            
            /**
             * @brief the tooltip that should be shown when the graph is hovered
             */
            std::string tooltip;
        public:
            /**
             * @brief create a new graph with a name and tooltip
             */
            statusDisplayGraph(std::string graphName, std::string tooltipMessage);
            
            /**
             * @brief display the graph in a window
             * 
             * @param showLatest if true next to the name of the graph there will be the latest value
             */
            void display(bool showLatest = true);
            
            /**
             * @brief append a value to the graph to be the latest value to be displayed
             */
            void appendValue(float newValue);
            
            /**
             * @brief get the name of the graph
             */
            const std::string& getName();
            
            /**
             * @brief get the latest value of the graph
             */
            float getLatest();
        };
    }
}

template<size_t VALUE_COUNT>
libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::statusDisplayGraph ( std::string graphName, std::string tooltipMessage ) {
    name = graphName;
    tooltip = tooltipMessage;
    
    for(auto& val:values){
        val = 0.0f;
    }
}

template<size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::appendValue ( float newValue ) {
    libtrainsim::core::Helper::appendValue<float,VALUE_COUNT>(values, newValue);
}

template<size_t VALUE_COUNT>
void libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::display(bool showLatest) {
    ImGui::BeginGroup();
    
        std::stringstream ss;
        ss << name;
        if(showLatest){
            ss << ": " << getLatest();
        }
        
        ImGui::PlotLines(ss.str().c_str(), values.data(), static_cast<int>(VALUE_COUNT) );
        
        if(ImGui::IsItemHovered()){
            ImGui::BeginTooltip();
            ImGui::Text("%s", tooltip.c_str());
            ImGui::EndTooltip();
        }
        
    ImGui::EndGroup();
}

template<size_t VALUE_COUNT>
const std::string & libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::getName() {
    return name;
}

template<size_t VALUE_COUNT>
float libtrainsim::extras::statusDisplayGraph<VALUE_COUNT>::getLatest() {
    return values[VALUE_COUNT-1];
}

