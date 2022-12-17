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
            void displayContent() override;
          public:
            styleSettings();
        };
        
        /**
         * @brief the settings page for the basic settings
         */
        class basicSettings : public tabPage{
          private:
            void displayContent() override;
            const std::array< std::pair<std::string, int> ,4> FBOsizeOptions;
            
            /*
             * checks if the path is okay
             * 
             * return value:
             *   cleanedLocation
             *   isOkay
            */
            std::tuple<std::filesystem::path, bool> checkPath(const std::filesystem::path& location, bool acceptOverwrite, bool denyOverwrite);
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
            void drawContent() override;
            
        public:
            settingsWindow();
            void addSettingsTab(std::shared_ptr<tabPage> newTab);
            void removeSettingsTab(std::string_view tabName);
        };
    }
}
