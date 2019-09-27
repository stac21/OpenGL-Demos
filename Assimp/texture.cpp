#include "texture.h"

Texture::Texture(int target, int type) {
	this->target = target;
	this->type = type;
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

void Texture::set_id(unsigned int id) {
	this->id = id;
}

void Texture::set_target(int enum_type) {
	this->target = enum_type;
}

void Texture::set_type(int type) {
	this->type = type;
}

void Texture::set_path(std::string& path) {
	this->path = path;
}

const unsigned int Texture::get_id() {
	return this->id;
}

const int Texture::get_target() {
	return this->target;
}

const int Texture::get_type() {
	return this->type;
}

const std::string Texture::get_path() {
	return this->path;
}