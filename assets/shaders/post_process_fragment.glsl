#version 460 core

in vec2 texCoords;

uniform sampler2D uTexture;
uniform float uTime;

out vec4 fragColor;

const float grainAmount = 0.08f;

float random(vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

void main(){
  vec4 screenColor = texture(uTexture,texCoords);

  float noise = random(texCoords + fract(uTime));
  float grain = (noise - 0.5f) * grainAmount;
  vec3 color = screenColor.rgb + grain;
  fragColor = vec4(color.rgb, screenColor.a);
}
