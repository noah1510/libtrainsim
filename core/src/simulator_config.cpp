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
        auto level = Helper::getOptionalJsonField<SimpleGFX::loggingLevel>(data_json,"logLevel",SimpleGFX::loggingLevel::debug);
        coreLogger = std::make_shared<SimpleGFX::logger>(level);
    }catch(...){
        std::throw_with_nested(std::runtime_error("error reading the logLevel option"));
    }

    //@todo add code and documentation for additional loggers. Available options: loggerTxt, loggerJson

    try{
        serialConfigLocation = p / Helper::getJsonField<std::string>(data_json,"serialConfig");
        coreLogger->logMessage("serial config location: " + serialConfigLocation.string(),SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error getting serial config location"));
    }

    try{
        shaderFolderLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"shaderLocation", "shaders");
        coreLogger->logMessage("shader location: " + shaderFolderLocation.string(),SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Could not load shader location"));
    }

    try{
        textureFolderLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"textureLocation", "textures");
        coreLogger->logMessage("texture location: " + textureFolderLocation.string(),SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Could not load texture location"));
    }

    try{
        extrasLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"extrasLocation", "extras");
        coreLogger->logMessage("extras location: " + extrasLocation.string(),SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Error reading the extras location"));
    }

    try{
        auto dat = Helper::getJsonField(data_json,"tracks");
        if(!dat.is_array()){
            coreLogger->logCurrrentException(true);
            throw std::runtime_error("tracks json filed is not an array");
        }

        tracks.reserve(dat.size());

        for(size_t i = 0;i < dat.size();i++){
            if(dat[i].is_string()){
                std::filesystem::path loc{dat[i].get<std::string>()};
                tracks.emplace_back(libtrainsim::core::Track(p/loc, lazyLoad));
            }else if(dat[i].is_object()){
                tracks.emplace_back(libtrainsim::core::Track(dat[i], p, lazyLoad));
            }else{
                coreLogger->logCurrrentException(true);
                throw std::runtime_error("not a valid track format");
            }
        }

    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error reading tracks"));
    }

    try{
        auto dat = Helper::getJsonField(data_json,"trains");
        if(!dat.is_array()){
            coreLogger->logCurrrentException(true);
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
                coreLogger->logCurrrentException(true);
                throw std::runtime_error("not a valid track format");
            }
        }
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error reading trains"));
    }

    if(tracks.empty()){
        coreLogger->logCurrrentException(true);
        throw std::runtime_error("no track specified in simulator configuration");
    }

    try{
        auto val = Helper::getOptionalJsonField<int>(data_json, "defaultTrack",0);
        selectTrack(val);
        coreLogger->logMessage("default track: " + tracks[val].getName(), SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error setting the default track"));
    }

    try{
        readOnly = Helper::getOptionalJsonField<bool>(data_json,"settingFileReadOnly",false);
        coreLogger->logMessage("setting config files read only: " + std::to_string(readOnly),SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error reading the readOnly option"));
    }

    try{
        inputManager = std::make_shared<SimpleGFX::eventManager>();
        coreLogger->logMessage("input manager created",SimpleGFX::loggingLevel::detail);
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error creating the input manager"));
    }

    coreLogger->logMessage("simulator configuration fully loaded",SimpleGFX::loggingLevel::normal);
    fileLocation = URI;

}

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getSerialConfigLocation() const noexcept{
    return serialConfigLocation;
}

const libtrainsim::core::Track & libtrainsim::core::simulatorConfiguration::getCurrentTrack() const noexcept{
    return tracks[currentTrack];
}

uint64_t libtrainsim::core::simulatorConfiguration::getCurrentTrackID() const noexcept {
    return currentTrack;
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

libtrainsim::core::Track & libtrainsim::core::simulatorConfiguration::getTrack ( uint64_t index ) noexcept(false) {
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

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getTextureLocation() const noexcept {
    return textureFolderLocation;
}


const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getShaderLocation() const noexcept {
    return shaderFolderLocation;
}

const std::filesystem::path & libtrainsim::core::simulatorConfiguration::getExtrasLocation() const noexcept {
    return extrasLocation;
}

std::shared_ptr<SimpleGFX::logger> libtrainsim::core::simulatorConfiguration::getLogger() noexcept {
    return coreLogger;
}

std::shared_ptr<SimpleGFX::eventManager> libtrainsim::core::simulatorConfiguration::getInputManager() noexcept {
    return inputManager;
}
