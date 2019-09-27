#pragma once
#include <glm/glm.hpp>

class Camera {
public:
	static float camera_speed, sensitivity, fov, pitch, yaw, last_x, last_y, near_plane, far_plane;
	static const float PITCH_LIMIT;
	static glm::vec3 pos, front, up;

	enum CAMERA_DIRECTIONS {
		FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN
	};

	static void process_keyboard_input(CAMERA_DIRECTIONS direction, float delta_time);
	static void process_mouse_input(double xpos, double ypos);
};
