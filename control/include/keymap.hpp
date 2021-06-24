#pragma once

#include <map>
#include <string>
#include <vector>

namespace libtrainsim {
    namespace control{
        /**
         * @brief assignes functions to specific keys
         * 
         */
        class keymap{
        private:
            std::map<char, std::string> _keymap;
        public:
            keymap();
            
            std::vector<char> getKeyList(const std::string& function) const;
            
            void add(char key, const std::string& function);
            void remove(char key);
            
            std::string getFunction(char key);
        };
    }
}
