#pragma once

#include <string>

namespace libtrainsim{
    namespace Video{
        class defaultShaderSources{
        private:
            defaultShaderSources();
            static defaultShaderSources& getInstance(){
                static defaultShaderSources instance{};
                return instance;
            }
            
            std::string basicVertexSource;
            std::string displacementFragmentSource;
            std::string copyFragmentSource;
            std::string drawFragmentSource;
        public:
            static const std::string& getBasicVertexSource(){
                return getInstance().basicVertexSource;
            }
            
            static const std::string& getDisplacementFragmentSource(){
                return getInstance().displacementFragmentSource;
            }
            
            static const std::string& getCopyFragmentSource(){
                return getInstance().copyFragmentSource;
            }
            
            static const std::string& getDrawFragmentSource(){
                return getInstance().drawFragmentSource;
            }
            
        };
    }
}
