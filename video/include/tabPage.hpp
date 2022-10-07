#pragma once

#include <string>
#include <string_view>

namespace libtrainsim{
    namespace Video{
        class tabPage{
          private:
            std::string displayText;
          public:
            tabPage(std::string name);
            virtual ~tabPage();
            
            virtual void displayContent();
            void operator()();
            
            std::string_view getName() const; 
        };
    }
}
