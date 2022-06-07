#pragma once

#include "genericWindowManager.hpp"

#ifdef HAS_GLFW_SUPPORT
    #if  __has_include("glad/glad.h") && __has_include("GLFW/glfw3.h") && __has_include("glm/glm.hpp")
        #include <glad/glad.h>
        #include <GLFW/glfw3.h>
        #include <glm/glm.hpp>
        #include <glm/gtc/matrix_transform.hpp>
        #include <glm/gtc/type_ptr.hpp>
    #else
        #undef HAS_GLFW_SUPPORT
    #endif
#endif


namespace libtrainsim {
    namespace Video{
        #if defined(HAS_GLFW_SUPPORT)
            class glfw3WindowManager : public genericWindowManager{
            private:
                GLFWwindow* _window = nullptr;
                unsigned int _width = 1280;
                unsigned int _height = 720;
                
                
                unsigned int VBO = 0;
                unsigned int VAO = 0;
                unsigned int EBO = 0;
                unsigned int shaderProgram = 0;
                
                unsigned int texture = 0;
                
                unsigned int textureY = 0;
                unsigned int textureU = 0;
                unsigned int textureV = 0;
                
                const glm::vec2 coords[4] = {
                    glm::vec2{-1.0f,1.0f},
                    glm::vec2{1.0f,1.0f},
                    glm::vec2{1.0f,-1.0f},
                    glm::vec2{-1.0f,-1.0f}
                };
                
                const glm::vec2 texels[4] = {
                    glm::vec2{0.0f,0.0f},
                    glm::vec2{1.0f,0.0f},
                    glm::vec2{1.0f,1.0f},
                    glm::vec2{0.0f,1.0f}
                };
                
                const unsigned int indices[6]{
                    0,1,2,
                    0,2,3
                };
                
                std::shared_ptr<libtrainsim::Frame> pict;
            public:
                glfw3WindowManager(genericRenderer& _renderer);
                ~glfw3WindowManager();
                void createWindow(const std::string& windowName);
                void refreshWindow();
                
                GLFWwindow* getGLFWwindow();
            };
        #endif
    }
}

