#include "cockpit.h"

Cockpit::Cockpit(const char* image_path) :
	texture(GL_TEXTURE_2D, Texture::DIFFUSE),
	quad_rendered_once(false) {

	this->texture.bind();
	this->texture.generate_texture(image_path, GL_RGBA);
}

void Cockpit::draw() {
	glActiveTexture(GL_TEXTURE0);
	this->texture.bind();
	this->render_quad();
}

Texture Cockpit::get_texture() const {
	return this->texture;
}

void Cockpit::render_quad() {
	if (!this->quad_rendered_once) {
		this->quad_rendered_once = true;

		float vertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};

		glGenVertexArrays(1, &quad_vao);
		glGenBuffers(1, &quad_vbo);

		glBindVertexArray(quad_vao);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(quad_vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}