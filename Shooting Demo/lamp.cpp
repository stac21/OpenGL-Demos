#include "lamp.h"

Lamp::Lamp(const glm::vec3& pos, const Model& model)
	: StaticEntity(pos, model) {}

Lamp::Lamp(const glm::vec3& pos, const glm::vec3& scale, const Model& model)
	: StaticEntity(pos, scale, model){}

Lamp::Lamp(const glm::vec3& pos, const glm::vec3& rotation_axis, float rotation_angle, const Model& model)
	: StaticEntity(pos, rotation_axis, rotation_angle, model) {}

Lamp::Lamp(const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotation_axis, float rotation_angle, const Model& model)
	: StaticEntity(pos, scale, rotation_axis, rotation_angle, model) {}

bool Lamp::checkCollision(const RayCast& ray) {
	return false;
}