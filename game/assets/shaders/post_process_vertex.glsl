#version 460 core

layout (location = 0) in vec2 aPos;

out vec2 texCoords;

void main(){
  gl_Position = vec4(aPos.xy * 2.f - 1.f, 0.f, 1.f);
  texCoords = aPos;
}

