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
         * It is required that there are a vertex shader and fragment shader. All other parts
         * are opional.
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
            /**
             * @brief create a shader config by loading a vertex and fragment shader from a file
             * 
             * @throws std::nested_exception when a given argument cannot be loaded from the file system
             */
            Shader_configuration(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc);
            
            /**
             * @brief create a shader by loading as many parts as wanted from the file system
             * 
             * You can pass {} for all of the shader parts you do not want to use.
             * If you pass a value it has to be able to be loaded or an error will be thorwn
             * 
             * @throws std::nested_exception when a given argument cannot be loaded from the file system
             */
            Shader_configuration(
                const std::filesystem::path& vertLoc,
                const std::filesystem::path& fragLoc,
                std::optional<std::filesystem::path> tessControlLoc,
                std::optional<std::filesystem::path> tessEvaluationLoc,
                std::optional<std::filesystem::path> GeometryLoc,
                std::optional<std::filesystem::path> ComputeLoc
            );
            
            /**
             * @brief create a shader config by giving the source code directly
             * @note this does not load a file it expects the shader source as the parameters
             */
            Shader_configuration(const std::string& vertSrc, const std::string& fragSrc);
            
            /**
             * @brief create a shader config by giving the source code directly
             * @note this does not load a file it expects the shader source as the parameters
             */
            Shader_configuration(
                const std::string& vertSrc,
                const std::string& fragSrc,
                std::optional<std::string> tessControlSrc,
                std::optional<std::string> tessEvaluationSrc,
                std::optional<std::string> GeometrySrc,
                std::optional<std::string> ComputeSrc
            );
            
            /**
             * @brief create a shader chonfig from another valid config
             * 
             * @throws std::invalid_argument if the given config is not valid
             */
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
            
            /**
             * @brief get the source code for the vertex shader
             */
            std::string getVertexSource();
            
            /**
             * @brief get the source code for the fragment shader
             */
            std::string getFragmentSource();
            
            /**
             * @brief get the source code for the tessalation control shader
             */
            std::optional<std::string> getTessControlSource();
            
            /**
             * @brief get the source code for the tessalation evaluation shader
             */
            std::optional<std::string> getTessEvaluationSource();
            
            /**
             * @brief get the source code for the geometry shader
             */
            std::optional<std::string> getGeometrySource();
            
            /**
             * @brief get the source code for the compute shader
             */
            std::optional<std::string> getComputeSource();
            
        };
        
        /**
         * @brief a class to abstract opengl shaders
         * 
         */
        class Shader{
        private:
            /**
             * @brief The openGL shader Program ID
             */
            unsigned int shaderProgram = 0;
            
            /**
             * @brief the shader configuration used for this shader
             * @note after calling createShader() this should be invalid since
             * keeping the shader source in ram is a waste of resources
             */
            Shader_configuration shader_config;
            
            /**
             * @brief create the shader program
             * 
             * This function uses the shader_config to compile each of the shader parts and
             * link them together to a single shader program. If anything goes wrong an
             * exception is thrown and the function cleans up the parts.
             * 
             * @throws std::invalid_argument if the shader fonfig is not valid
             * @throws std::nested_exception if any of the shader parts failed to compile
             * @throws std::runtime_error if the shader fails to link
             */
            void createShader();
            
            /**
             * @brief compiles a shader part
             * 
             * @details Compiles a shader parts into a given location
             *          This function needs the shader source code, where the compiled part
             *          is stored and which type this shader part is.
             * 
             * @param code The shader code that should be compiled
             * @param shaderLoc The location where the shader part is supposed to be stored
             * @param shaderType The type of this shader. The valid types are defined in the
             * [opengl doc](https://registry.khronos.org/OpenGL-Refpages/gl4/html/glCreateShader.xhtml).
             * 
             * @throws std::runtime_error if there was an error while compiling the code
             */
            void compileShader(std::string code, unsigned int& shaderLoc, int shaderType);
        public:
            /**
             * @brief creates a shader from a valid shader config
             * 
             * @throws std::invalid_argument if the config is invalid
             * @throws std::nested_exception when there was an error compiling the shader
             */
            Shader(const Shader_configuration& config);
            
            /**
             * @brief create a shader from the filesystem
             * 
             * @throws std::nested_exception when there was an error reading the files
             * @throws std::nested_exception when there was an error compiling the shader
             */
            Shader(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc);
            
            /**
             * @brief destroys the shader and deletes the program on the gpu
             */
            ~Shader();
            
            /**
             * @brief use the shader
             * 
             * @details This functions activates the shader.
             *          This allows the uniform variables to be modified
             *          and this shader to be used for the next draw call.
             *          This is basically a simple abstraction over glUseProgram.
             * 
             * @throws std::nested_exception if there was an openGL error
             */
            void use();
            
            /**
             * @brief get the location index of a uniform value in this shader
             * 
             * This function retrieves the position of a uniform variable in this shader.
             * It checks for errors with the function call and throws an exception
             * acordingly.
             * 
             * @throws std::invalid_argument if the location cannot be loaded
             * 
             * @param location the variable name of the wanted uniform variable
             * @return the location for use with glUniform
             */
            int getLocationIndex(const std::string& location);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, int value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, size_t value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, float value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, glm::vec1 value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, glm::vec2 value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, glm::vec3 value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, glm::vec4 value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const glm::mat4& value);
            
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<int>& value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<float>& value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<glm::vec1>& value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<glm::vec2>& value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<glm::vec3>& value);
            
            /**
             * @brief set a uniform variable
             * 
             * @throws std::nested_exception if the location cannot be found or used
             * @throws std::nested_exception if the value cannot be set to that location
             * 
             */
            void setUniform(const std::string& location, const std::vector<glm::vec4>& value);
            
        };
    }
}
