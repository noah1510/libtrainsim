#include "track_configuration.hpp"

#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;
using namespace sakurajin::unit_system::common;
using namespace sakurajin::unit_system::base;

const length& libtrainsim::core::undergorundDataPoint::begin() const {
    return std::get<0>(*this);
}

const length& libtrainsim::core::undergorundDataPoint::end() const {
    return std::get<1>(*this);
}

const sakurajin::unit_system::common::area& libtrainsim::core::undergorundDataPoint::area() const {
    return std::get<2>(*this);
}

libtrainsim::core::undergorundDataPoint::undergorundDataPoint(
    sakurajin::unit_system::base::length _begin,
    sakurajin::unit_system::base::length _end,
    sakurajin::unit_system::common::area _area
):tuple{_begin, _end, _area}{}


const std::string & libtrainsim::core::stopDataPoint::name() const {
    return std::get<0>(*this);
}

const sakurajin::unit_system::base::length & libtrainsim::core::stopDataPoint::position() const {
    return std::get<1>(*this);
}

const libtrainsim::core::stopTypes & libtrainsim::core::stopDataPoint::type() const {
    return std::get<2>(*this);
}

libtrainsim::core::stopDataPoint::stopDataPoint (
    std::string _name, 
    sakurajin::unit_system::base::length _position, 
    libtrainsim::core::stopTypes _type 
): tuple{_name,_position,_type}{}


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
        parseJsonData();
        if(!lazyLoad){
            parseTrack();
            data_json.reset();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("could not parse json data"));
    }
    
}

void libtrainsim::core::Track::parseTrack() {
    if(!data_json.has_value()){
        return;
    }
    
    if(!data_json->is_object()){
        throw std::invalid_argument("the given data is not a json object");
    }
    
    try{
        auto dat = Helper::getJsonField(data_json.value(),"data");
        if(dat.is_string()){
            std::filesystem::path da = parentPath / dat.get<std::string>();
            track_dat = std::make_optional<Track_data>(da);
        }else if(dat.is_array()){
            track_dat = std::make_optional<Track_data>(dat);
        }else{
            throw std::runtime_error("invalid track data format");
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error constructing the track object"));
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
        auto mult = Helper::getOptionalJsonField<double>(data_json.value(), "defaultTrackFrictionMultiplier");
        if(mult.has_value()){
            defaultTrackFrictionMultiplier = mult.value();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error reading defaultTrackFrictionMultiplier"));
    }
    
    try{
        auto under = Helper::getOptionalJsonField(data_json.value(), "undergroundData");
        if(under.has_value()){
            if(!under->is_array()){throw std::runtime_error("Could not read undergroundData. Not an array");};
            
            for(auto _dat : under.value() ){
                auto sta = Helper::getJsonField<double>(_dat, "begin");
                auto en = Helper::getJsonField<double>(_dat, "end");
                auto _ar = Helper::getOptionalJsonField<double>(_dat, "tunnelArea");
                
                auto start = sakurajin::unit_system::base::length{sta};
                auto end = sakurajin::unit_system::base::length{en};
                sakurajin::unit_system::common::area area;
                if(_ar.has_value()){
                    area = sakurajin::unit_system::common::area{_ar.value()};
                }else{
                    area = std::acos(0) * sakurajin::unit_system::common::square(3.5_m);
                }
                
                undergroundData.emplace_back(undergorundDataPoint{start, end, area});
            }
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error reading the underground data"));
    }
    
    try{
        auto stops = Helper::getOptionalJsonField(data_json.value(), "stops");
        if(stops.has_value()){
            if(!stops->is_array()){throw std::runtime_error("Could not read stops data. Not an array");};
            
            for(auto _dat : stops.value() ){
                
                auto name = Helper::getJsonField<std::string>(_dat, "name");
                auto loc = Helper::getJsonField<double>(_dat, "location");
                auto ty = Helper::getJsonField<std::string>(_dat, "type");
                
                auto location = sakurajin::unit_system::base::length{loc};
                stopTypes type;
                if(ty == "station"){
                    type = station;
                }else{
                    throw std::runtime_error("Invalid stop type:" + ty);
                }
                
                stopsData.emplace_back(stopDataPoint{name, location, type});
            }
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error reading the stops data"));
    }
    
    
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
    return endPoint;
}

length Track::firstLocation() const{
    return startingPoint;
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

std::tuple<
    bool, 
    sakurajin::unit_system::common::area, 
    sakurajin::unit_system::base::length
> libtrainsim::core::Track::getUndergroundInfo ( sakurajin::unit_system::base::length position ) const {
    auto end = undergroundData.size();
    if(end == 0){return {false, 0_m2, 0_m};}
    
    for(size_t i = 0; i < end;i++){
        auto point = undergroundData[i];
        if(point.begin() > position && point.end() < position){
            return {true, point.area(), point.end() - position};
        }
    }
    
    return {false, 0_m2, 0_m};
}


void libtrainsim::core::Track::ensure() {
    try{
        parseTrack();
        data_json.reset();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error parsing the json data"));
    }
}


