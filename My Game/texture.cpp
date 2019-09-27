#include "texture.h"

Texture::Texture(int enum_type) {
	this->target = enum_type;
	glGenTextures(1, &id);
}

void Texture::bind() {
	glBindTexture(this->target, this->id);
}

void Texture::generate_texture(const char* image_path, int rgb_type) {
	int width, height, num_channels;
	unsigned char* data = stbi_load(image_path, &width, &height, &num_channels, 0);

	glTexParameteri(this->target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(this->target, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set the texture filtering method for downscaling then upscaling
	glTexParameteri(this->target, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(this->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data) {
		glTexImage2D(this->target, 0, GL_RGB, width, height, 0, rgb_type, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(this->target);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	stbi_image_free(data);
}