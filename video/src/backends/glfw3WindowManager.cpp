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
                                     "    TexCoord = aTexCoord;\n"
                                     "}\0";

static const char *fragmentShaderSource = "#version 330 core\n"
                                    "out vec4 FragColor;\n"
                                    "in vec2 TexCoord;\n"
                                    "uniform sampler2D Frame;\n"
                                    "void main()\n"
                                    "{\n"
                                    "    FragColor = texture(Frame, TexCoord);\n"
                                    "}\0";

libtrainsim::Video::glfw3WindowManager::glfw3WindowManager ( libtrainsim::Video::genericRenderer& _renderer ) : genericWindowManager{_renderer} {
    _width = renderer.getWidth();
    _height = renderer.getHight();
    pict = std::make_shared<libtrainsim::Frame>();
};

glfw3WindowManager::~glfw3WindowManager(){
    
    glfwTerminate();
}

void glfw3WindowManager::createWindow(const std::string& windowName){
    if(currentWindowName != "" || windowName == "" || renderer.reachedEndOfFile() || windowFullyCreated){
        return;
    }
    
    //create the empty window
    currentWindowName = windowName;
    std::cout << currentWindowName << std::endl;
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    
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
    
    
    /*screen = SDL_CreateWindow(
        currentWindowName.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        renderer.getWidth()/2,
        renderer.getHight()/2,
        SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE
    );

    if (!screen){
        std::cerr << "SDL: could not set video mode - exiting." << std::endl;
        return;
    }
    
    SDL_GL_SetSwapInterval(1);
    auto renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    #ifdef ENDABLE_VSYNC
    if(true){renderFlags |= SDL_RENDERER_PRESENTVSYNC;};
    #endif
    sdl_renderer = SDL_CreateRenderer(screen, -1, renderFlags);
    
    texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_YV12,
        SDL_TEXTUREACCESS_STREAMING,
        renderer.getWidth(),
        renderer.getHight()
    );*/
    
    renderer.initFrame(pict);
    
    lastFrame.reset();
    lastFrame = renderer.getNextFrame();
    
    windowFullyCreated = true;
}

void glfw3WindowManager::refreshWindow(){
    if(!windowFullyCreated){
        return;
    }
    
    pict.reset();
    pict = renderer.scaleFrame(lastFrame);

    glClear(GL_COLOR_BUFFER_BIT);
    
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
