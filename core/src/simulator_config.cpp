#include "simulator_config.hpp"

#include <fstream>

libtrainsim::core::simulatorConfiguration::simulatorConfiguration(const std::filesystem::path& URI, bool _lazyLoad, const std::string& _appID): lazyLoad{_lazyLoad}, appID{_appID}{
    if(!std::filesystem::exists(URI)){
        throw std::invalid_argument("The simulator config file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json" ){
        throw std::invalid_argument("the file has no json extention");
    }

    try{
        appDir = Helper::getApplicationDirectory(appID);
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot get the application directory"));
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

    auto logsFolder = appDir / "logs";
    auto debugLogsFolder = logsFolder / "debug";

    try{
        auto loggerConf = Helper::getOptionalJsonField(data_json,"loggerConfig");

        SimpleGFX::loggingLevel coreLogLevel = SimpleGFX::loggingLevel::debug;
        if(loggerConf.has_value()){
            auto logeLevelStr = Helper::getJsonField<std::string>(loggerConf.value(), "logLevel");
            coreLogLevel = SimpleGFX::levelFromString(logeLevelStr);
        }
        coreLogger = std::make_shared<SimpleGFX::logger>(coreLogLevel);

        *coreLogger << SimpleGFX::loggingLevel::debug << "initialized core logger";

        if(loggerConf.has_value()){
            auto extraLogger = Helper::getOptionalJsonField(loggerConf.value(), "extraLoggers");
            if(extraLogger.has_value()){
                *coreLogger << SimpleGFX::loggingLevel::debug << "loading " << extraLogger.value().size() << " extra loggers";

                for(auto& logger: extraLogger.value()){
                    //load all fields from the json object
                    auto logLevelStr = Helper::getJsonField<std::string>(logger, "logLevel");
                    auto logLevel = SimpleGFX::levelFromString(logLevelStr);
                    auto logFile = std::filesystem::path{Helper::getJsonField<std::string>(logger, "file")};
                    auto appendDate = Helper::getOptionalJsonField<bool>(logger, "appendDate", false);
                    auto cleanFile = Helper::getOptionalJsonField<bool>(logger, "cleanFile", false);

                    //add the date to the logFile if needed
                    if(appendDate){
                        logFile = logFile.stem().string() + "_" + SimpleGFX::logger::getCurrentTimestamp() + logFile.extension().string();
                    }

                    //set the log file to the correct location
                    std::filesystem::path logFileLocation = logsFolder / logFile;
                    if(logLevel == SimpleGFX::loggingLevel::debug && appendDate){
                        logFileLocation = debugLogsFolder / logFile;
                    }

                    //create the folder if it does not exist
                    auto logFileFolder = logFileLocation.parent_path();
                    if(!std::filesystem::exists(logFileFolder)){
                        std::filesystem::create_directories(logFileFolder);
                    }

                    //create the logger object for the correct type
                    auto loggerType = Helper::getJsonField<std::string>(logger, "type");
                    if(loggerType == "txt"){
                        SimpleGFX::loggerTxtProperties props{logLevel, logFileLocation};
                        props.cleanFile = cleanFile;

                        auto loggerTxt = std::make_shared<SimpleGFX::loggerTxt>(props);
                        coreLogger->addExtraLogger(loggerTxt);
                        *coreLogger << SimpleGFX::loggingLevel::debug << "added loggerTxt for file: " << logFileLocation;
                    }else if(loggerType == "json"){
                        auto loggerJson = std::make_shared<SimpleGFX::loggerJson>(logLevel, logFileLocation);
                        coreLogger->addExtraLogger(loggerJson);
                        *coreLogger << SimpleGFX::loggingLevel::debug << "added loggerJson for file: " << logFileLocation;
                    }else{
                        *coreLogger << SimpleGFX::loggingLevel::error << "unknown logger type: " << loggerType;
                        throw std::runtime_error("unknown logger type: " + loggerType);
                    }
                }
            }else{
                *coreLogger << SimpleGFX::loggingLevel::debug << "no extra loggers found";
            }
        }

    }catch(...){
        std::throw_with_nested(std::runtime_error("error creating the logger interface"));
    }

    *coreLogger << SimpleGFX::loggingLevel::detail << "loading config file: " << URI;
    *coreLogger << SimpleGFX::loggingLevel::debug << "application ID: " << appID;
    *coreLogger << SimpleGFX::loggingLevel::debug << "application directory: " << appDir.string();

    try{
        serialConfigLocation = p / Helper::getJsonField<std::string>(data_json,"serialConfig");
        *coreLogger << SimpleGFX::loggingLevel::detail << "serial config location: " << serialConfigLocation;
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error getting serial config location"));
    }

    try{
        shaderFolderLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"shaderLocation", "shaders");
        *coreLogger << SimpleGFX::loggingLevel::detail << "shader location: " << shaderFolderLocation;
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Could not load shader location"));
    }

    try{
        textureFolderLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"textureLocation", "textures");
        *coreLogger << SimpleGFX::loggingLevel::detail << "texture location: " << textureFolderLocation;
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("Could not load texture location"));
    }

    try{
        extrasLocation = p / Helper::getOptionalJsonField<std::string>(data_json,"extrasLocation", "extras");
        *coreLogger << SimpleGFX::loggingLevel::detail << "extras location: " << extrasLocation;
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

        for(const auto& _dat : dat){
            if(_dat.is_string()){
                std::filesystem::path loc{_dat.get<std::string>()};
                tracks.emplace_back(libtrainsim::core::Track(p/loc, lazyLoad));
            }else if(_dat.is_object()){
                tracks.emplace_back(libtrainsim::core::Track(_dat, p, lazyLoad));
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
        *coreLogger << SimpleGFX::loggingLevel::detail << "loading " << dat.size() << " extra trains";
        for(const auto& _dat : dat){
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
        auto val = Helper::getOptionalJsonField<int>(data_json, "defaultTrack", 0);
        selectTrack(val);
        *coreLogger << SimpleGFX::loggingLevel::detail << "default track: " << tracks[val].getName() << "index: " << val;
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error setting the default track"));
    }

    try{
        readOnly = Helper::getOptionalJsonField<bool>(data_json,"settingFileReadOnly",false);
        *coreLogger << SimpleGFX::loggingLevel::detail << "setting config files read only: " << readOnly;
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error reading the readOnly option"));
    }

    try{
        inputManager = std::make_shared<SimpleGFX::eventManager>();
        *coreLogger << SimpleGFX::loggingLevel::detail << "input manager created";
    }catch(...){
        coreLogger->logCurrrentException(true);
        std::throw_with_nested(std::runtime_error("error creating the input manager"));
    }

    *coreLogger << SimpleGFX::loggingLevel::normal << "simulator configuration fully loaded";
    fileLocation = std::filesystem::absolute(URI);

    save();
}

void libtrainsim::core::simulatorConfiguration::save(){
    static std::mutex saveMutex;
    std::scoped_lock lock{saveMutex};

    auto launchFile = appDir / "lastLaunch.json";
    *coreLogger << SimpleGFX::loggingLevel::detail << "saving last launch file to: " << launchFile;

    auto lastLaunchObject = nlohmann::json::object();

    if(std::filesystem::exists(launchFile)) {
        *coreLogger << SimpleGFX::loggingLevel::detail << "loading last state of the file";
        std::ifstream fileLoad(launchFile);
        fileLoad >> lastLaunchObject;
    }

    lastLaunchObject["lastConfigFile"] = fileLocation.string();
    lastLaunchObject["lazyLoad"] = lazyLoad;
    lastLaunchObject["loadedTrack"] = currentTrack;

    try{
        std::ofstream file{launchFile};
        file << lastLaunchObject.dump(4);
    }catch(...){
        *coreLogger << SimpleGFX::loggingLevel::warning << "could not save last launch file";
        coreLogger->logCurrrentException(false);
    }
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
        *coreLogger << SimpleGFX::loggingLevel::debug << "selecting track id: " << index << " name: " << getTrack(index).getName();
        ensureTrack(index);
    }catch(...){
        coreLogger->logCurrrentException(false);
        std::throw_with_nested("could not ensure availability of the given track");
    }

    currentTrack = index;
    if(autosave){
        save();
    }
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

const std::string& libtrainsim::core::simulatorConfiguration::getAppID() const noexcept {
    return appID;
}

std::shared_ptr<SimpleGFX::logger> libtrainsim::core::simulatorConfiguration::getLogger() noexcept {
    return coreLogger;
}

std::shared_ptr<SimpleGFX::eventManager> libtrainsim::core::simulatorConfiguration::getInputManager() noexcept {
    return inputManager;
}
