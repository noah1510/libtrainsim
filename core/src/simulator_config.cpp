#include "simulator_config.hpp"

#include <fstream>

libtrainsim::core::simulatorConfiguration::simulatorConfiguration(const std::filesystem::path& URI){
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The simulator config file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }
    
    auto in = std::ifstream(URI);
    nlohmann::json data_json;
    
    in >> data_json;
    
    auto dat = data_json["formatVersion"];
    if(!dat.empty() && dat.is_string()){
        version ver = dat.get<std::string>();
        if(version::compare(format_version,ver) < 0){
            throw std::runtime_error(
                "libtrainsim format version not high enough.\nneeds at least:" + 
                format_version.print() + " but got:" + format_version.print()
            );
        };
    };
    
    dat = data_json["serialConfig"];
    if(dat.empty() || !dat.is_string()){
        throw std::runtime_error("invalid serialConfig field");
    }
    serialConfigLocation = dat.get<std::string>();
    
    dat = data_json["tracks"];
    if(dat.empty() || !dat.is_array()){
        throw std::runtime_error("invalid tracks field");
    }
    tracks.reserve(dat.size());
    for(auto _dat : dat){
        try{
            if(_dat.is_string()){
                tracks.emplace_back( libtrainsim::core::Track(_dat.get<std::string>()) );
            }else if(_dat.is_object()){
                //construct track from json object
            }else{
                throw std::runtime_error("not a valid track format");
            }
        }catch(...){
            std::throw_with_nested(std::runtime_error("error when constructing a Track object"));
        }
    }
    
    dat = data_json["trains"];
    if(dat.empty() || !dat.is_array()){
        throw std::runtime_error("invalid trains field");
    }
    extraTrains.reserve(dat.size());
    for(auto _dat : dat){
        try{
            if(_dat.is_string()){
                extraTrains.emplace_back( libtrainsim::core::train_properties(_dat.get<std::string>()) );
            }else if(_dat.is_object()){
                extraTrains.emplace_back( libtrainsim::core::train_properties(_dat) );
            }else{
                throw std::runtime_error("not a valid train format");
            }
        }catch(...){
            std::throw_with_nested(std::runtime_error("error when constructing a Train object"));
        }
    }
    
}
