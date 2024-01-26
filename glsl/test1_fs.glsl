#version 330 core
out vec4 FragColor;
  
in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform float ourMix;

void main() {
  FragColor = mix(texture(ourTexture, TexCoord*2), texture(ourTexture2, TexCoord*2), ourMix);
}
