#pragma once

#include "tabPage.hpp"
#include "window.hpp"
#include "popup.hpp"

#include <vector>

namespace libtrainsim{
    namespace Video{
        class LIBTRAINSIM_EXPORT_MACRO noIniPopup : public popup{
          protected:
            void content() override;
          public:
            noIniPopup();
        };
        
        class LIBTRAINSIM_EXPORT_MACRO emptyFilePopup: public popup{
          protected:
            void content() override;
          public:
            emptyFilePopup();
        };
        
        class LIBTRAINSIM_EXPORT_MACRO overwriteFilePopup: public popup{
          protected:
            bool acceptOverwrite = false;
            bool denyOverwrite = false;
            void content() override;
          public:
            overwriteFilePopup();
            
            std::tuple<bool,bool> getChoices();
        };
      
        /**
         * @brief the settings page for the style settings
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO styleSettings : public tabPage{
          private:
            void content() override;
          public:
            styleSettings();
        };
        
        /**
         * @brief the settings page for the basic settings
         */
        class LIBTRAINSIM_EXPORT_MACRO basicSettings : public tabPage{
          private:
            void content() override;
            const std::array< std::pair<std::string, int> ,4> FBOsizeOptions;
            noIniPopup noIni{};
            emptyFilePopup noFile{};
            overwriteFilePopup askOverwrite{};
            
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
            std::tuple<std::filesystem::path, bool> checkSavePath(const std::filesystem::path& location);
            
            /*
             * checks if the load path is okay
             * 
             * return value:
             *   cleanedLocation
             *   isOkay
            */
            std::tuple<std::filesystem::path, bool> checkLoadPath(const std::filesystem::path& location);
          public:
            basicSettings();
        };
        
        /**
         * @brief a class to handle the settings window
         */
        class LIBTRAINSIM_EXPORT_MACRO settingsWindow : public window{
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
