#version 410

layout(location=0) in vec3 aPosition;
layout(location=1) in vec3 aNormal;

uniform mat4 uModelMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

out vec3 vMVPosition;
out vec3 vMVNormal;

void main() {
	vec4 mvPos = uViewMatrix * uModelMatrix * vec4(aPosition, 1.0);
	gl_Position = uProjectionMatrix * mvPos;
	vMVPosition = vec3(mvPos) / mvPos.w;
	vMVNormal = mat3(uViewMatrix * uModelMatrix) * aNormal;
}
