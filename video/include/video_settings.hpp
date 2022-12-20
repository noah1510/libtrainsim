#pragma once

#include "tabPage.hpp"
#include "window.hpp"
#include <vector>

namespace libtrainsim{
    namespace Video{
        /**
         * @brief the settings page for the style settings
         * 
         */
        class styleSettings : public tabPage{
          private:
            void content() override;
          public:
            styleSettings();
        };
        
        /**
         * @brief the settings page for the basic settings
         */
        class basicSettings : public tabPage{
          private:
            void content() override;
            const std::array< std::pair<std::string, int> ,4> FBOsizeOptions;
            
            char saveLocation[1000] = "windowSettings.ini\0";
            bool autosave = false;
            size_t fboSizeIndex = 1;
            
            /*
             * checks if the save path is okay
             * 
             * return value:
             *   cleanedLocation
             *   isOkay
            */
            std::tuple<std::filesystem::path, bool> checkSavePath(const std::filesystem::path& location, bool acceptOverwrite, bool denyOverwrite);
            
            /*
             * checks if the load path is okay
             * 
             * return value:
             *   cleanedLocation
             *   isOkay
            */
            std::tuple<std::filesystem::path, bool> checkLoadPath(const std::filesystem::path& location);
            std::tuple<bool,bool> showPopups();
          public:
            basicSettings();
        };
        
        /**
         * @brief a class to handle the settings window
         */
        class settingsWindow : public window{
        private:
            
            /**
             * @brief all of the tabs for the settings window
             */
            std::vector<std::shared_ptr<tabPage>> settingsTabs;
            void content() override;
            
        public:
            settingsWindow();
            void addSettingsTab(std::shared_ptr<tabPage> newTab);
            void removeSettingsTab(std::string_view tabName);
        };
    }
}
