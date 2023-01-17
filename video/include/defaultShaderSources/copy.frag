R""""(
#version 330 core
precision highp float;

layout(location = 0) out vec4 FragColor;

in vec2 CoordSrc;

uniform sampler2D sourceImage;

void main(){
    FragColor = texture(sourceImage, CoordSrc);
}

)"""";
