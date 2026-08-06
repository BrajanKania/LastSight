#version 460 core

in vec2 texCoords;

uniform sampler2D uTexture;

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


void main(){
  vec4 color = texture(uTexture, texCoords);

  vec2 distFromCenter = texCoords - vec2(0.5f);
  float len = length(distFromCenter);

  float damageFactor = smoothstep(uDamageInnerRadius, uDamageOuterRadius, len) * uDamageIntensity;

  float staminaFactor = smoothstep(uStaminaInnerRadius, uStaminaOuterRadius, len) * uStaminaIntensity;

  float totalDesatFactor = clamp(
    damageFactor * uMaxDamageDesaturation + staminaFactor * uMaxStaminaDesaturation, 
    0.0f, 
    1.0f
  );

  float gray = dot(color.rgb, vec3(0.2126f, 0.7152f, 0.0722f));
  vec3 desaturatedColor = mix(color.rgb, vec3(gray), totalDesatFactor);

  vec3 finalColor = mix(desaturatedColor, uDamageVignetteColor, damageFactor * 0.5f);

  fragColor = vec4(finalColor, color.a);
}
