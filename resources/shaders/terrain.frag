#version 410

uniform sampler2D uTextureSampler0;
uniform sampler2D uTextureSampler1;
uniform sampler2D uTextureSampler2;
uniform sampler2D uTextureSampler3;

uniform mat4 uViewMatrix;
uniform vec3 uLightPosition0;

in vec3 vMVPosition;
in vec3 vMVNormal;
in vec3 vMVLight;
in vec2 vTexCoord;
in vec4 vWeights;

out vec4 oColor;

void main() {

    vec4 texelColor = vec4(0.0, 0.0, 0.0, 1.0);
    texelColor += vWeights.x * texture(uTextureSampler0, vec2(vTexCoord.x, vTexCoord.y));
    texelColor += vWeights.y * texture(uTextureSampler1, vec2(vTexCoord.x, vTexCoord.y));
    texelColor += vWeights.z * texture(uTextureSampler2, vec2(vTexCoord.x, vTexCoord.y));
    texelColor += vWeights.w * texture(uTextureSampler3, vec2(vTexCoord.x, vTexCoord.y));

    vec3 normal = normalize(vMVNormal);
    vec3 vLight = vec3(uViewMatrix * vec4(uLightPosition0, 1.0));
    vec3 lightDir = normalize(vLight - vMVPosition);

    float ambient = 0.4;
    float diffuse = max(dot(lightDir, normal), 0.0);

    oColor = vec4(
        ambient * texelColor.rgb +
        diffuse * texelColor.rgb,
        texelColor.a);
}
