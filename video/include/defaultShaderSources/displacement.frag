R""""(
#version 330 core

layout(location = 0) out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D img;
uniform sampler2D displacement;

uniform float multiplier = 1.0;
const float zeroOffset = 127.0/255.0;

void main(){
    vec2 delta = texture(displacement, TexCoord).rg - vec2(zeroOffset, zeroOffset);
    
    vec2 coords = TexCoord + delta * multiplier;
    
    FragColor = texture(img, coords);
}

)"""";
