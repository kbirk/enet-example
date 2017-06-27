#version 410

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec4 aWeights;

uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

out vec3 vMVPosition;
out vec3 vMVNormal;
out vec2 vTexCoord;
out vec4 vWeights;

void main() {
    vec4 mvPos = uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
    gl_Position = uProjectionMatrix * mvPos;
    vMVPosition = vec3(mvPos) / mvPos.w;
    vMVNormal = mat3(uViewMatrix * uModelMatrix) * aNormal;
    vTexCoord = aTexCoord;
    vWeights = aWeights;
}
