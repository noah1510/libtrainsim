#include "track_configuration.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::common;
using namespace sakurajin::unit_system::base;

Track::Track(const std::filesystem::path& URI){
        
    if(!std::filesystem::exists(URI)){
        std::cerr << "The Track file location is empty:" << URI.string() << std::endl;
        return;
    }

    if (URI.extension() != ".json" ){
        std::cerr << "the file has no json extention" << std::endl;
        return;
    }

    auto in = std::ifstream(URI);

    in >> data_json;
    
    auto dat = data_json["formatVersion"];
    if(!dat.empty() && dat.is_string()){
        version ver = dat.get<std::string>();
        if(version::compare(format_version,ver) < 0){
            std::cerr << "libtrainsim format version not high enough." << std::endl;
            std::cerr << "needs at least:" << format_version.print() << " but got:" << format_version.print() << std::endl;
            return;
        };
    };
        
    dat = data_json["name"];
    if(!dat.is_string()){
        return;
    }
    name = dat.get<std::string>();
    
    dat = data_json["videoFile"];
    if(!dat.is_string()){
        return;
    }
    const auto p = std::filesystem::absolute(URI.parent_path());
    videoFile = p / dat.get<std::string>();
    if(videoFile.empty()){
        std::cerr << "The Video file location is empty:" << videoFile.string() << std::endl;
        return;
    }
    
    try{
        dat = data_json["data"];
        if(dat.is_string()){
            std::filesystem::path da = p / dat.get<std::string>();
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
        dat = data_json["train"];
        if(dat.is_string()){
            std::filesystem::path tr = p / dat.get<std::string>();
            train_dat = train_properties(tr);
        }else if(dat.is_object()){
            train_dat = train_properties(dat);
        }else{
            throw std::runtime_error("invalid train in track file");
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error constructing the train object"));
    }
    
    dat = data_json["startingPoint"];
    if(dat.is_number_float()){
        startingPoint =  length{dat.get<double>()};
    }else{
        startingPoint = track_dat.firstLocation();
    }
    
    dat = data_json["endPoint"];
    if(dat.is_number_float()){
        endPoint = length{dat.get<double>()};
    }else{
        endPoint = track_dat.lastLocation();
    }
    
    startingPoint = std::clamp(startingPoint,track_dat.firstLocation(),track_dat.lastLocation());
    endPoint = std::clamp(endPoint,track_dat.firstLocation(),track_dat.lastLocation());
    if(startingPoint > endPoint){
        std::cerr << "the last location was smaller than the first position:" << startingPoint << " > " << endPoint << std::endl;
        return;
    };
    
    hasError = false;
    return;
}

const Track_data& Track::data() const{
    return track_dat;
}
            
const train_properties& Track::train() const{
    return train_dat;
}

bool Track::isValid() const{
    return !hasError;
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
    return isValid() ? videoFile : "";
}
