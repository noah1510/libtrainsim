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
            
        public:
            settingsWindow();
            void addSettingsTab(std::shared_ptr<tabPage> newTab);
            void removeSettingsTab(std::string_view tabName);
            void drawContent() override;
        };
    }
}
