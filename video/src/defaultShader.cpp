#include "defaultShader.hpp"

libtrainsim::Video::defaultShaderSources::defaultShaderSources(){
    //these strings are assigned by using #include on the actual shader source files
    //The shader soure file have to start with `R""""(` and end with `)"""";`.
    //This way they can be copy pasted as multiline C++ strings.
    
    basicVertexSource = 
        #include "defaultShaderSources/basic.vert"
    
    displacementFragmentSource =
        #include "defaultShaderSources/displacement.frag"
        
    copyFragmentSource = 
        #include "defaultShaderSources/copy.frag"
    
    drawFragmentSource = 
        #include "defaultShaderSources/draw.frag"
    
}
