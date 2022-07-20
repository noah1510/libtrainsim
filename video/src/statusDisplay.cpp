#include "statusDisplay.hpp"

using namespace sakurajin::unit_system::base::literals;
using namespace sakurajin::unit_system::common::literals;

libtrainsim::Video::statusDisplay::statusDisplay(){
    libtrainsim::Video::imguiHandler::init();
    for(auto& x : frametimes){
        x = 0;
    }
    
    currentAcceleration = 0_mps2;
    currentVelocity = 0_mps;
    
    currentPosition = 0_m;
    endPosition = 0_m;
    
    currentSpeedLevel = 0;
}

ImGuiIO & libtrainsim::Video::statusDisplay::io() {
    return ImGui::GetIO();
}


libtrainsim::Video::statusDisplay::~statusDisplay() {
    ImGui::DestroyContext();
}


void libtrainsim::Video::statusDisplay::update() {
    ImGui::Begin("Status Window", &my_tool_active, ImGuiWindowFlags_MenuBar);

    // Plot the frametimes
    ImGui::PlotLines("Frame Times", frametimes.data(), frametimes.size());
    
    ImGui::BeginChild("Status Text");
        ImGui::TextColored(textColor, "current Position: %Lfm / %LFm", currentPosition.value, endPosition.value);
        ImGui::TextColored(textColor, "current Velocity: %Lf km/h", currentVelocity.value);
        ImGui::TextColored(textColor, "current Acceleration: %Lf m/s²", currentAcceleration.value);
        ImGui::TextColored(textColor, "current SpeedLevel: %Lf", currentSpeedLevel.get());
        ImGui::TextColored(textColor, "current Frametime: %f", frametimes[99]);
    ImGui::EndChild();

    ImGui::End();
    
}

void libtrainsim::Video::statusDisplay::appendFrametime ( sakurajin::unit_system::base::time_si frametime ) {
    for(size_t i = 0; i < frametimes.size();i++){
        frametimes[i] = frametimes[i+1];
    }
    frametime = sakurajin::unit_system::unit_cast(frametime, sakurajin::unit_system::prefix::milli);
    frametimes[frametimes.size()-1] = frametime.value;
}

void libtrainsim::Video::statusDisplay::changePosition ( sakurajin::unit_system::base::length newPosition ) {
    currentPosition = sakurajin::unit_system::unit_cast(newPosition, 1);
}

void libtrainsim::Video::statusDisplay::changeEndPosition ( sakurajin::unit_system::base::length newEndPosition ) {
    endPosition = sakurajin::unit_system::unit_cast(newEndPosition, 1);;
}

void libtrainsim::Video::statusDisplay::setAcceleration ( sakurajin::unit_system::common::acceleration newAcceleration ) {
    currentAcceleration = sakurajin::unit_system::unit_cast(newAcceleration, 1);
}

void libtrainsim::Video::statusDisplay::setVelocity ( sakurajin::unit_system::common::speed newVelocity ) {
    currentVelocity = sakurajin::unit_system::unit_cast(newVelocity, (1_kmph).multiplier );
}

void libtrainsim::Video::statusDisplay::setSpeedLevel ( core::input_axis newSpeedLevel ) {
    currentSpeedLevel = newSpeedLevel;
}

