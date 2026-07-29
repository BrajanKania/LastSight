#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main(){
  vec4 texColor = texture(uTexture, texCoords);
  fragColor = texColor * uColor;
}
