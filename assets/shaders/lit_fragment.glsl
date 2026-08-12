#version 460 core

out vec4 fragColor;

in vec2 texCoords;
in vec2 fragWorldPos;

uniform vec4 uColor;
uniform sampler2D uTexture;

uniform bool uRequiresFov;
uniform vec2 uFovPos;
uniform vec2 uFovDir;
uniform float uFovInnerRadius;
uniform float uFovOuterRadius;
uniform float uFovHalfAngleRad;
uniform float uFovSmoothnessRad;
uniform float uFovSmoothnessDist;

void main() {
    vec4 texColor = texture(uTexture, texCoords) * uColor;

    if (uRequiresFov) {
        vec2 fragPos = fragWorldPos.xy;
        vec2 toFrag = fragPos - uFovPos;
        float dist = length(toFrag);

        float innerDistVis = 1.0 - smoothstep(0.0, uFovInnerRadius, dist);

        float coneVis = 1.0;
        if (uFovHalfAngleRad > 0.001f) {
            vec2 dirToFrag = (dist > 0.0001f) ? normalize(toFrag) : uFovDir;
            float dotProd = dot(uFovDir, dirToFrag);
            float fragAngleRad = acos(clamp(dotProd, -1.0f, 1.0f));

            float innerAngle = max(0.0f, uFovHalfAngleRad - uFovSmoothnessRad);
            float outerAngle = uFovHalfAngleRad + uFovSmoothnessRad;

            coneVis = 1.0 - smoothstep(innerAngle, outerAngle, fragAngleRad);
        }

        float outerDistVis = 1.0 - smoothstep(uFovOuterRadius - uFovSmoothnessDist, uFovOuterRadius, dist);
        float coneFinal = coneVis * outerDistVis;

        float visibility = 1.0 - (1.0 - coneFinal) * (1.0 - innerDistVis);
        visibility = clamp(visibility, 0.0f, 1.0f);

        texColor.a *= visibility;
    }

    if (texColor.a < 0.01f) {
        discard;
    }

    fragColor = texColor;
}
