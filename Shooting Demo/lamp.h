#pragma once

#include "static_entity.h"
#include "box.h"
#include "ray_cast.h"

class Lamp :public StaticEntity {
private:
	Box aabb;

public:
	Lamp(const glm::vec3& pos, const Model& model);
	Lamp(const glm::vec3& pos, const glm::vec3& scale, const Model& model);
	Lamp(const glm::vec3& pos, const glm::vec3& rotation_axis, float rotation_angle, const Model& model);
	Lamp(const glm::vec3& pos, const glm::vec3& scale, const glm::vec3& rotation_axis, float rotation_angle, const Model& model);

	bool checkCollision(const RayCast& ray);
};