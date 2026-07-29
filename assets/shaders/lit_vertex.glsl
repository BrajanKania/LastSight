#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat4 uModel;
uniform mat4 uViewProjection;

void main(){
  gl_Position = uViewProjection * uModel * vec4(aPos.xy, 0.f, 1.f);
}
