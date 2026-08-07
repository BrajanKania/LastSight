#version 460 core

out vec4 fragColor;

in vec2 texCoords;

uniform sampler2D uScreenTexture;

uniform vec2 uViewPos;
uniform vec2 uViewDir;
uniform mat4 uInvViewProj;

uniform float uInnerRadius;
uniform float uOuterRadius;
uniform float uHalfFovRad;
uniform float uSmoothnessRad;
uniform float uSmoothnessDistance;

uniform float uDarkness;

void main() {
  vec4 fragNDC = vec4(texCoords.xy * 2.f - 1.f, 0.f, 1.f);
  vec2 fragPos = (uInvViewProj * fragNDC).xy;

  vec2 fragDir =  normalize(fragPos - uViewPos);
  float fragAngleCos = dot(uViewDir, fragDir);

  float fragAngleRad = acos(clamp(fragAngleCos, -1.0, 1.0));

  float innerRad = max(0.0, uHalfFovRad - uSmoothnessRad);
  float outerRad = uHalfFovRad + uSmoothnessRad;

  float coneVis = 1.0 - smoothstep(innerRad, outerRad, fragAngleRad);

  float dist = length(fragPos - uViewPos);

  float outerDistVis = 1.f - smoothstep(uOuterRadius - uSmoothnessDistance, uOuterRadius, dist);
  float innerDistVis = 1.f - smoothstep(0.f, uInnerRadius, dist);

  float coneFinal = coneVis * outerDistVis;
  float visibility = 1.f - (1.f - coneFinal) * (1.f - innerDistVis);
  visibility = clamp(visibility, 0.f, 1.f);

  vec4 screenColor = texture(uScreenTexture, texCoords);
  float gray = dot(screenColor.rgb, vec3(0.2126f, 0.7152f, 0.0722f));

  vec4 grayColor = vec4(vec3(gray) * uDarkness, screenColor.a);

  fragColor = mix(grayColor, screenColor, visibility);
}
