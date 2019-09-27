#pragma once

#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>

#include "stb_image.h"

class Texture {
public:
	Texture(const char* image_path);
};