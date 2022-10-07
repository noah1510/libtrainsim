#include "statusDisplay.hpp"

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;


libtrainsim::extras::statusDisplaySettings::statusDisplaySettings(statusDisplay& disp):tabPage{"statusDisplay"}, display{disp}{}

void libtrainsim::extras::statusDisplaySettings::displayContent() {
    ImGui::Checkbox("Display Latest Values", &display.displayLatestValue);
}


libtrainsim::extras::statusDisplay::statusDisplay(){
    libtrainsim::Video::imguiHandler::init();
    
    defaultGraphNames = {
        "frametimes",
        "rendertimes",
        "acceleration",
        "velocity",
        "speedLevel"
    };
    
    beginPosition = 0_m;
    currentPosition = 0_m;
    endPosition = 0_m;
    
    graphs.emplace_back(statusDisplayGraph<100>{"frametimes", "frametimes in ms"});
    graphs.emplace_back(statusDisplayGraph<100>{"rendertimes", "rendertimes in ms"});
    graphs.emplace_back(statusDisplayGraph<100>{"acceleration", "Acceleration in m/s²"});
    graphs.emplace_back(statusDisplayGraph<100>{"velocity", "Velocity in km/h"});
    graphs.emplace_back(statusDisplayGraph<100>{"speedLevel", "SpeedLevel"});
    
    libtrainsim::Video::imguiHandler::addSettingsTab(std::make_shared<statusDisplaySettings>(*this));
}


libtrainsim::extras::statusDisplay::~statusDisplay() {
    libtrainsim::Video::imguiHandler::removeSettingsTab("statusDisplay");
}


void libtrainsim::extras::statusDisplay::update() {
    
    static bool firstStart = true;
    if(firstStart){
        auto size = ImGui::GetIO().DisplaySize;
            
        ImVec2 initialSize {size.x,200};
        ImGui::SetNextWindowSize( initialSize );
        
        ImVec2 initialPos {0,size.y-200};
        ImGui::SetNextWindowPos(initialPos);
        firstStart = false;
    }
    
    //actually start drawing th window
    ImGui::Begin("Status Window");
        
        //display the prograss bar for the position
        ImGui::ProgressBar((currentPosition-beginPosition)/(endPosition-beginPosition));
        if(ImGui::IsItemHovered()){
            ImGui::SetTooltip("The position along the Track. Begin: %Lfm, End: %LFm, Current: %LFm", beginPosition.value, endPosition.value, currentPosition.value);
        }

        // Plot the all of the graphs
        for(auto& graph:graphs){
            graph.display(displayLatestValue);
        }

    ImGui::End();
    
}

void libtrainsim::extras::statusDisplay::appendFrametime ( sakurajin::unit_system::base::time_si frametime ) {
    frametime = sakurajin::unit_system::unit_cast(frametime, sakurajin::unit_system::prefix::milli);
    appendToGraph("frametimes", frametime.value);
}

void libtrainsim::extras::statusDisplay::appendRendertime ( sakurajin::unit_system::base::time_si rendertime ) {
    rendertime = sakurajin::unit_system::unit_cast(rendertime, sakurajin::unit_system::prefix::milli);
    appendToGraph("rendertimes", rendertime.value);
}


void libtrainsim::extras::statusDisplay::changeBeginPosition ( sakurajin::unit_system::base::length newBeginPosition ) {
    beginPosition = sakurajin::unit_system::unit_cast(newBeginPosition, 1);
}

void libtrainsim::extras::statusDisplay::changePosition ( sakurajin::unit_system::base::length newPosition ) {
    currentPosition = sakurajin::unit_system::unit_cast(newPosition, 1);
}

void libtrainsim::extras::statusDisplay::changeEndPosition ( sakurajin::unit_system::base::length newEndPosition ) {
    endPosition = sakurajin::unit_system::unit_cast(newEndPosition, 1);
}

void libtrainsim::extras::statusDisplay::setAcceleration ( sakurajin::unit_system::common::acceleration newAcceleration ) {
    auto acc = sakurajin::unit_system::unit_cast(newAcceleration, 1);
    appendToGraph("acceleration", acc.value);
}

void libtrainsim::extras::statusDisplay::setVelocity ( sakurajin::unit_system::common::speed newVelocity ) {
    auto vel = sakurajin::unit_system::unit_cast(newVelocity, 1);
    appendToGraph("velocity", vel.value);
}

void libtrainsim::extras::statusDisplay::setSpeedLevel ( core::input_axis newSpeedLevel ) {
    appendToGraph("speedLevel", newSpeedLevel.get());
}

void libtrainsim::extras::statusDisplay::createCustomGraph ( std::string graphName, std::string tooltipMessage ) {
    for(auto& graph: graphs){
        if(graph.getName() == graphName){
            throw std::invalid_argument("A graph with the given name already exists!");
        }
    }
    
    graphs.emplace_back(statusDisplayGraph<100>{graphName,tooltipMessage});
}

void libtrainsim::extras::statusDisplay::removeGraph ( std::string graphName ) {
    if(libtrainsim::core::Helper::contains(defaultGraphNames,graphName)){
        throw std::invalid_argument("render and frame times may not be removed!");
    }
    
    for(auto i = graphs.begin(); i < graphs.end(); i++){
        if((*i).getName() == graphName){
            graphs.erase(i);
            return;
        }
    }
    
    throw std::invalid_argument("no graph with this name exists");
}

void libtrainsim::extras::statusDisplay::appendToGraph ( std::string graphName, float value ) {
    for(auto& graph: graphs){
        if(graph.getName() == graphName){
            graph.appendValue(value);
            return;
        }
    }
    
    throw std::invalid_argument("no graph with this name exists");
}

