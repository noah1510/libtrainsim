#pragma once

#include <string>
#include <string_view>

namespace libtrainsim{
    namespace Video{
        /**
         * @brief a class to abstract imgui tabs
         * 
         * This class can be used to allow other parts of the code to
         * inject tabs into a configuration window. This is only the
         * template class to provide the general inteface. Because of
         * this it is not possible to create objects of this class
         */
        class tabPage{
          private:
            //The name of this tab
            std::string displayText;
            
            //diable the default constructor
            tabPage() = delete;
          protected:
            /**
             * @brief create a new tabPage with a given name
             */
            tabPage(std::string name);
            
            /**
             * @brief this function displays the content of the tab when needed
             * 
             * To custom tab pages should inherit from this class and override this function.
             * This function should contain all of the Elements that will be drawn in that tab.
             * 
             * @warning keep this function private or protected in child classes since it is not
             * supposed to be called from outside of the class. Use the () operator to draw the tab.
             */
            virtual void content();
          
          public:
            
            /**
             * @brief clean up this tabPage
             */
            virtual ~tabPage();
            
            /**
             * @brief this function draws the tabPage and handles the check if it actually should be drawn.
             */
            void operator()();
            
            /**
             * @brief get the name of the this tab
             */
            std::string_view getName() const; 
        };
    }
}
