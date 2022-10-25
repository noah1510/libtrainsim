#pragma once

#include "imguiHandler.hpp"

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iostream>

namespace libtrainsim{
    namespace Video{
        class Shader_configuration{
        public:
            std::filesystem::path vertex_shader_location = "";
            std::filesystem::path tessControl_shader_location = "";
            std::filesystem::path tessEvaluation_shader_location = "";
            std::filesystem::path geometry_shader_location = "";
            std::filesystem::path fragment_shader_location = "";
            std::filesystem::path compute_shader_location = "";
            
            Shader_configuration(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc);
            Shader_configuration(const Shader_configuration& other);
            Shader_configuration();
            
            bool isValid() const;
            
            bool hasTessControlShader() const;
            bool hasTessEvaluationShader() const;
            bool hasGeometryShader() const;
            bool hasComputeShader() const;
            
        };
        
        class Shader{
        private:
            unsigned int shaderProgram = 0;
            
            Shader_configuration shader_config;
            
            int createShader();
            
            int compileShader(std::string code, unsigned int& shaderLoc, int shaderType);
        public:
            Shader(const Shader_configuration& config);
            Shader(Shader_configuration config);
            
            Shader(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc);
            
            ~Shader();
            
            void use();
            
            void setUniform(const std::string& location, int value);
            void setUniform(const std::string& location, size_t value);
            void setUniform(const std::string& location, float value);
            void setUniform(const std::string& location, glm::vec1 value);
            void setUniform(const std::string& location, glm::vec2 value);
            void setUniform(const std::string& location, glm::vec3 value);
            void setUniform(const std::string& location, glm::vec4 value);
            void setUniform(const std::string& location, const glm::mat4& value);
            
            void setUniform(const std::string& location, const std::vector<int>& value);
            void setUniform(const std::string& location, const std::vector<float>& value);
            void setUniform(const std::string& location, const std::vector<glm::vec1>& value);
            void setUniform(const std::string& location, const std::vector<glm::vec2>& value);
            void setUniform(const std::string& location, const std::vector<glm::vec3>& value);
            void setUniform(const std::string& location, const std::vector<glm::vec4>& value);
            
        };
    }
}
