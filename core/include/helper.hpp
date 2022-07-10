#pragma once

#include <exception>
#include <iostream>
#include <optional>
#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core{
        /**
         * @brief A Helper class for Functionality needed across all classes and components
         * 
         */
        class Helper{
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
                
                auto val = data_json[location];
                if(val.empty()){
                    throw nlohmann::json::invalid_iterator::create(1002, "field exists but is empty", nlohmann::basic_json());
                }
                
                return val;
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
            
        };
    }
}
