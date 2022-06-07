#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "backends/glfw3WindowManager.hpp"

#if defined(HAS_SDL_SUPPORT)

using namespace libtrainsim;
using namespace libtrainsim::Video;

static const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec2 aPos;\n"
                                     "layout (location = 1) in vec2 aTexCoord;\n"
                                     "out vec2 TexCoord;\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_Position = vec4(aPos.xy, 0.0, 1.0);\n"
                                     "    TexCoord = gl_Position.xy;\n"
                                     "}\0";

static const char *fragmentShaderSource = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "uniform sampler2D Frame;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    //FragColor = vec4(TexCoord.x, TexCoord.y, 1.0, 1.0);\n"
                                    "    FragColor = vec4(1.0, 0.0, 1.0, 1.0);\n"
                                    "}\0";

libtrainsim::Video::glfw3WindowManager::glfw3WindowManager ( libtrainsim::Video::genericRenderer& _renderer ) : genericWindowManager{_renderer} {
    _width = renderer.getWidth();
    _height = renderer.getHight();
    pict = std::make_shared<libtrainsim::Frame>();
};

glfw3WindowManager::~glfw3WindowManager(){
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
}

void glfw3WindowManager::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || renderer.reachedEndOfFile() || windowFullyCreated){
        return;
    }
    
    //create the empty window
    currentWindowName = windowName;
    std::cout << currentWindowName << std::endl;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    if(_width == 0){_width = 1280;};
    if(_height == 0){_height = 720;};
    
    _window = glfwCreateWindow(_width, _height, currentWindowName.c_str(), NULL, NULL);
    if (_window == nullptr){
        std::cerr << "Failed to create GLFW window" << std::endl;
        const char* desc;
        glfwGetError(&desc);
        std::cerr << desc << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(_window);
    
    //initiate glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }
    
    //generate Video Buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //create the rectangle data
    float vertices[16];
    for(unsigned int i = 0; i < 4;i++){
        vertices[4*i] = coords[i].x;
        vertices[4*i+1] = coords[i].y;
        vertices[4*i+2] = texels[i].x;
        vertices[4*i+3] = texels[i].y;
    }
    
    //send the data to the gpu
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texel attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //compile and link the shaders
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int  success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return;
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &success);
    if(!success){
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
        return;
    }

    glUseProgram(shaderProgram);
    
    //enable texture filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //enable mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //generate the opengl texture
    glGenTextures(1, &texture);
    //glBindTexture(GL_TEXTURE_2D, texture);
    
    renderer.initFrame(pict);
    
    lastFrame.reset();
    lastFrame = renderer.getNextFrame();
    
    
    windowFullyCreated = true;
}

void glfw3WindowManager::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    int _width = 0;
    int _height = 0;
    glfwGetFramebufferSize(_window, &_width, &_height);
    glViewport(0, 0, _width, _height);
    
    //clear the opengl buffer
    glClearColor(0.2f,0.3f,0.3f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //get the new frame
    pict.reset();
    pict = renderer.scaleFrame(lastFrame);
    
    //load the frame to the gpu
    //TODO convert the image to rgb
    //glBindTexture(GL_TEXTURE_2D, texture);
    
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pict->dataFF()->width, pict->dataFF()->height, 0, GL_RED, GL_UNSIGNED_BYTE, pict->dataFF()->data[0]);
    //glGenerateMipmap(GL_TEXTURE_2D);
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    
    glUseProgram(shaderProgram);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(_window);
    glfwPollEvents();  
    /*SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = renderer.getWidth();
    rect.h = renderer.getHight();
    
    SDL_UpdateYUVTexture(
        texture,
        &rect,
        pict->dataFF()->data[0],
        pict->dataFF()->linesize[0],
        pict->dataFF()->data[1],
        pict->dataFF()->linesize[1],
        pict->dataFF()->data[2],
        pict->dataFF()->linesize[2]
    );

    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer,texture,NULL,NULL);
    SDL_RenderPresent(sdl_renderer);*/
}

GLFWwindow* glfw3WindowManager::getGLFWwindow(){
    return _window;
}

#endif
