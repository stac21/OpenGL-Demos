#pragma once

#include <glm/glm.hpp>

class RayCast {
private:
	static const int DISTANCE = 20;
	glm::vec3 start_point;
	glm::vec3 end_point;

public:
	RayCast();
	RayCast(int start_x, int start_y, int start_z);

	// setters
	void set_start_point(int x, int y, int z);
	void set_start_x(int x);
	void set_start_y(int y);
	void set_start_z(int z);

	// getters
	glm::vec3 get_start_point() const;
	glm::vec3 get_end_point() const;
};