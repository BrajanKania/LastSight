#version 460 core

out vec4 fragColor;

in vec2 texCoords;
in vec2 fragWorldPos;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main() {
  vec4 texColor = texture(uTexture, texCoords) * uColor;

  if (texColor.a < 0.01f) {
    discard;
  }

  fragColor = texColor;
}
