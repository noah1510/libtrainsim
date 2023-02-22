R""""(
#version 330 core
precision highp float;

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 color;

void main(){
    FragColor = color;
}

)"""";
