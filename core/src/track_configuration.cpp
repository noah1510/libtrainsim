#include "track_configuration.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::common;
using namespace sakurajin::unit_system::base;

Track::Track(const std::filesystem::path& URI, bool lazyLoad){
        
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The Track file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }
    
    parentPath = URI.parent_path();
    
    try{
        auto in = std::ifstream(URI);
        nlohmann::json _dat;
        in >> _dat;
        data_json = _dat;
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not read file into json structure"));
    }
    
    try{
        if(!lazyLoad){
            parseJsonData();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not parse json data"));
    }
    
}

void Track::parseJsonData(){
    
    if(!data_json.has_value()){
        return;
    }
    
    if(!data_json->is_object()){
        throw std::invalid_argument("the given data is not a json object");
    }
    
    try{
        auto str = Helper::getOptionalJsonField<std::string>(data_json.value(), "formatVersion");
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
    
    try{
        name = Helper::getJsonField<std::string>(data_json.value(), "name");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read name field"));
    }
    
    try{
        videoFile = parentPath / Helper::getJsonField<std::string>(data_json.value(), "videoFile");
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not read video file field"));
    }
    
    try{
        auto dat = Helper::getJsonField(data_json.value(),"data");
        if(dat.is_string()){
            std::filesystem::path da = parentPath / dat.get<std::string>();
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
        auto dat = Helper::getJsonField(data_json.value(),"train");
        if(dat.is_string()){
            std::filesystem::path tr = parentPath/dat.get<std::string>();
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
        startingPoint.value = Helper::getJsonField<double>(data_json.value(),"startingPoint");
    }catch(...){
        startingPoint = track_dat->firstLocation();
    }
    
    try{
        endPoint.value = Helper::getJsonField<double>(data_json.value(),"endPoint");
    }catch(...){
        endPoint = track_dat->lastLocation();
    }
    
    startingPoint = std::clamp(startingPoint,track_dat->firstLocation(),track_dat->lastLocation());
    endPoint = std::clamp(endPoint,track_dat->firstLocation(),track_dat->lastLocation());
    if(startingPoint > endPoint){
        throw std::runtime_error("the last location was smaller than the first position");
    };
    
    data_json.reset();
}

const Track_data& Track::data() const{
    if(!track_dat.has_value()){
        throw std::runtime_error("Track not loaded yet");
    }
    return track_dat.value();
}
            
const train_properties& Track::train() const{
    if(!train_dat.has_value()){
        throw std::runtime_error("Train not loaded yet");
    }
    return train_dat.value();
}

length Track::lastLocation() const{
    try{
        auto loc = data().lastLocation();
        return (endPoint < loc) ? endPoint : loc;
    }catch(...){
        std::throw_with_nested(std::runtime_error("error acessing data"));
    }
}

length Track::firstLocation() const{
    try{
        auto loc = data().firstLocation();
        return (startingPoint > loc) ? startingPoint : loc;
    }catch(...){
        std::throw_with_nested(std::runtime_error("error acessing data"));
    }
}

const std::string & libtrainsim::core::Track::getName() const {
    if(!track_dat.has_value()){
        throw std::runtime_error("Track not loaded yet");
    }
    
    return name;
}


std::filesystem::path Track::getVideoFilePath() const{
    return videoFile;
}

void libtrainsim::core::Track::ensure() {
    try{
        parseJsonData();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error parsing the json data"));
    }
}

