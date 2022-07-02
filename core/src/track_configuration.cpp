#include "track_configuration.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::common;
using namespace sakurajin::unit_system::base;

Track::Track(const std::filesystem::path& URI){
        
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The Track file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }

    nlohmann::json data_json;
    
    try{
        auto in = std::ifstream(URI);
        in >> data_json;
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not read file into json structure"));
    }
    
    try{
        parseJsonData(data_json, URI.parent_path());
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not parse json data"));
    }
    
}

void Track::parseJsonData(const nlohmann::json& data_json, const std::filesystem::path& p){
    
    if(!data_json.is_object()){
        throw std::invalid_argument("the given data is not a json object");
    }
    
    try{
        version ver = Helper::getJsonField<std::string>(data_json, "formatVersion");
        if(version::compare(format_version,ver) < 0){
            throw std::runtime_error(
                "libtrainsim format version not high enough.\nneeds at least:" + 
                format_version.print() + " but got:" + format_version.print()
            );
        };
    }catch(const nlohmann::json::exception& ){
    }catch(...){
        std::throw_with_nested(std::runtime_error("format version too old"));
    }
    
    try{
        name = Helper::getJsonField<std::string>(data_json, "name");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read name field"));
    }
    
    try{
        videoFile = p / Helper::getJsonField<std::string>(data_json, "videoFile");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read video file field"));
    }
    
    try{
        auto dat = data_json["data"];
        if(dat.is_string()){
            std::filesystem::path da = p/dat.get<std::string>();
            track_dat = Track_data(da);
        }else if(dat.is_array()){
            track_dat = Track_data(dat);
        }else{
            throw std::runtime_error("invalid track data format");
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error constructing the track object"));
    }
    
    try{
        auto dat = data_json["train"];
        if(dat.is_string()){
            std::filesystem::path tr = p/dat.get<std::string>();
            train_dat = train_properties(tr);
        }else if(dat.is_object()){
            train_dat = train_properties(dat);
        }else{
            throw std::runtime_error("invalid train in track file");
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error constructing the train object"));
    }
    
    try{
        startingPoint.value = Helper::getJsonField<double>(data_json,"startingPoint");
    }catch(...){
        startingPoint = track_dat->firstLocation();
    }
    
    try{
        endPoint.value = Helper::getJsonField<double>(data_json,"endPoint");
    }catch(...){
        endPoint = track_dat->lastLocation();
    }
    
    startingPoint = std::clamp(startingPoint,track_dat->firstLocation(),track_dat->lastLocation());
    endPoint = std::clamp(endPoint,track_dat->firstLocation(),track_dat->lastLocation());
    if(startingPoint > endPoint){
        throw std::runtime_error("the last location was smaller than the first position");
    };
}

const Track_data& Track::data() const{
    return track_dat.value();
}
            
const train_properties& Track::train() const{
    return train_dat.value();
}

length Track::lastLocation() const{
    auto loc = data().lastLocation();
    return (endPoint < loc) ? endPoint : loc;
}

length Track::firstLocation() const{
    auto loc = data().firstLocation();
    return (startingPoint > loc) ? startingPoint : loc;
}

std::filesystem::path Track::getVideoFilePath() const{
    return videoFile;
}
