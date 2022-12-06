#include "statusDisplay.hpp"

using namespace sakurajin::unit_system;
using namespace sakurajin::unit_system::literals;
using namespace std::literals;

libtrainsim::extras::statusDisplaySettings::statusDisplaySettings(statusDisplay& disp):tabPage{"statusDisplay"}, display{disp}{}

void libtrainsim::extras::statusDisplaySettings::displayContent() {
    

    ImGui::Checkbox("Display Latest Values", &display.displayLatestValue);
    
    ImGui::Text("Graph visibility:");
    for(auto& graph:display.graphs){
        std::stringstream ss;
        ss << "Show Graph: " << graph.first.getName();
        ImGui::Checkbox(ss.str().c_str(), &graph.second);
        if(ImGui::IsItemHovered()){
            ImGui::SetTooltip("Change if a graph should be visible on the statusDisplay");
        }
    }
}


libtrainsim::extras::statusDisplay::statusDisplay(bool _manageSettings){
    libtrainsim::Video::imguiHandler::init();
    manageSettings = _manageSettings;
    
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
    
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{"frametimes", "frametimes in ms"},true});
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{"rendertimes", "rendertimes in ms"},true});
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{"acceleration", "Acceleration in m/s²"},true});
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{"velocity", "Velocity in km/h"},true});
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{"speedLevel", "SpeedLevel"},true});
    
    if(manageSettings){
        libtrainsim::Video::imguiHandler::addSettingsTab(std::make_shared<statusDisplaySettings>(*this));
    }
}


libtrainsim::extras::statusDisplay::~statusDisplay() {
    if(manageSettings){
        libtrainsim::Video::imguiHandler::removeSettingsTab("statusDisplay");
    }
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
            if(graph.second){
                graph.first.display(displayLatestValue);
            }
        }

    ImGui::End();
    
}

void libtrainsim::extras::statusDisplay::appendFrametime ( sakurajin::unit_system::time_si frametime ) {
    frametime = sakurajin::unit_system::unit_cast(frametime, multiplier(std::milli::type{}));
    appendToGraph("frametimes", frametime.value);
}

void libtrainsim::extras::statusDisplay::appendRendertime ( sakurajin::unit_system::time_si rendertime ) {
    rendertime = sakurajin::unit_system::unit_cast(rendertime, multiplier(std::milli::type{}));
    appendToGraph("rendertimes", rendertime.value);
}


void libtrainsim::extras::statusDisplay::changeBeginPosition ( sakurajin::unit_system::length newBeginPosition ) {
    beginPosition = sakurajin::unit_system::unit_cast(newBeginPosition, 1);
}

void libtrainsim::extras::statusDisplay::changePosition ( sakurajin::unit_system::length newPosition ) {
    currentPosition = sakurajin::unit_system::unit_cast(newPosition, 1);
}

void libtrainsim::extras::statusDisplay::changeEndPosition ( sakurajin::unit_system::length newEndPosition ) {
    endPosition = sakurajin::unit_system::unit_cast(newEndPosition, 1);
}

void libtrainsim::extras::statusDisplay::setAcceleration ( sakurajin::unit_system::acceleration newAcceleration ) {
    auto acc = sakurajin::unit_system::unit_cast(newAcceleration, 1);
    appendToGraph("acceleration", acc.value);
}

void libtrainsim::extras::statusDisplay::setVelocity ( sakurajin::unit_system::speed newVelocity ) {
    auto vel = sakurajin::unit_system::unit_cast(newVelocity, 1);
    appendToGraph("velocity", vel.value);
}

void libtrainsim::extras::statusDisplay::setSpeedLevel ( core::input_axis newSpeedLevel ) {
    appendToGraph("speedLevel", newSpeedLevel.get());
}

void libtrainsim::extras::statusDisplay::createCustomGraph ( std::string graphName, std::string tooltipMessage ) {
    for(auto& graph: graphs){
        if(graph.first.getName() == graphName){
            throw std::invalid_argument("A graph with the given name already exists!");
        }
    }
    
    graphs.emplace_back(std::pair{statusDisplayGraph<100>{graphName,tooltipMessage}, true});
}

void libtrainsim::extras::statusDisplay::removeGraph ( std::string graphName ) {
    if(libtrainsim::core::Helper::contains(defaultGraphNames,graphName)){
        throw std::invalid_argument("render and frame times may not be removed!");
    }
    
    for(auto i = graphs.begin(); i < graphs.end(); i++){
        if((*i).first.getName() == graphName){
            graphs.erase(i);
            return;
        }
    }
    
    throw std::invalid_argument("no graph with this name exists");
}

void libtrainsim::extras::statusDisplay::appendToGraph ( std::string graphName, float value ) {
    for(auto& graph: graphs){
        if(graph.first.getName() == graphName){
            graph.first.appendValue(value);
            return;
        }
    }
    
    throw std::invalid_argument("no graph with this name exists");
}

