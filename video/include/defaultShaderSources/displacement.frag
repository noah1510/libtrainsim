R""""(
#version 410 core
precision highp float;

layout(location = 0) out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D img;
uniform sampler2D displacement;

uniform float multiplier = 1.0;
const float zeroOffset = 127.0/255.0;

void main(){
    vec2 delta = texture(displacement, texCoord).rg - vec2(zeroOffset, zeroOffset);
    
    vec2 coords = texCoord + delta * multiplier;
    
    FragColor = texture(img, coords);
}

)"""";
