#version 410

uniform mat4 uViewMatrix;
uniform vec3 uLightPosition0;

uniform vec4 uAmbientColor;
uniform vec4 uDiffuseColor;
uniform vec4 uSpecularColor;
uniform float uShininess;

in vec3 vMVPosition;
in vec3 vMVNormal;
in vec2 vTexCoord;

out vec4 oColor;

void main() {

	vec3 normal = normalize(vMVNormal);
	vec3 vLight = vec3(uViewMatrix * vec4(uLightPosition0, 1.0));
	vec3 lightDir = normalize(vLight - vMVPosition);
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 viewDir = normalize(-vMVPosition);

	float diffuse = max(dot(lightDir, normal), 0.0);
	float specular = 0.0;

	if (diffuse > 0.0) {
		float specAngle = max(dot(reflectDir, viewDir), 0.0);
		specular = pow(specAngle, uShininess);
	}

	oColor = vec4(
		uAmbientColor.rgb +
		diffuse * uDiffuseColor.rgb +
		specular * uSpecularColor.rgb, uDiffuseColor.a);
}
