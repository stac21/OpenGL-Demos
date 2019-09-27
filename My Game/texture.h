#pragma once

#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>

#include "stb_image.h"

class Texture {
private:
	unsigned int id;
	int target;

public:
	Texture(int enum_type);
	void bind();
	void generate_texture(const char* image_path, int rgb_type);
};

