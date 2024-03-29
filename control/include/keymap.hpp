#pragma once

#include <map>
#include <string>
#include <tuple>
#include <vector>
#include "helper.hpp"

namespace libtrainsim {
    namespace control{
        /**
         * @brief assignes functions to specific keys
         * 
        */
        class LIBTRAINSIM_EXPORT_MACRO keymap{
        private:
            /**
             * @brief The underlying map that assigns a function to each key.
             * 
             */
            std::map<int, std::string> _keymap;
        public:
            /**
             * @brief Construct a new keymap object.
             * 
             */
            keymap();
            
            /**
             * @brief Get a list of all of the keys that have a given function.
             * This can be used to query which keys are assigned to a function and only listen to those keys.
             * 
             * @param function the function that the keys perform
             * @return std::vector<char> the list with all of the keys
             */
            std::vector<int> getKeyList(const std::string& function) const;
            
            /**
             * @brief Get a list of all of the keys that have a function.
             * This returns any keys that have a functions assigned to them.
             * 
             * @return std::vector<std::pair<char, std::string>> the list with all of the keys and their function
             */
            std::vector<std::pair<int, std::string>> getAllKeys() const;
            
            /**
             * @brief add a key to the keymap.
             * @note if the key is already added, this will assign it a new function
             * @param key the key that should be added.
             * @param function the function that the key should have
             */
            void add(int key, const std::string& function);

            /**
             * @brief remove a key from the map.
             * Use this function to unbind key.
             * @warning make sure to always have a key with the CLOSE function
             * unless you have a different method to close the program.
             * @param key the key that should no longer be part of the map
             */
            void remove(int key);
            
            /**
             * @brief Get the Function a given key has.
             * This simply returns the function a gvien key has or NONE if it is not assigned yet.
             * 
             * @param key the key you want to check
             * @return std::string the function that that key has or NONE
             */
            std::string getFunction(int key);
        };
    }
}
