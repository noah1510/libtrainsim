R""""(
#version 410 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec2 Coord;

uniform mat4 transform;

void main(){
    gl_Position = transform * vec4(aPos,0.0 , 1.0);
    TexCoord = aTexCoord;
    Coord = aPos;
}

)"""";
