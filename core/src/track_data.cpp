#include "track_data.hpp"
#include <cmath>
#include <iostream>
#include <fstream>

using namespace libtrainsim::core;

Track_data::Track_data(const std::string& URI):libtrainsim::core::Track_data::Track_data (std::filesystem::path(URI)) {};
Track_data::Track_data(const char* URI):libtrainsim::core::Track_data::Track_data (std::filesystem::path(URI)) {};

Track_data::Track_data(const std::filesystem::path& URI){
    if(URI.empty()){
        std::cerr << "The file location is empty" << std::endl;
        return;
    }

    if (URI.extension() != ".json" ){
        std::cerr << "the file has no json extention" << std::endl;
        return;
    }

    auto in = std::ifstream(URI);

    in >> data_json;
    
    if(data_json.size() == 0){
        std::cerr << "The array size is 0" << std::endl;
        return;
    }

    m_isValid = true;
    return;
}

Track_data::Track_data(const json& data){
    if(data.size() == 0){
        return;
    }
    
    data_json = data;
    
    m_isValid = true;
    return;
}

Track_data::~Track_data(){
    data_json.clear();
}

int64_t Track_data::getFrame_c(double location, int64_t index, int64_t lower, int64_t upper) const{
    if(!isValid() || lower < 0 || lower > getSize() || upper < lower || upper > getSize() || index < lower || index > upper){return 0;};

    while(true){
        double loc = data_json.at(index)["location"];

        //if it is an exact match return the current index
        if (loc == location){
            return index;
        }

        //if the current location is larger adjust the upper bound, otherwise correct the lower bound.
        if(loc > location){
            upper = index;
        }else{
            lower = index;
        }

        //get the next index
        index = (upper+lower)/2;

        //if the algorithm cannot continue exit
        if(upper == index || lower == index){
            break;
        }

    }

    return index;

}

int64_t Track_data::getFrame(double location){
    if(!isValid()){return 0;};

    return last_frame_index = getFrame_c(location,last_frame_index,0,getSize());
    
}

int64_t Track_data::getFrame(double location) const{
    if(!isValid()){return 0;};

    return getFrame_c(location,getSize()/2,0,getSize());
    
}

std::string Track_data::dump(int ident) const{
    if(!isValid()){return "";};
    return data_json.dump(ident);
}

            
int64_t Track_data::getSize() const{
    if(!isValid()){return 0;};
    return data_json.size();
}

bool Track_data::isValid() const{
    return m_isValid;
}
