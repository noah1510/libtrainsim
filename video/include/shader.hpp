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
        
        /**
         * @brief a small class to handle the configuration of the shader parts
         * 
         * @details For more information on what a shader and its parts do
         * look into the [opengl documentation](https://www.khronos.org/opengl/wiki/Shader).
         * 
         * This class allows loading shaders from the filesystem or directly from strings.
         * 
         */
        class Shader_configuration{
          private:
            /**
             * @brief the source of the vertex shader
             */
            std::string vertex_shader_source = "";
            
            /**
             * @brief the source of the fragment shader
             */
            std::string fragment_shader_source = "";
            
            /**
             * @brief the source of the tessalation control shader
             */
            std::optional<std::string> tessControl_shader_source = "";
            
            /**
             * @brief the source of the tessalation Evaluation shader
             */
            std::optional<std::string> tessEvaluation_shader_source = "";
            
            /**
             * @brief the source of the geometry shader
             */
            std::optional<std::string> geometry_shader_source = "";
            
            /**
             * @brief the source of the compute shader
             */
            std::optional<std::string> compute_shader_source = "";
            
            /**
             * @brief true if all of the sources are cleared
             */
            bool isCleared = true;
            
            //get rid of the default constructor;
            Shader_configuration() = delete;
            
          public:
            
            Shader_configuration(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc);
            Shader_configuration(
                const std::filesystem::path& vertLoc,
                const std::filesystem::path& fragLoc,
                std::optional<std::filesystem::path> tessControlLoc,
                std::optional<std::filesystem::path> tessEvaluationLoc,
                std::optional<std::filesystem::path> GeometryLoc,
                std::optional<std::filesystem::path> ComputeLoc
            );
            
            Shader_configuration(const std::string& vertSrc, const std::string& fragSrc);
            Shader_configuration(
                const std::string& vertSrc,
                const std::string& fragSrc,
                std::optional<std::string> tessControlSrc,
                std::optional<std::string> tessEvaluationSrc,
                std::optional<std::string> GeometrySrc,
                std::optional<std::string> ComputeSrc
            );
            
            Shader_configuration(const Shader_configuration& other);
            
            /**
             * @brief clear the source strings to free memory
             * @note this will cause the get functions to throw an exception
             * if they are called after this function
             */
            void clear();
            
            /**
             * @brief true if the config is constructed and not cleared yet
             */
            bool isValid() const;
            
            std::string getVertexSource();
            std::string getFragmentSource();
            std::optional<std::string> getTessControlSource();
            std::optional<std::string> getTessEvaluationSource();
            std::optional<std::string> getGeometrySource();
            std::optional<std::string> getComputeSource();
            
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
