#include "obb.h"

OBB::OBB(const glm::vec3& center_point, const glm::vec3& front, float half_width, float half_height, float half_depth)
	: center_point(center_point), up(0.0f, 1.0f, 0.0f), half_width(half_width), half_height(half_height), half_depth(half_depth) {
	this->u = glm::normalize(front);
	this->w = glm::normalize(glm::cross(u, up));
	this->v = glm::normalize(glm::cross(u, w));
}

void OBB::draw(const Shader& shader) {
	if (this->vao == NULL) {
		float points[] = {
			center_point.x, center_point.y, center_point.z
		};

		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &this->vbo);

		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		glEnableVertexAttribArray(0);
	}

	shader.set_vec3("u", this->u);
	shader.set_vec3("w", this->w);
	shader.set_vec3("v", this->v);

	glBindVertexArray(this->vao);
	glDrawArrays(GL_POINT, 0, 1);
	glBindVertexArray(0);
}
