#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 instancedMatrix;

out VS_OUT {
	vec2 texCoords;
} vs_out;

uniform mat4 projection;
uniform mat4 view;

void main() {
	vec4 vertexPos = vec4(aPos, 1.0f);
	vs_out.texCoords = aTexCoords;

	gl_Position = projection * view * instancedMatrix * vertexPos;
}