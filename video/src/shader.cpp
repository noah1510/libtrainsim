#include "shader.hpp"

libtrainsim::Video::Shader_configuration::Shader_configuration(const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc):Shader_configuration{vertLoc,fragLoc,{},{},{},{}}{}
    
libtrainsim::Video::Shader_configuration::Shader_configuration ( const libtrainsim::Video::Shader_configuration& other ) {
    if(!other.isValid()){
        throw std::invalid_argument("Cannot create shader config from invalid config");
    }
    
    vertex_shader_source = other.vertex_shader_source;
    fragment_shader_source = other.fragment_shader_source;
    tessControl_shader_source = other.tessControl_shader_source;
    tessEvaluation_shader_source = other.tessEvaluation_shader_source;
    geometry_shader_source = other.geometry_shader_source;
    compute_shader_source = other.compute_shader_source;
    isCleared = false;
}


libtrainsim::Video::Shader_configuration::Shader_configuration(
    const std::filesystem::path& vertLoc,
    const std::filesystem::path& fragLoc,
    std::optional<std::filesystem::path> tessControlLoc,
    std::optional<std::filesystem::path> tessEvaluationLoc,
    std::optional<std::filesystem::path> GeometryLoc,
    std::optional<std::filesystem::path> ComputeLoc
){
    try{
        vertex_shader_source = libtrainsim::core::Helper::loadFile(vertLoc);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Cannot load vertex shader"));
    }
    
    try{
        fragment_shader_source = libtrainsim::core::Helper::loadFile(fragLoc);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Cannot load fragment shader"));
    }
    
    if(tessControlLoc.has_value()){
        try{
            tessControl_shader_source = libtrainsim::core::Helper::loadFile(tessControlLoc.value());
        }catch(...){
            std::throw_with_nested(std::invalid_argument("Cannot load tessalation control shader"));
        }
    }else{
        tessControl_shader_source = {};
    }
    
    if(tessEvaluationLoc.has_value()){
        try{
            tessEvaluationLoc = libtrainsim::core::Helper::loadFile(tessEvaluationLoc.value());
        }catch(...){
            std::throw_with_nested(std::invalid_argument("Cannot load tessalation evaluation shader"));
        }
    }else{
        tessEvaluation_shader_source = {};
    }
    
    if(GeometryLoc.has_value()){
        try{
            geometry_shader_source = libtrainsim::core::Helper::loadFile(GeometryLoc.value());
        }catch(...){
            std::throw_with_nested(std::invalid_argument("Cannot load geomerty shader"));
        }
    }else{
        geometry_shader_source = {};
    }
    
    if(ComputeLoc.has_value()){
        try{
            compute_shader_source = libtrainsim::core::Helper::loadFile(ComputeLoc.value());
        }catch(...){
            std::throw_with_nested(std::invalid_argument("Cannot load compute shader"));
        }
    }else{
        compute_shader_source = {};
    }
    
    isCleared = false;
}

libtrainsim::Video::Shader_configuration::Shader_configuration(const std::string& vertSrc, const std::string& fragSrc ):Shader_configuration{vertSrc,fragSrc,{},{},{},{}} {}

libtrainsim::Video::Shader_configuration::Shader_configuration (
    const std::string& vertSrc,
    const std::string& fragSrc,
    std::optional<std::string> tessControlSrc,
    std::optional<std::string> tessEvaluationSrc,
    std::optional<std::string> GeometrySrc,
    std::optional<std::string> ComputeSrc
) :
vertex_shader_source{vertSrc},
fragment_shader_source{fragSrc},
tessControl_shader_source{tessControlSrc},
tessEvaluation_shader_source{tessEvaluationSrc},
geometry_shader_source{GeometrySrc},
compute_shader_source{ComputeSrc}{}


std::string libtrainsim::Video::Shader_configuration::getFragmentSource() {
    return fragment_shader_source;
}

std::string libtrainsim::Video::Shader_configuration::getVertexSource() {
    return vertex_shader_source;
}

std::optional<std::string> libtrainsim::Video::Shader_configuration::getTessEvaluationSource() {
    return tessEvaluation_shader_source;
}

std::optional<std::string> libtrainsim::Video::Shader_configuration::getTessControlSource() {
    return tessControl_shader_source;
}

std::optional<std::string> libtrainsim::Video::Shader_configuration::getGeometrySource() {
    return geometry_shader_source;
}

std::optional<std::string> libtrainsim::Video::Shader_configuration::getComputeSource() {
    return compute_shader_source;
}


void libtrainsim::Video::Shader_configuration::clear(){
    vertex_shader_source.clear();
    fragment_shader_source.clear();
    tessControl_shader_source = {};
    tessEvaluation_shader_source = {};
    geometry_shader_source = {};
    compute_shader_source = {};
    isCleared = true;
}
    
bool libtrainsim::Video::Shader_configuration::isValid() const {
    return !isCleared;
}

    
libtrainsim::Video::Shader::Shader ( const libtrainsim::Video::Shader_configuration& config ) : shader_config{config} {
    if(!shader_config.isValid()){
        throw std::invalid_argument("Shder configuration is not valid!");
    }
    
    try{
        createShader();
    }catch(...){
        std::throw_with_nested( std::runtime_error("could not create Shader") );
    }
    
}

libtrainsim::Video::Shader::Shader ( libtrainsim::Video::Shader_configuration config ) : shader_config{config} {
    if(!shader_config.isValid()){
        throw std::invalid_argument("Shder configuration is not valid!");
    }
    
    try{
        createShader();
    }catch(...){
        std::throw_with_nested( std::runtime_error("could not create Shader") );
    }
}

libtrainsim::Video::Shader::Shader ( const std::filesystem::path& vertLoc, const std::filesystem::path& fragLoc ) : shader_config{vertLoc, fragLoc} {
    if(!shader_config.isValid()){
        throw std::invalid_argument("Shder configuration is not valid!");
    }
    
    try{
        createShader();
    }catch(...){
        std::throw_with_nested( std::runtime_error("could not create Shader") );
    }
}


libtrainsim::Video::Shader::~Shader() {
    glDeleteProgram(shaderProgram);
}


void libtrainsim::Video::Shader::use() {
    glUseProgram(shaderProgram);
    try{
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Cannot use shader program"));
    }
}

int libtrainsim::Video::Shader::getLocationIndex ( const std::string& location ) {
    auto loc = glGetUniformLocation(shaderProgram, location.c_str() );
    
    try{
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("Error while getting location in shader"));
    }
    
    if(loc < 0){
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        
        std::stringstream ss;
        ss << "cannot get the requested location: " << infoLog;
        throw std::invalid_argument{ss.str()};
    }
    
    return loc;
}



void libtrainsim::Video::Shader::setUniform ( const std::string& location, int value ) {
    try{
        auto dat = std::vector<int>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform int"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, size_t value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform1ui(loc, value);
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform size_t"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, float value ) {
    try{
        auto dat = std::vector<float>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform float"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, glm::vec1 value ) {
    try{
        auto dat = std::vector<glm::vec1>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec1"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, glm::vec2 value ) {
    try{
        auto dat = std::vector<glm::vec2>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec2"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, glm::vec3 value ) {
    try{
        auto dat = std::vector<glm::vec3>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec3"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, glm::vec4 value ) {
    try{
        auto dat = std::vector<glm::vec4>{value};
        setUniform(location, dat);
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec4"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const glm::mat4& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform mat4"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<int>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform1iv(loc, value.size(), value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform int array"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<float>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform1fv(loc, value.size(), value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform float array"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<glm::vec1>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform1fv(loc, value.size(), (float*) value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec1 array"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<glm::vec2>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform2fv(loc, value.size(), (float*) value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec2 array"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<glm::vec3>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform3fv(loc, value.size(), (float*) value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec3 array"));
    }
}

void libtrainsim::Video::Shader::setUniform ( const std::string& location, const std::vector<glm::vec4>& value ) {
    try{
        auto loc = getLocationIndex(location);
        glUniform4fv(loc, value.size(), (float*) value.data());
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::invalid_argument("Error setting uniform vec4 array"));
    }
}


void libtrainsim::Video::Shader::createShader () {
    if(!shader_config.isValid()){
        throw std::invalid_argument{"shader config is not valid"};
    }
    
    //compile and load the vertex shader
    unsigned int vertexShader = 0;
    try{
        compileShader(shader_config.getVertexSource(), vertexShader, GL_VERTEX_SHADER);
    }catch(...){
        glDeleteShader(vertexShader);
        std::throw_with_nested(std::runtime_error("Could not compile or load vertex shader"));
    }
    
    //compile and load the fragment shader
    unsigned int fragmentShader = 0;
    try{
        compileShader(shader_config.getFragmentSource(), fragmentShader, GL_FRAGMENT_SHADER);
    }catch(...){
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        std::throw_with_nested(std::runtime_error("Could not compile or load fragment shader"));
    }
    
    //optional shader parts
    unsigned int geometryShader = 0, computeShader = 0, tessControlShader = 0, tessEvalShader = 0;
    
    //compile and load the geomerty shader
    if(shader_config.getGeometrySource().has_value()){
        try{
            compileShader(shader_config.getGeometrySource().value(), geometryShader, GL_GEOMETRY_SHADER);
        }catch(...){
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteShader(geometryShader);
            std::throw_with_nested(std::runtime_error("Could not compile or load geomerty shader"));
        }
    }
    
    //compile and load the compute shader
    if(shader_config.getComputeSource().has_value()){
        try{
            compileShader(shader_config.getComputeSource().value(), computeShader, GL_COMPUTE_SHADER);
        }catch(...){
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteShader(geometryShader);
            glDeleteShader(computeShader);
            std::throw_with_nested(std::runtime_error("Could not compile or load compute shader"));
        }
    }
    
    //compile and load the tessalation control shader
    if(shader_config.getTessControlSource().has_value()){
        try{
            compileShader(shader_config.getTessControlSource().value(), tessControlShader, GL_TESS_CONTROL_SHADER);
        }catch(...){
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteShader(geometryShader);
            glDeleteShader(computeShader);
            glDeleteShader(tessControlShader);
            std::throw_with_nested(std::runtime_error("Could not compile or load tessalation control shader"));
        }
    }
    
    //compile and load the tessalation evalutaion shader
    if(shader_config.getTessEvaluationSource().has_value()){
        try{
            compileShader(shader_config.getTessEvaluationSource().value(), tessEvalShader, GL_TESS_EVALUATION_SHADER);
        }catch(...){
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            glDeleteShader(geometryShader);
            glDeleteShader(computeShader);
            glDeleteShader(tessControlShader);
            glDeleteShader(tessEvalShader);
            std::throw_with_nested(std::runtime_error("Could not compile or load tessalation evalutaion shader"));
        }
    }
    
    //clear the config to empty the strings in ram
    shader_config.clear();
    
    //prepare the shader program to link every part
    int success;
    
    if(shaderProgram){
        glDeleteProgram(shaderProgram);
    }
    shaderProgram = glCreateProgram();
    
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    if(geometryShader){glAttachShader(shaderProgram, geometryShader);};
    if(computeShader){glAttachShader(shaderProgram, computeShader);};
    if(tessControlShader){glAttachShader(shaderProgram, tessControlShader);};
    if(tessControlShader){glAttachShader(shaderProgram, tessControlShader);};
    
    //link the individual shaders to a shader program
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success != GL_TRUE) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(geometryShader);
        glDeleteShader(computeShader);
        glDeleteShader(tessControlShader);
        glDeleteShader(tessEvalShader);
        
        shaderProgram = 0;
        throw std::runtime_error(std::string{"could not link shader program: "}.append(infoLog));
    }
    
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);
    glDeleteShader(computeShader);
    glDeleteShader(tessControlShader);
    glDeleteShader(tessEvalShader);
}

void libtrainsim::Video::Shader::compileShader( std::string code, unsigned int& shaderLoc, int shaderType ) {
    // compile the shader and return the location of the result
    int success;

    auto shaderCode = code.c_str();
    int shader = 0;
    try{
        shader = glCreateShader(shaderType);
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot create shader"));
    }
    
    try{
        glShaderSource(shader, 1, &shaderCode, NULL);
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot upload shader source shader"));
    }
    
    try{
        glCompileShader(shader);
        imguiHandler::glErrorCheck();
    }catch(...){
        std::throw_with_nested(std::runtime_error("cannot compile shader"));
    }

    // print compile errors if anything went wrong
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE) {
        //get the error description
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        
        //create the full error message
        std::stringstream ss;
        ss << "could not compile shader: " << shaderCode << ";reason: " << infoLog;
        throw std::runtime_error(ss.str());
    };

    shaderLoc = shader;
}

