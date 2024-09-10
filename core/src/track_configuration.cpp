#include "track_configuration.hpp"

using namespace libtrainsim::core;
using namespace SimpleGFX;
using namespace sakurajin::unit_system;

Track_data_point::Track_data_point(uint64_t              _frame,
                                   length                _location,
                                   double                _radius,
                                   double                _slope,
                                   std::optional<double> _frictionMultiplier)
    : Frame{_frame},
      Location{_location},
      Radius{_radius},
      Slope{_slope},
      FrictionMultiplier{_frictionMultiplier} {}


const length& libtrainsim::core::undergroundDataPoint::begin() const {
    return std::get<0>(*this);
}

const length& libtrainsim::core::undergroundDataPoint::end() const {
    return std::get<1>(*this);
}

const sakurajin::unit_system::area& libtrainsim::core::undergroundDataPoint::area() const {
    return std::get<2>(*this);
}

libtrainsim::core::undergroundDataPoint::undergroundDataPoint(sakurajin::unit_system::length _begin,
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

libtrainsim::core::stopDataPoint::stopDataPoint(const std::string&             _name,
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
        std::ifstream  in{URI};
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

    auto track_data = json::getJsonField(data_json.value(), "data");

    if (track_data.is_string()) {
        std::filesystem::path URI = parentPath / track_data.get<std::string>();
        if (!std::filesystem::exists(URI)) {
            throw std::invalid_argument("The Data file location is empty:" + URI.string());
        }

        if (URI.extension() != ".json") {
            throw std::invalid_argument("the file has no json extention");
        }

        nlohmann::json track_data_json;

        try {
            auto in = std::ifstream(URI);
            in >> track_data_json;
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error reading file into json structure"));
        }

        track_data = track_data_json;
    }

    if (!track_data.is_array()) {
        throw std::invalid_argument("json data is not an array");
    }

    if (track_data.empty()) {
        throw std::invalid_argument("The array is empty");
    }

    data.reserve(track_data.size());
    try {
        for (const auto& dat : track_data) {
            length location{json::getJsonField<double>(dat, "location")};
            auto   frame              = json::getJsonField<uint64_t>(dat, "frame");
            auto   slope              = json::getOptionalJsonField<double>(dat, "slope", 0);
            auto   radius             = json::getOptionalJsonField<double>(dat, "radius", std::numeric_limits<double>::infinity());
            auto   frictionMultiplier = json::getOptionalJsonField<double>(dat, "frictionMultiplier");

            libtrainsim::core::Track_data_point point{frame, location, radius, slope, frictionMultiplier};
            data.emplace_back(point);
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("error reading track data values"));
    }

    try {
        startingPoint.val() = json::getJsonField<double>(data_json.value(), "startingPoint");
        startingPoint       = std::clamp(startingPoint,  data.front().Location, data.back().Location);
    } catch (...) {
        startingPoint =  data.front().Location;
    }

    try {
        endPoint.val() = json::getJsonField<double>(data_json.value(), "endPoint");
        endPoint       = std::clamp(endPoint,  data.front().Location, data.back().Location);
    } catch (...) {
        endPoint = data.back().Location;
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
        auto str = json::getOptionalJsonField<std::string>(data_json.value(), "formatVersion");
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
        name = json::getJsonField<std::string>(data_json.value(), "name");
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read name field"));
    }

    try {
        videoFile = parentPath / json::getJsonField<std::string>(data_json.value(), "videoFile");
    } catch (...) {
        std::throw_with_nested(std::runtime_error("could not read video file field"));
    }

    try {
        auto dat = json::getJsonField(data_json.value(), "train");
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
        defaultTrackFrictionMultiplier = json::getOptionalJsonField<double>(data_json.value(), "defaultTrackFrictionMultiplier", 1.0);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading defaultTrackFrictionMultiplier"));
    }

    try {
        auto under = json::getOptionalJsonField(data_json.value(), "undergroundData");
        if (under.has_value()) {
            if (!under->is_array()) {
                throw std::runtime_error("Could not read undergroundData. Not an array");
            }

            for (const auto& _dat : under.value()) {
                auto sta = json::getJsonField<double>(_dat, "begin");
                auto en  = json::getJsonField<double>(_dat, "end");
                auto _ar = json::getOptionalJsonField<double>(_dat, "tunnelArea");

                auto                         start = sakurajin::unit_system::length{sta};
                auto                         end   = sakurajin::unit_system::length{en};
                sakurajin::unit_system::area area;
                if (_ar.has_value()) {
                    area = sakurajin::unit_system::area{_ar.value()};
                } else {
                    area = sakurajin::unit_system::square(3.5_m) * std::acos(0);
                }

                undergroundData.emplace_back(start, end, area);
            }
        }
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading the underground data for track " + name));
    }

    try {
        auto stops = json::getOptionalJsonField(data_json.value(), "stops");
        if (stops.has_value()) {
            if (!stops->is_array()) {
                throw std::runtime_error("Could not read stops data. Not an array");
            }

            for (const auto& _dat : stops.value()) {

                auto _name = json::getJsonField<std::string>(_dat, "name");
                auto _loc  = json::getJsonField<double>(_dat, "location");
                auto _ty   = json::getJsonField<std::string>(_dat, "type");

                auto      _location = sakurajin::unit_system::length{_loc};
                stopTypes _type;
                if (_ty == "station") {
                    _type = station;
                } else {
                    throw std::runtime_error("Invalid stop type:" + _ty);
                }

                stopsData.emplace_back(_name, _location, _type);
            }
        }

    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error reading the stops data for track " + name));
    }

    try {
        auto excludeTrackBounds = json::getOptionalJsonField<bool>(data_json.value(), "excludeTrackBounds", true);
        if (!excludeTrackBounds || stopsData.size() < 2) {
            stopsData.reserve(stopsData.size() + 2);
            stopsData.insert(stopsData.begin(), {"begin", 0_m, station});
            stopsData.insert(stopsData.end(),{
                 "end", sakurajin::unit_system::length{std::numeric_limits<long double>::infinity(), 1},
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

uint64_t Track::getFrame_c(length location) const {
    uint64_t index = data.size() / 2;
    uint64_t lower = 0;
    uint64_t upper = data.size();

    location = sakurajin::unit_system::unit_cast(location, 1);

    while (true) {
        auto loc = data[index].Location;

        // if it is an exact match return the current index
        if (loc == location) {
            return index;
        }

        // if the current location is larger adjust the upper bound, otherwise correct the lower bound.
        if (loc > location) {
            upper = index;
        } else {
            lower = index;
        }

        // get the next index
        index = (upper + lower) / 2;

        // if the algorithm cannot continue exit
        if (upper == index || lower == index) {
            break;
        }
    }

    return index;
}

const Track_data_point& Track::getDataPointAt(sakurajin::unit_system::length location) const {
    return data[getFrame_c(location)];
}

double Track::get_frictionMultiplier(sakurajin::unit_system::length location) const{
    const auto& point = getDataPointAt(location);
    if(point.FrictionMultiplier.has_value()) {
        return point.FrictionMultiplier.value();
    }

    return defaultTrackFrictionMultiplier;
}

uint64_t Track::getFrame(length location) const {
    return getDataPointAt(location).Frame;
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
        startingPoint = sakurajin::unit_system::clamp(pos, data.front().Location, data.back().Location);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not update start position"));
    }
}

void libtrainsim::core::Track::setLastLocation(sakurajin::unit_system::length pos) {
    try {
        ensure();
        endPoint = sakurajin::unit_system::clamp(pos, data.front().Location, data.back().Location);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Could not update start position"));
    }
}
