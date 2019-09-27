#pragma once

#include "texture.h"
#include "shader.h"

class Cockpit {
private:
	Texture texture;

	bool quad_rendered_once;
	unsigned int quad_vao, quad_vbo;

	void render_quad();
public:
	Cockpit(const char* image_path);

	void draw();

	Texture get_texture() const;
};