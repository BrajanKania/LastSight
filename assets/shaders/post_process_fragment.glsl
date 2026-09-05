#version 460 core

in vec2 texCoords;

uniform sampler2D uScreenTexture;

out vec4 fragColor;

uniform vec3 uDamageVignetteColor;
uniform float uDamageInnerRadius;
uniform float uDamageOuterRadius;
uniform float uMaxDamageDesaturation;
uniform float uDamageIntensity;

uniform float uStaminaInnerRadius;
uniform float uStaminaOuterRadius;
uniform float uMaxStaminaDesaturation;
uniform float uStaminaIntensity;

void main() {
  vec4 color = texture(uScreenTexture, texCoords);

  vec2 distFromCenter = texCoords - vec2(0.5);
  float len = length(distFromCenter);

  float damageFactor = 0.0;
  if (uDamageOuterRadius > uDamageInnerRadius) {
    damageFactor = smoothstep(uDamageInnerRadius, uDamageOuterRadius, len) * uDamageIntensity;
  }

  float staminaFactor = 0.0;
  if (uStaminaOuterRadius > uStaminaInnerRadius) {
    staminaFactor = smoothstep(uStaminaInnerRadius, uStaminaOuterRadius, len) * uStaminaIntensity;
  }

  float totalDesatFactor = clamp(
      damageFactor * uMaxDamageDesaturation + staminaFactor * uMaxStaminaDesaturation,
      0.0,
      1.0
    );

  float gray = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
  vec3 desaturatedColor = mix(color.rgb, vec3(gray), totalDesatFactor);

  vec3 finalColor = mix(desaturatedColor, uDamageVignetteColor, damageFactor * 0.5);

  fragColor = vec4(finalColor, 1.0);
}
