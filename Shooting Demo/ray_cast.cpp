#include "ray_cast.h"

RayCast::RayCast()
	: start_point(), end_point() {

}
RayCast::RayCast(int start_x, int start_y, int start_z) 
	: start_point(start_x, start_y, start_z), end_point(this->start_point.x * this->DISTANCE, this->start_point.y * this->DISTANCE, this->start_point.z * this->DISTANCE) {}

// setters
void RayCast::set_start_point(int x, int y, int z) {
	this->start_point.x = x;
	this->start_point.y = y;
	this->start_point.z = z;
}

void RayCast::set_start_x(int x) {
	this->start_point.x = x;
}

void RayCast::set_start_y(int y) {
	this->start_point.y = y;
}

void RayCast::set_start_z(int z) {
	this->start_point.z = z;
}

// getters
glm::vec3 RayCast::get_start_point() const {
	return this->start_point;
}

glm::vec3 RayCast::get_end_point() const {
	return this->end_point;
}