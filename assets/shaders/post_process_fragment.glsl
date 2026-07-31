#version 460 core

in vec2 texCoords;

uniform sampler2D uTexture;

out vec4 fragColor;

const float offset = 1.f / 1200.f;

void main(){
  vec2 offsets[9] = vec2[](
    vec2(-offset, offset), // top-left
    vec2( 0.0f, offset), // top-center
    vec2( offset, offset), // top-right
    vec2(-offset, 0.0f), // center-left
    vec2( 0.0f, 0.0f), // center-center
    vec2( offset, 0.0f), // center-right
    vec2(-offset, -offset), // bottom-left
    vec2( 0.0f, -offset), // bottom-center
    vec2( offset, -offset) // bottom-right
  );

  float p = 16.f;
  float blurKernel[9] = float[](
      1.f / p, 2.f / p, 1.f / p,
      2.f / p, 4.f / p, 2.f / p,
      1.f / p, 2.f / p, 1.f / p
  );

  vec3 sampleTex[9];
  for(int i = 0; i < 9; i++) {
    sampleTex[i] = vec3(texture(uTexture, texCoords.st + offsets[i]));
  }
  vec3 col = vec3(0.0);

  for(int i = 0; i < 9; i++)
    col += sampleTex[i] * blurKernel[i];

  fragColor = vec4(col, 1.0);
}
