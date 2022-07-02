#pragma once

#include <exception>
#include <iostream>
#include <nlohmann/json.hpp>

namespace libtrainsim{
    namespace core{
        class Helper{
        private:
            Helper();
            static Helper& get(){
                static Helper instance{};
                return instance;
            }
            void print_exception_impl(const std::exception& e, int level =  0);
        public:
            static void print_exception(const std::exception& e) {
                Helper::get().print_exception_impl(e);
            }
            
            template<typename T>
            static T getJsonField(const nlohmann::json& data_json, const std::string& location){
                auto it = data_json.find(location);
                if(it == data_json.end()){
                    throw nlohmann::json::out_of_range::create(1001, "field does not exist");
                }
                
                auto val = data_json[location];
                if(val.empty()){
                    throw nlohmann::json::invalid_iterator::create(1002, "field exists but is empty");
                }
                
                T retval;
                try{
                    retval = val.get<T>();
                }catch(...){
                    std::throw_with_nested( nlohmann::json::type_error::create(1003, "wrong field type") );
                }
                
                return retval;
            }
            
        };
    }
}
