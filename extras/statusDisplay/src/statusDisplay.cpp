#include "statusDisplay.hpp"

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

libtrainsim::extras::statusDisplay::statusDisplay(){
    libtrainsim::Video::imguiHandler::init();
    for(auto& x : frametimes){
        x = 0;
    }
    for(auto& x : rendertimes){
        x = 0;
    }
    
    currentAcceleration = 0_mps2;
    currentVelocity = 0_mps;
    
    currentPosition = 0_m;
    endPosition = 0_m;
    
    currentSpeedLevel = 0;
}

ImGuiIO & libtrainsim::extras::statusDisplay::io() {
    return ImGui::GetIO();
}


libtrainsim::extras::statusDisplay::~statusDisplay() {
}


void libtrainsim::extras::statusDisplay::update() {
    
    //set size and pos on program start to initial values
    static bool firstStart = true;
    if(firstStart){
        auto size = ImGui::GetIO().DisplaySize;
        
        ImVec2 initialSize {size.x,200};
        //ImGui::SetNextWindowContentSize( initialSize );
        ImVec2 size_min {1400,200};
        ImVec2 size_max {1400,600};
        ImGui::SetNextWindowSizeConstraints(size_min,size_max);
        
        ImVec2 initialPos {0,size.y-200};
        ImGui::SetNextWindowPos(initialPos);
        
        firstStart = false;
    }
    
    //actually start drawing th window
    ImGui::Begin("Status Window", &my_tool_active, ImGuiWindowFlags_MenuBar);

        // Plot the frametimes
        ImGui::PlotLines("Frame Times", frametimes.data(), frametimeValues);
        ImGui::PlotLines("Render Times", rendertimes.data(), rendertimeValues);
        
        ImGui::BeginChild("Status Text");
            ImGui::TextColored(textColor, "current Position: %Lfm / %LFm", currentPosition.value, endPosition.value);
            ImGui::TextColored(textColor, "current Velocity: %Lf km/h", currentVelocity.value);
            ImGui::TextColored(textColor, "current Acceleration: %Lf m/s²", currentAcceleration.value);
            ImGui::TextColored(textColor, "current SpeedLevel: %Lf", currentSpeedLevel.get());
            ImGui::TextColored(textColor, "current Frametime: %f ms", frametimes[frametimeValues-1]);
            ImGui::TextColored(textColor, "current Rendertime: %f ms", rendertimes[rendertimeValues-1]);
        ImGui::EndChild();

    ImGui::End();
    
}

void libtrainsim::extras::statusDisplay::appendFrametime ( sakurajin::unit_system::base::time_si frametime ) {
    frametime = sakurajin::unit_system::unit_cast(frametime, sakurajin::unit_system::prefix::milli);
    libtrainsim::core::Helper::appendValue<float,frametimeValues>(frametimes,frametime.value);
}

void libtrainsim::extras::statusDisplay::appendRendertime ( sakurajin::unit_system::base::time_si rendertime ) {
    rendertime = sakurajin::unit_system::unit_cast(rendertime, sakurajin::unit_system::prefix::milli);
    libtrainsim::core::Helper::appendValue<float,rendertimeValues>(rendertimes,rendertime.value);
}


void libtrainsim::extras::statusDisplay::changePosition ( sakurajin::unit_system::base::length newPosition ) {
    currentPosition = sakurajin::unit_system::unit_cast(newPosition, 1);
}

void libtrainsim::extras::statusDisplay::changeEndPosition ( sakurajin::unit_system::base::length newEndPosition ) {
    endPosition = sakurajin::unit_system::unit_cast(newEndPosition, 1);;
}

void libtrainsim::extras::statusDisplay::setAcceleration ( sakurajin::unit_system::common::acceleration newAcceleration ) {
    currentAcceleration = sakurajin::unit_system::unit_cast(newAcceleration, 1);
}

void libtrainsim::extras::statusDisplay::setVelocity ( sakurajin::unit_system::common::speed newVelocity ) {
    currentVelocity = sakurajin::unit_system::unit_cast(newVelocity, (1_kmph).multiplier );
}

void libtrainsim::extras::statusDisplay::setSpeedLevel ( core::input_axis newSpeedLevel ) {
    currentSpeedLevel = newSpeedLevel;
}

