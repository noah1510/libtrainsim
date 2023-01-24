#include "defaultShader.hpp"

//@TODO look into using GL_ARB_separate_shader_objects for shader pipelines
// this would allow reusing the vertex shader multiple times as separate program
// despite the fragment shader being different

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
