#version 460 core

layout (location = 0) in vec2 aPos;

uniform mat4 uModel;
uniform mat4 uViewProjection;
uniform vec2 uUvScale;

out vec2 texCoords;
out vec2 fragWorldPos;

void main(){
  vec4 worldPos = uModel * vec4(aPos.xy - 0.5f, 0.f, 1.f);
  gl_Position = uViewProjection * worldPos;
  texCoords = aPos * uUvScale;
  fragWorldPos = worldPos.xy;
}
