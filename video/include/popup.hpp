#pragma once

#include <string>
#include <string_view>

namespace libtrainsim{
    namespace Video{
        /**
         * @brief a class to abstract imgui popups
         * 
         * This class makes creating and updating popups a bit
         * esier. Drawing popups can be a huge pain since the
         * draw functions have to be called in the correct place
         * which can be difficult.
         */
        class popup{
          private:
            //The name of this popup
            std::string displayText;
            
            //diable the default constructor
            popup() = delete;
          protected:
            /**
             * @brief create a new popup with a given name
             */
            popup(std::string name);
            
            /**
             * @brief this function displays the content of the popup when needed
             * 
             * To custom popups should inherit from this class and override this function.
             * This function should contain all of the Elements that will be drawn in that popup.
             * 
             * @warning keep this function private or protected in child classes since it is not
             * supposed to be called from outside of the class. Use the () operator to draw the tab.
             */
            virtual void content();
          
          public:
            
            /**
             * @brief clean up this tabPage
             */
            virtual ~popup();
            
            /**
             * @brief this function draws the tabPage and handles the check if it actually should be drawn.
             */
            void operator()();
            
            /**
             * @brief open the popup
             */
            void open();
            
            /**
             * @brief get the name of the this tab
             */
            std::string_view getName() const; 
        };
    }
}

