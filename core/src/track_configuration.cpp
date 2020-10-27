#include "track_configuration.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;

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
    
    dat = data_json["data"];
    if(dat.is_string()){
        auto da = p / dat.get<std::string>();
        track_dat = Track_data(da);
    }else if(dat.is_array()){
        track_dat = Track_data(dat);
    }else{
        return;
    }
    
    dat = data_json["train"];
    if(dat.is_string()){
        const auto tr = p / dat.get<std::string>();
        train_dat = train_properties(tr);
    }else if(dat.is_object()){
        train_dat = train_properties(dat);
    }else{
        return;
    }
    
    if(!track_dat.isValid() || !train_dat.isValid()){return;};
    
    dat = data_json["startingPoint"];
    if(dat.is_number_float()){
        startingPoint = dat.get<double>();
    }else{
        startingPoint = track_dat.firstLocation();
    }
    
    dat = data_json["endPoint"];
    if(dat.is_number_float()){
        endPoint = dat.get<double>();
    }else{
        endPoint = track_dat.lastLocation();
    }
    
    startingPoint = clamp<double>(startingPoint,track_dat.firstLocation(),track_dat.lastLocation());
    endPoint = clamp<double>(endPoint,track_dat.firstLocation(),track_dat.lastLocation());
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

double Track::lastLocation() const{
    auto loc = data().lastLocation();
    return (endPoint < loc) ? endPoint : loc;
}

double Track::firstLocation() const{
    auto loc = data().firstLocation();
    return (startingPoint > loc) ? startingPoint : loc;
}

std::filesystem::path Track::getVideoFilePath() const{
    return isValid() ? videoFile : "";
}
