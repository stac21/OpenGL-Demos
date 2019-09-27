#pragma once

#include <glm/glm.hpp>

#include "shader.h"

class OBB {
private:
	glm::vec3 center_point;
	glm::vec3 up;
	glm::vec3 u, v, w;
	unsigned int vao, vbo;
	
	float half_width, half_height, half_depth;

public:
	OBB(const glm::vec3& center_point, const glm::vec3& front, float half_width, float half_height, float half_depth);
	void draw(const Shader& shader);
};