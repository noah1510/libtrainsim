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
    
    try{
        auto str = Helper::getOptionalJsonField<std::string>(data_json, "formatVersion");
        if(str.has_value()){
            version ver = str.value();
            if(version::compare(format_version,ver) < 0){
                throw std::runtime_error(
                    "libtrainsim format version not high enough.\nneeds at least:" + 
                    format_version.print() + " but got:" + ver.print()
                );
            };
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("format version too old"));
    }
    
    auto p = URI.parent_path();
    try{
        serialConfigLocation = p / Helper::getJsonField<std::string>(data_json,"serialConfig");
    }catch(...){
        std::throw_with_nested(std::runtime_error("error getting serial config location"));
    }
    
    try{
        auto dat = Helper::getJsonField(data_json,"tracks");
        if(!dat.is_array()){
            throw std::runtime_error("tracks json filed is not an array");
        }
        
        tracks.reserve(dat.size());
        for(auto _dat : dat){
            if(_dat.is_string()){
                std::filesystem::path loc{_dat.get<std::string>()};
                tracks.emplace_back( libtrainsim::core::Track(p/loc) );
            }else if(_dat.is_object()){
                //construct track from json object
            }else{
                throw std::runtime_error("not a valid track format");
            }
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading tracks"));
    }

    try{
        auto dat = Helper::getJsonField(data_json,"trains");
        if(!dat.is_array()){
            throw std::runtime_error("trains json filed is not an array");
        }
        
        extraTrains.reserve(dat.size());
        for(auto _dat : dat){
            if(_dat.is_string()){
                std::filesystem::path loc{_dat.get<std::string>()};
                extraTrains.emplace_back( libtrainsim::core::train_properties(p/loc) );
            }else if(_dat.is_object()){
                extraTrains.emplace_back( libtrainsim::core::train_properties(_dat) );
            }else{
                throw std::runtime_error("not a valid track format");
            }
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading trains"));
    }
    
    if(tracks.empty()){
        throw std::runtime_error("no track specified in simulator configuration");
    }
    
}

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getSerialConfigLocation() const {
    return serialConfigLocation;
}

