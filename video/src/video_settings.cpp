#include "imguiHandler.hpp"

void libtrainsim::Video::styleSettings::displayContent() {
    imguiHandler& handle = imguiHandler::getInstance();
    float clearCol[3] = {handle.clear_color.x,handle.clear_color.y,handle.clear_color.z};
    
    ImGui::ShowStyleEditor(&ImGui::GetStyle());
        
    //add an 'empty' line between the options
    ImGui::Text(" ");
    
    //display the color changer for the clear color
    ImGui::TableNextColumn();
    ImGui::ColorPicker3("clear color picker", clearCol);
    if(ImGui::IsItemHovered()){
        ImGui::SetTooltip("change the background color of the background window");
    }

    handle.clear_color = ImVec4{clearCol[0],clearCol[1],clearCol[2],1.0};
}

libtrainsim::Video::styleSettings::styleSettings() : tabPage("style"){}

void libtrainsim::Video::basicSettings::displayContent() {
    imguiHandler& handle = imguiHandler::getInstance();
    
    ImGui::BeginTable("basic Settings table", 1, (ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp));
        //display the style changer with its tooltip
        ImGui::TableNextColumn();
        ImGui::ShowStyleSelector("active imgui style");
        if(ImGui::IsItemHovered()){
            ImGui::SetTooltip("change the style of the windows");
        }
        
        //add an 'empty' line between the options
        ImGui::TableNextColumn();
        ImGui::Text(" ");
        
        //display the default FBO size selection
        ImGui::TableNextColumn();
        int sizeY = handle.defaultFBOSize.y();
        
        static size_t fboSizeIndex = 1;
        if(ImGui::BeginCombo("Select default FBO size", FBOsizeOptions.at(fboSizeIndex).first.c_str() )){
            for(size_t i = 0; i < FBOsizeOptions.size();i++){
                if(ImGui::Selectable(FBOsizeOptions.at(i).first.c_str(), fboSizeIndex == i)){
                    fboSizeIndex = i;
                }
            }
            
            ImGui::EndCombo();
        }
        
        sizeY = FBOsizeOptions.at(fboSizeIndex).second;
        handle.defaultFBOSize.x() = sizeY*16/9;
        handle.defaultFBOSize.y() = sizeY;
        
    ImGui::EndTable();
}

libtrainsim::Video::basicSettings::basicSettings() : tabPage("basic"), FBOsizeOptions{{
    {"720p", 720},
    {"1080p", 1080},
    {"1440p", 1440},
    {"2160p", 2160}
}}{}

libtrainsim::Video::settingsWindow::settingsWindow() : window("Settings Window", false) {
    
    flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;
    showWindow = false;
    
    addSettingsTab(std::make_shared<basicSettings>());
    addSettingsTab(std::make_shared<styleSettings>());
    
}

void libtrainsim::Video::settingsWindow::drawContent() {
    if(ImGui::BeginTabBar("settings tabs")){
        for(auto& tab:settingsTabs){(*tab)();}
        ImGui::EndTabBar();
    }
}


void libtrainsim::Video::settingsWindow::addSettingsTab ( std::shared_ptr<tabPage> newTab ) {
    for(auto& tab:settingsTabs){
        if(tab->getName() == newTab->getName()){
            throw std::invalid_argument{"a tab with this name already exists in the settings"};
        }
    }
    settingsTabs.emplace_back(newTab);

}

void libtrainsim::Video::settingsWindow::removeSettingsTab ( std::string_view tabName ) {
    auto i = settingsTabs.begin();
    i+=2;
    while(i != settingsTabs.end()){
        if((*i)->getName() == tabName){
            settingsTabs.erase(i);
            return;
        }
        i++;
    }
}
