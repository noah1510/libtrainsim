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
        
        //add an 'empty' line between the options
        ImGui::TableNextColumn();
        ImGui::Text(" ");
        
        //Add manual saving and loading of window configurations
        ImGui::TableNextColumn();
        static char saveLocation[1000] = "windowSettings.ini\0";
        ImGui::InputText("Location of the save file:", saveLocation, 1000);
        static bool startSaving = false;
        if(ImGui::Button("Save Window Settings to file")){
            startSaving = true;
        }
        
        if(startSaving){
            auto [a,d] = showPopups();
            auto [location, isOkay] = checkPath(saveLocation, a, d);
            if(isOkay){
                ImGui::SaveIniSettingsToDisk(location.string().c_str());
            }
            if(a || d || isOkay){
                startSaving = false;
            }
        }
        
    ImGui::EndTable();
    
}

std::tuple<bool, bool> libtrainsim::Video::basicSettings::showPopups() {
    if(ImGui::BeginPopup("No ini")){
        ImGui::Text("The file has no ini extention!");
        ImGui::EndPopup();
    }
    
    bool acceptOverwrite = false;
    bool denyOverwrite = false;
    if(ImGui::BeginPopup("Existing File")){
        ImGui::Text("File already exists! Do you want to overwrite it?");
        acceptOverwrite = ImGui::Button("Accept");
        denyOverwrite = ImGui::Button("Deny");
        ImGui::EndPopup();
    }
    
    if(acceptOverwrite || denyOverwrite){
        ImGui::CloseCurrentPopup();
    }
    
    return {acceptOverwrite, denyOverwrite};
}


std::tuple<std::filesystem::path, bool> libtrainsim::Video::basicSettings::checkPath ( const std::filesystem::path& location, bool acceptOverwrite, bool denyOverwrite) {
    auto cleanedLocation = location;
    if(cleanedLocation.is_relative()){
        cleanedLocation = std::filesystem::current_path() / cleanedLocation;
    }
    
    if(denyOverwrite){
        return {"",false};
    }
    
    if(cleanedLocation.extension() != ".ini"){
        ImGui::OpenPopup("No ini");
        return {"", false};
    }else if(std::filesystem::exists(cleanedLocation) && !acceptOverwrite){
        ImGui::OpenPopup("Existing File");
        return {"", false};
    }
    
    auto loc = cleanedLocation;
    loc.remove_filename();
    try{
        std::filesystem::create_directories(loc);
    }catch(const std::exception& e){
        libtrainsim::core::Helper::print_exception(e);
    }
    cleanedLocation = std::filesystem::relative(cleanedLocation);
    
    return {cleanedLocation, true};
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
