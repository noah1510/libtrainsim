#include "track_configuration.hpp"

using namespace libtrainsim::core;
using namespace sakurajin::unit_system;

const length& libtrainsim::core::undergorundDataPoint::begin() const {
    return std::get<0>(*this);
}

const length& libtrainsim::core::undergorundDataPoint::end() const {
    return std::get<1>(*this);
}

const sakurajin::unit_system::area& libtrainsim::core::undergorundDataPoint::area() const {
    return std::get<2>(*this);
}

libtrainsim::core::undergorundDataPoint::undergorundDataPoint(sakurajin::unit_system::length _begin,
                                                              sakurajin::unit_system::length _end,
                                                              sakurajin::unit_system::area   _area)
    : tuple{_begin, _end, _area} {}


const std::string& libtrainsim::core::stopDataPoint::name() const {
    return std::get<0>(*this);
}

const sakurajin::unit_system::length& libtrainsim::core::stopDataPoint::position() const {
    return std::get<1>(*this);
}

const libtrainsim::core::stopTypes& libtrainsim::core::stopDataPoint::type() const {
    return std::get<2>(*this);
}

libtrainsim::core::stopDataPoint::stopDataPoint(std::string                    _name,
                                                sakurajin::unit_system::length _position,
                                                libtrainsim::core::stopTypes   _type)
    : tuple{_name, _position, _type} {}


Track::Track(const std::filesystem::path& URI, bool lazyLoad) {

    if (!std::filesystem::exists(URI)) {
        throw std::invalid_argument("The Track file location is empty:" + URI.string());
    }

    if (URI.extension() != ".json") {
        throw std::invalid_argument("the file has no json extention");
    }

    parentPath = URI.parent_path();

    try {
        auto           in = std::ifstream(URI);
        nlohmann::json _dat;
        in >> _dat;
        data_json = _dat;
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not read file into json structure"));
    }

    try {
        parseJsonData();
        if (!lazyLoad) {
            parseTrack();
            data_json.reset();
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not parse json data"));
    }
}

Track::Track(const nlohmann::json& _data_json, const std::filesystem::path& _parentPath, bool lazyLoad) {

    parentPath = _parentPath;
    data_json  = _data_json;

    try {
        parseJsonData();
        if (!lazyLoad) {
            parseTrack();
            data_json.reset();
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not parse json data"));
    }
}

void libtrainsim::core::Track::parseTrack() {
    if (!data_json.has_value()) {
        return;
    }

    if (!data_json->is_object()) {
        throw std::invalid_argument("the given data is not a json object");
    }

    try {
        auto dat = Helper::getJsonField(data_json.value(), "data");
        if (dat.is_string()) {
            std::filesystem::path da = parentPath / dat.get<std::string>();
            track_dat                = std::make_optional<Track_data>(da);
        } else if (dat.is_array()) {
            track_dat = std::make_optional<Track_data>(dat);
        } else {
            throw std::runtime_error("invalid track data format");
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error constructing the track object"));
    }

    try {
        startingPoint.value = Helper::getJsonField<double>(data_json.value(), "startingPoint");
        startingPoint       = std::clamp(startingPoint, track_dat->firstLocation(), track_dat->lastLocation());
    } catch (...) {
        startingPoint = track_dat->firstLocation();
    }

    try {
        endPoint.value = Helper::getJsonField<double>(data_json.value(), "endPoint");
        endPoint       = std::clamp(endPoint, track_dat->firstLocation(), track_dat->lastLocation());
    } catch (...) {
        endPoint = track_dat->lastLocation();
    }

    if (startingPoint > endPoint) {
        throw std::runtime_error("the last location was smaller than the first position");
    }
}


void Track::parseJsonData() {

    if (!data_json.has_value()) {
        return;
    }

    if (!data_json->is_object()) {
        throw std::invalid_argument("the given data is not a json object");
    }

    try {
        auto str = Helper::getOptionalJsonField<std::string>(data_json.value(), "formatVersion");
        if (str.has_value()) {
            version ver = str.value();
            if (version::compare(format_version, ver) < 0) {
                throw std::runtime_error("libtrainsim format version not high enough.\nneeds at least:" + format_version.print() +
                                         " but got:" + ver.print());
            }
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("format version too old"));
    }

    try {
        name = Helper::getJsonField<std::string>(data_json.value(), "name");
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read name field"));
    }

    try {
        videoFile = parentPath / Helper::getJsonField<std::string>(data_json.value(), "videoFile");
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read video file field"));
    }

    try {
        auto dat = Helper::getJsonField(data_json.value(), "train");
        if (dat.is_string()) {
            std::filesystem::path tr = parentPath / dat.get<std::string>();
            train_dat                = train_properties(tr);
        } else if (dat.is_object()) {
            train_dat = train_properties(dat);
        } else {
            throw std::runtime_error("invalid train in track file");
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error constructing the train object"));
    }

    try {
        defaultTrackFrictionMultiplier = Helper::getOptionalJsonField<double>(data_json.value(), "defaultTrackFrictionMultiplier", 1.0);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading defaultTrackFrictionMultiplier"));
    }

    try {
        auto under = Helper::getOptionalJsonField(data_json.value(), "undergroundData");
        if (under.has_value()) {
            if (!under->is_array()) {
                throw std::runtime_error("Could not read undergroundData. Not an array");
            }

            for (const auto& _dat : under.value()) {
                auto sta = Helper::getJsonField<double>(_dat, "begin");
                auto en  = Helper::getJsonField<double>(_dat, "end");
                auto _ar = Helper::getOptionalJsonField<double>(_dat, "tunnelArea");

                auto                         start = sakurajin::unit_system::length{sta};
                auto                         end   = sakurajin::unit_system::length{en};
                sakurajin::unit_system::area area;
                if (_ar.has_value()) {
                    area = sakurajin::unit_system::area{_ar.value()};
                } else {
                    area = sakurajin::unit_system::square(3.5_m) * std::acos(0);
                }

                undergroundData.emplace_back(undergorundDataPoint{start, end, area});
            }
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading the underground data for track " + name));
    }

    try {
        auto stops = Helper::getOptionalJsonField(data_json.value(), "stops");
        if (stops.has_value()) {
            if (!stops->is_array()) {
                throw std::runtime_error("Could not read stops data. Not an array");
            }

            for (const auto& _dat : stops.value()) {

                auto _name = Helper::getJsonField<std::string>(_dat, "name");
                auto _loc  = Helper::getJsonField<double>(_dat, "location");
                auto _ty   = Helper::getJsonField<std::string>(_dat, "type");

                auto      _location = sakurajin::unit_system::length{_loc};
                stopTypes _type;
                if (_ty == "station") {
                    _type = station;
                } else {
                    throw std::runtime_error("Invalid stop type:" + _ty);
                }

                stopsData.emplace_back(stopDataPoint{_name, _location, _type});
            }
        }

    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading the stops data for track " + name));
    }

    try {
        auto excludeTrackBounds = Helper::getOptionalJsonField<bool>(data_json.value(), "excludeTrackBounds", true);
        if (!excludeTrackBounds || stopsData.size() < 2) {
            stopsData.reserve(stopsData.size() + 2);
            stopsData.insert(stopsData.begin(), {"begin", 0_m, station});
            stopsData.insert(stopsData.end(),
                             {
                                 "end", {std::numeric_limits<long double>::infinity(), 1},
                                  station
            });
        }

        stationsData.reserve(stopsData.size());
        for (auto& dat : stopsData) {
            if (dat.type() == station) {
                stationsData.emplace_back(dat);
            }
        }
        stationsData.shrink_to_fit();
    } catch (...) {
        std::throw_with_nested(std::runtime_error("error updating the stops with begin and end"));
    }
}

const Track_data& Track::data() const {
    if (stopsData.size() < 2) {
        throw std::runtime_error("stops data not fully initialized. There are not enugh stops defined");
    }
    if (!track_dat.has_value()) {
        throw std::runtime_error("Track not loaded yet");
    }
    return track_dat.value();
}

const train_properties& Track::train() const {
    return train_dat.value();
}

length Track::lastLocation() const {
    return endPoint;
}

length Track::firstLocation() const {
    return startingPoint;
}

const std::string& libtrainsim::core::Track::getName() const {
    return name;
}


std::filesystem::path Track::getVideoFilePath() const {
    return videoFile;
}

std::tuple<bool, sakurajin::unit_system::area, sakurajin::unit_system::length>
libtrainsim::core::Track::getUndergroundInfo(sakurajin::unit_system::length position) const {
    auto end = undergroundData.size();
    if (end == 0) {
        return {false, 0_m2, 0_m};
    }

    for (size_t i = 0; i < end; i++) {
        auto point = undergroundData[i];
        if (point.begin() > position && point.end() < position) {
            return {true, point.area(), point.end() - position};
        }
    }

    return {false, 0_m2, 0_m};
}

const std::vector<stopDataPoint>& libtrainsim::core::Track::getStations() const {
    if (!stationsData.empty()) {
        if (stationsData.size() < 2) {
            throw std::runtime_error("Not enough stations are defined");
        } else {
            return stationsData;
        }
    }

    if (stopsData.size() < 2) {
        throw std::runtime_error("stops data not fully initialized. There are not enugh stops defined");
    }

    return stationsData;
}


void libtrainsim::core::Track::ensure() {
    try {
        parseTrack();
        data_json.reset();
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error parsing the json data"));
    }
}

void libtrainsim::core::Track::setFirstLocation(sakurajin::unit_system::length pos) {
    try {
        ensure();
        startingPoint = sakurajin::unit_system::clamp(pos, track_dat->firstLocation(), track_dat->lastLocation());
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not update start position"));
    }
}

void libtrainsim::core::Track::setLastLocation(sakurajin::unit_system::length pos) {
    try {
        ensure();
        endPoint = sakurajin::unit_system::clamp(pos, track_dat->firstLocation(), track_dat->lastLocation());
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not update start position"));
    }
}
