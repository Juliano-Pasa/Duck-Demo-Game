// Vertex Shader //

#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec2 vertexUV;

uniform mat4 MVP;
uniform mat3 NM;
uniform float shellHeight;

out vec3 vColor;
out vec3 vNormal;
out vec2 vUV;

void main()
{
	vNormal = normalize(NM * vertexNormal);
	vUV = vertexUV;
	vColor = vertexColor;

	vec3 position = vertexPosition + vertexNormal * shellHeight;
	gl_Position = MVP * vec4(position, 1.0);
}