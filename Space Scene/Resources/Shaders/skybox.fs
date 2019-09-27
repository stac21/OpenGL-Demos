#version 330 core

out vec4 FragColor;

in VS_OUT {
	vec3 texCoords;
} fs_in;

uniform samplerCube skyboxCubemap;

void main() {
	FragColor = texture(skyboxCubemap, fs_in.texCoords);
}