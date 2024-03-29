#pragma once

#include <exception>
#include <iostream>
#include <optional>
#include <vector>
#include <type_traits>
#include <chrono>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#ifndef LIBTRAINSIM_EXPORT_MACRO
#define LIBTRAINSIM_EXPORT_MACRO
#endif

namespace libtrainsim{
    namespace core{
        /**
         * @brief A Helper class for Functionality needed across all classes and components
         * 
         */
        class LIBTRAINSIM_EXPORT_MACRO Helper{
        private:
            /**
             * @brief Construct a new Helper object
             * 
             */
            Helper();

            /**
             * @brief get the instance of the helper
             * This function basically guarantees that there is only one instance of the helper
             * 
             * @return Helper& 
             */
            static Helper& get(){
                static Helper instance{};
                return instance;
            }

            /**
             * @brief the implementation of the print_exception function
             * 
             * @param e the (nested) exception
             * @param level the identation level for the exception
             */
            void print_exception_impl(const std::exception& e, int level =  0);
        public:
            /**
             * @brief print a (nested) exception
             * 
             * This funciton will add an identation for every nested exception level
             * 
             * @param e the (nested) exception
             */
            static void print_exception(const std::exception& e) {
                Helper::get().print_exception_impl(e);
            }

            /**
            * @brief splits a string into several others
            *
            * @param s the string to be split
            * @param delimiter the character that determines where to split the string
            * @return std::vector<std::string> a vector containing with the split parts of the string.
            */
            static std::vector<std::string> split_string(const std::string& s, char delimiter){
                std::vector<std::string> tokens;
                std::string token;
                std::istringstream tokenStream(s);
                while (std::getline(tokenStream, token, delimiter)){
                    tokens.push_back(token);
                }
                return tokens;
            }
            
            /**
             * @brief Get a json field from an json data object
             * 
             * This function prevents the program from crashing if the field does not exist.
             * 
             * @param data_json the json data object
             * @param location the name of the wanted field
             * @return nlohmann::json the value of the field
             */
            static nlohmann::json getJsonField(const nlohmann::json& data_json, const std::string& location){
                auto it = data_json.find(location);
                if(it == data_json.end()){
                    throw nlohmann::json::out_of_range::create(1001, "field does not exist", nlohmann::basic_json());
                }
                
                auto val = it.value();
                if(val.empty()){
                    throw nlohmann::json::invalid_iterator::create(1002, "field exists but is empty", nlohmann::basic_json());
                }
                
                return val;
            }
            
            /**
             * @brief Get a json field from an json data object
             * 
             * This function prevents the program from crashing if the field does not exist.
             * 
             * @param data_json the json data object
             * @param location the name of the wanted field
             * @return nlohmann::json the value of the field
             */
            static std::optional<nlohmann::json> getOptionalJsonField(const nlohmann::json& data_json, const std::string& location){
                try{
                    auto val = getJsonField(data_json,location);
                    return val;
                }catch(...){
                    return {};
                }
                
            }
            
            /**
             * @brief Get a value from a json field
             * 
             * This function prevents the program from crashing if the field does not exist.
             * It also handles the type conversion and throws an error if the field cannot be converted.
             * 
             * @tparam T The type the field should have
             * @param data_json the json data object
             * @param location the name of the wanted field
             * @return T The value of the json field in the requested type
             */
            template<typename T>
            static T getJsonField(const nlohmann::json& data_json, const std::string& location){
                nlohmann::json val;
                try{
                    val = getJsonField(data_json,location);
                }catch(const nlohmann::json::exception& e){
                    throw nlohmann::json::other_error::create(e.id, e.what(), nlohmann::basic_json());
                }
                
                T retval;
                try{
                    retval = val.get<T>();
                }catch(...){
                    std::throw_with_nested( nlohmann::json::type_error::create(1003, "wrong field type", nlohmann::basic_json()) );
                }
                
                return retval;
            }
            
            /**
             * @brief Get an optional value from json data
             * 
             * This function prevents the program from crashing if the field does not exist.
             * It also handles the type conversion and throws an error if the field cannot be converted.
             * This function will not throw errors when there is no value only if the type is wrong.
             * To check if the value exists you can use the exists function of the return value.
             * 
             * @tparam T The type the field should have
             * @param data_json the json data object
             * @param location the name of the wanted field
             * @return std::optional<T> The value of the json field in the requested type or null if it does not exist
             */
            template<typename T>
            static std::optional<T> getOptionalJsonField(const nlohmann::json& data_json, const std::string& location){
                T val;
                try{
                    val = getJsonField<T>(data_json,location);
                }catch(const nlohmann::json::exception&){
                    return {};
                }catch(...){
                    std::throw_with_nested("error parsing optional field");
                }
                
                return val;
            }
            
            /**
             * @brief Get an optional value from json data
             * 
             * This function prevents the program from crashing if the field does not exist.
             * It also handles the type conversion and throws an error if the field cannot be converted.
             * This function will not throw errors when there is no value only if the type is wrong.
             * This function returns fallback value in case the optional value does not exist
             * 
             * @tparam T The type the field should have
             * @param data_json the json data object
             * @param location the name of the wanted field
             * @param fallbackValue the value to return if the wanted field does not exist
             * @return T The value of the json field in the requested type or fallbackValue if it does not exist
             */
            template<typename T>
            static T getOptionalJsonField(const nlohmann::json& data_json, const std::string& location, const T& fallbackValue){
                T val;
                try{
                    val = getJsonField<T>(data_json,location);
                }catch(const nlohmann::json::exception&){
                    return fallbackValue;
                }catch(...){
                    std::throw_with_nested("error parsing optional field");
                }
                
                return val;
            }
            
            /**
             * @brief checks if a vector contains a given element
             * @warning The function only returns if the object was found not where it is!
             * 
             * @tparam T The type of all the array elements
             * @param data The array containing the data to be searched through
             * @param key The value that is being searched for
             * @return true The array contains the element
             * @return false the array does not contain the element
             */
            template<typename T>
            static bool contains(const std::vector<T>& data, const T& key){
                for(auto x : data){
                    if(x == key){
                        return true;
                    }
                }
                return false;
            }
            
            /**
             * @brief append a single value to an array
             * This function move all of the elements in the array by one and
             * and then adds the new element to the back
             * 
             * @tparam T The type of the value and the array
             * @tparam SIZE The number of elements this array has
             * @param data The array the value should be stored in
             * @param value The value to be stored in the array
             */
            template<typename T, size_t SIZE>
            static void appendValue(std::array<T, SIZE>& data, const T& value){
                for(size_t i = 0; i < SIZE;i++){
                    data[i] = data[i+1];
                }
                data[SIZE-1] = value;
            }

            /**
             * @brief This returns the current time point
             * 
             * @return std::chrono::time_point<std::chrono::high_resolution_clock> The current time point
             */
            static std::chrono::time_point<std::chrono::high_resolution_clock> now(){
                return std::chrono::high_resolution_clock::now();
            }
            
            /**
             * @brief like vector.emplaceBack(val) but value can be a vector
             * 
             * @tparam T The type of the vector elements
             * @param destination The vector the data will be inserted to
             * @param source The vector where the new data is coming from
             */
            template <typename T>
            static void emplaceBack(std::vector<T>& destination, const std::vector<T>& source){
                destination.insert(destination.end(), source.begin(), source.end());
            }
            
            /**
             * @brief compares if two values are equal with a tolerance
             * 
             * 
             * @tparam T The type of the values
             * @param value1 The first value
             * @param value2 The second value
             * @param precision The allowed tolerance. The default value of 1 per 1000 shoould be fine for most cases.
             * @return will return true if they are equal within the tolerance and false if not
             */
            template <typename T>
            static bool isRoughly(T value1, T value2, double precision = 0.001){
                if(value1 == value2){return true;}; //this should get 0 comparison working
                
                bool val1Larger = std::abs(value1) > std::abs(value2);
                double epsilon = static_cast<double>(std::abs( val1Larger ? value1 : value2 )) * precision;
                
                return std::abs(value1 - value2) <= static_cast<T>(epsilon);
            }
            
            /**
             * @brief This functions kind of allows strings to be used for a stringSwitch
             * 
             * This function needs a value which would go into the switch() expression and a vector
             * with all of the cases. It then returns the first matched index which can be used in cases.
             * 
             * If no item matches -1 is returned.
             * 
             * So the intendet use is the following:
             * 
             *    ```cpp
             *       switch(Helper::stringSwitch(myString,{"a","b"})){
             *           case(0):
             *               std::cout << "is a" << std::endl;
             *               break;
             *           case(1):
             *               std::cout << "is b" << std::endl;
             *               break;
             *           default:
             *               std::cout << "is something else" << std::endl;
             *               break;
             *       }
             *    ```
             * 
             * @param value The variable that is going to be checked in the switch
             * @param cases An array (vector) containing all of the cases in order
             * @return The index of the case that matches
             * 
             */
            static int64_t stringSwitch(const std::string& value, const std::vector<std::string>& cases){
                for(size_t i = 0; i < cases.size(); i++){
                    if(cases[i] == value){return i;};
                }
                
                return -1;
            }
            
            /**
             * @brief load a file into a string
             * This function provides a simple interface to load a file into a string.
             * 
             * @param location The file location to read from
             * @return The data as string
             * 
             * @throws std::nested_exception if anything went wrong when reading the file
             */
            static std::string loadFile(std::filesystem::path location){
                std::string stringData;
                std::ifstream File;
                // ensure ifstream objects can throw exceptions:
                File.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                try {
                    // open files
                    File.open(location);
                    std::stringstream DataStream;
                    // read file's buffer contents into streams
                    DataStream << File.rdbuf();
                    // close file handlers
                    File.close();
                    // convert stream into string
                    stringData = DataStream.str();
                } catch (...) {
                    std::throw_with_nested(std::runtime_error("Shader file not sucessfully read"));
                }
                
                return stringData;
            }
            
        };
    }
}
