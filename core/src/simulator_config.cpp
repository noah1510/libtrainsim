#include "simulator_config.hpp"

#include <fstream>

libtrainsim::core::simulatorConfiguration::simulatorConfiguration(const std::filesystem::path& URI, bool lazyLoad){
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
        shaderFolderLocation = p / Helper::getJsonField<std::string>(data_json,"shaderLocation");
    }catch(...){
        std::throw_with_nested(std::runtime_error("Could not load shader location"));
    }
    
    try{
        auto dat = Helper::getJsonField(data_json,"tracks");
        if(!dat.is_array()){
            throw std::runtime_error("tracks json filed is not an array");
        }
        
        tracks.reserve(dat.size());
        
        //unsigned int n_threads = std::thread::hardware_concurrency();
        std::vector< std::future<libtrainsim::core::Track> > loadingQueue;
        
        for(size_t i = 0;i < dat.size();i++){
            if(dat[i].is_string()){
                std::filesystem::path loc{dat[i].get<std::string>()};
                loadingQueue.emplace_back( std::async(std::launch::async, [p,loc,lazyLoad](){return libtrainsim::core::Track(p/loc, lazyLoad);} ) );
            }else if(dat[i].is_object()){
                //construct track from json object
            }else{
                throw std::runtime_error("not a valid track format");
            }
        }
        
        for(auto& v:loadingQueue){
            if(!v.valid()){
                throw std::runtime_error("got a non valid future 1");
            }
            v.wait();
            if(!v.valid()){
                throw std::runtime_error("got a non valid future 2");
            }
            auto val = v.get();
            tracks.emplace_back(val);
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

    try{
        auto val = Helper::getOptionalJsonField<int>(data_json, "defaultTrack");
        if(val.has_value()){
            selectTrack(val.value());
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error setting the default track"));
    }
    
    try{
        auto val = Helper::getOptionalJsonField<bool>(data_json,"settingFileReadOnly");
        if(val.has_value()){
            readOnly = val.value();
        }
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading the readOnly option"));
    }
    
    fileLocation = URI;
    
}

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getSerialConfigLocation() const noexcept{
    return serialConfigLocation;
}

const libtrainsim::core::Track & libtrainsim::core::simulatorConfiguration::getCurrentTrack() const noexcept{
    return tracks[currentTrack];
}

uint64_t libtrainsim::core::simulatorConfiguration::getTrackCount() const noexcept {
    return tracks.size();
}

void libtrainsim::core::simulatorConfiguration::selectTrack ( uint64_t index ) {
    try{
        ensureTrack(index);
    }catch(...){
        std::throw_with_nested("could not ensure availability of the given track");
    }
    
    currentTrack = index;
}

const libtrainsim::core::Track & libtrainsim::core::simulatorConfiguration::getTrack ( uint64_t index ) const noexcept(false) {
    if(index > getTrackCount()){
        throw std::invalid_argument("track index too high");
    }
    
    return tracks[index];
}

void libtrainsim::core::simulatorConfiguration::ensureTrack ( uint64_t index ) noexcept(false) {
    if(getTrackCount() <= index){
        throw std::invalid_argument("track index too high");
    }
    
    try{
        tracks[index].ensure();
    }catch(...){
        std::throw_with_nested(std::runtime_error("error ensuring the availability of the selected track"));
    }
}

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getShaderLocation() const noexcept {
    return shaderFolderLocation;
}


