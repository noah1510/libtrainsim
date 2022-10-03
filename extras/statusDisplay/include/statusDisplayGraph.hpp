#pragma once

#include "imguiHandler.hpp"
#include "helper.hpp"

namespace libtrainsim{
    namespace extras{
        template<size_t VALUE_COUNT>
        class statusDisplayGraph{
        private:
            std::array<float, VALUE_COUNT> values;
            
            std::string name;
            std::string tooltip;
        public:
            statusDisplayGraph(std::string graphName, std::string tooltipMessage);
            
            void display(bool showLatest = true);
            
            void appendValue(float newValue);
            const std::string& getName();
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

