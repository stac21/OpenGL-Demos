#include "camera.h"

float Camera::camera_speed = 2.5f;
float Camera::sensitivity = 0.05f;
float Camera::fov = glm::radians(45.0f);
float Camera::pitch = 0.0f;
float Camera::yaw = -90.0f;
float Camera::last_x = 0.0f;
float Camera::last_y = 0.0f;
float Camera::near_plane = 0.1f;
float Camera::far_plane = 100.0f;
const float Camera::PITCH_LIMIT = 89.0f;

glm::vec3 Camera::pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 Camera::front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::up = glm::vec3(0.0f, 1.0f, 0.0f);

void Camera::process_keyboard_input(CAMERA_DIRECTIONS direction, float delta_time) {
	float distance = camera_speed * delta_time;

	switch (direction) {
	case FORWARD: {
		float temp_y = pos.y;
		pos += distance * front;
		pos.y = temp_y;
	}
		break;
	case BACKWARD: {
		float temp_y = pos.y;
		pos -= distance * front;
		pos.y = temp_y;
	}
		break;
	case LEFT: {
		float temp_y = pos.y;
		pos -= distance * glm::normalize(glm::cross(front, up));
		pos.y = temp_y;
	}
		break;
	case RIGHT: {
		float temp_y = pos.y;
		pos += distance * glm::normalize(glm::cross(front, up));
		pos.y = temp_y;
	}
		break;
	case UP:
		pos += distance * up;
		break;
	case DOWN:
		pos -= distance * up;
		break;
	}
}

void Camera::process_mouse_input(double xpos, double ypos) {
	float x_offset = sensitivity * ((float)xpos - last_x);
	float y_offset = sensitivity * (last_y - (float)ypos);

	last_x = xpos;
	last_y = ypos;

	yaw += x_offset;
	pitch += y_offset;

	if (pitch > PITCH_LIMIT) {
		pitch = 89.0f;
	} else if (pitch < -PITCH_LIMIT) {
		pitch = -89.0f;
	}

	glm::vec3 vec;
	vec.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	vec.y = sin(glm::radians(pitch));
	vec.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	front = glm::normalize(vec);
}