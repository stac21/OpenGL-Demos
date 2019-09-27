#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "stb_image.h"

#include "shader.h"
#include "window_info.h"
#include "texture.h"
#include "camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
float get_delta_time();

float last_frame_time, current_frame_time = 0.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(WindowInfo::width, WindowInfo::height, WindowInfo::TITLE, NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		//return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// prevents the mouse cursor from leaving the window and hides the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	int temp_x, temp_y;
	glfwGetFramebufferSize(window, &temp_x, &temp_y);

	Camera::last_x = (float)temp_x / 2, Camera::last_y = (float)temp_y / 2;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	float vertices[] = {
		// positions          // normals           // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
	};

	glm::vec3 container_positions[] = {
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 point_light_positions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	Shader lighting_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\colors.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\container.fs");
	Shader lamp_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\lamp.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\lamp.fs");

	Texture container(GL_TEXTURE_2D);
	container.bind();
	container.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\container2.png", GL_RGBA);

	Texture container_specular(GL_TEXTURE_2D);
	container_specular.bind();
	container_specular.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Multiple Lights\\container2_specular.png", GL_RGBA);

	unsigned int box_vao, light_vao, vbo;
	glGenVertexArrays(1, &box_vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(box_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// set aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// set aNormal
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// set aTexCoords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * (sizeof(float)), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &light_vao);
	glBindVertexArray(light_vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	// set aPos
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glEnable(GL_DEPTH_TEST);

	glm::mat4 view, proj, model;

	while (!glfwWindowShouldClose(window)) {
		last_frame_time = current_frame_time;
		current_frame_time = (float)glfwGetTime();

		// input
		process_input(window);

		glClearColor(0.001f, 0.001f, 0.001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		lighting_shader.use();

		view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);
		proj = glm::perspective(Camera::fov, (float)WindowInfo::width / (float)WindowInfo::height, 0.1f, 100.0f);

		glUniformMatrix4fv(glGetUniformLocation(lighting_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lighting_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "viewPos"), Camera::pos.x, Camera::pos.y, Camera::pos.z);

		glUniform3f(glGetUniformLocation(lighting_shader.ID, "material.ambient"), 1.0f, 0.5f, 0.31f);
		glUniform1f(glGetUniformLocation(lighting_shader.ID, "material.shininess"), 32.0f);
		
		lighting_shader.set_int("material.diffuse", 0);
		lighting_shader.set_int("material.specular", 1);

		glm::vec3 light_color(1.0);

		glm::vec3 diffuse_color = light_color * glm::vec3(0.3f);	// decrease the influence
		glm::vec3 ambient_color = light_color * glm::vec3(0.1f);	// low influence

		glUniform3f(glGetUniformLocation(lighting_shader.ID, "spotlight.ambient"), 0.0f, 0.0f, 0.0f);
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "spotlight.diffuse"), 0.5f, 0.5f, 0.5f);
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "spotlight.specular"), 1.0f, 1.0f, 1.0f);

		glUniform3f(glGetUniformLocation(lighting_shader.ID, "spotlight.pos"), Camera::pos.x, Camera::pos.y, Camera::pos.z);
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "spotlight.direction"), Camera::front.x, Camera::front.y, Camera::front.z);
		/*
		Passing a cosine value instead of an angle because in the fragment shader we're calculating
		the dot product between lightDir and spotDir (the dot product returns a cosine value, not
		an angle) so we can't directly compare the dot product between the two with an angle.
		Thus, the cosine value is passed into the fragment shader so we can compare them in the
		shader without going through expensive operations 
		*/
		glUniform1f(glGetUniformLocation(lighting_shader.ID, "spotlight.cutOff"), glm::cos(glm::radians(12.5f)));
		glUniform1f(glGetUniformLocation(lighting_shader.ID, "spotlight.outerCutOff"), glm::cos(glm::radians(17.5f)));

		glUniform1f(glGetUniformLocation(lighting_shader.ID, "spotlight.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(lighting_shader.ID, "spotlight.linear"), 0.09f);
		glUniform1f(glGetUniformLocation(lighting_shader.ID, "spotlight.quadratic"), 0.032f);
		
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "dirLight.direction"), -0.2f, -1.0f, -0.3f);

		glUniform3f(glGetUniformLocation(lighting_shader.ID, "dirLight.ambient"), ambient_color.x, ambient_color.y, ambient_color.z);
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "dirLight.diffuse"), diffuse_color.x, diffuse_color.y, diffuse_color.z);
		glUniform3f(glGetUniformLocation(lighting_shader.ID, "dirLight.specular"), 1.0f, 1.0f, 1.0f);

		for (GLuint i = 0; i < 4; i++)
		{
			std::string number = std::to_string(i);

			glUniform3f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].pos").c_str()), point_light_positions[i].x, point_light_positions[i].y, point_light_positions[i].z);
			glUniform3f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].ambient").c_str()), 0.05f, 0.05f, 0.05f);
			glUniform3f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].diffuse").c_str()), 0.3f, 0.9f, 0.3f);
			glUniform3f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].specular").c_str()), 1.0f, 1.0f, 1.0f);
			glUniform1f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].constant").c_str()), 1.0f);
			glUniform1f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].linear").c_str()), 0.09f);
			glUniform1f(glGetUniformLocation(lighting_shader.ID, ("pointLights[" + number + "].quadratic").c_str()), 0.032f);
		}

		glActiveTexture(GL_TEXTURE0);
		container.bind();
		glActiveTexture(GL_TEXTURE1);
		container_specular.bind();
	
		glBindVertexArray(box_vao);

		for (int i = 0; i < 10; i++) {
			model = glm::mat4();
			model = glm::translate(model, container_positions[i]);
			float angle = glfwGetTime();
			model = glm::rotate(model, glm::radians(angle * i), glm::vec3(1.0f, 0.3f, 0.5f));

			glUniformMatrix4fv(glGetUniformLocation(lighting_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		lamp_shader.use();

		glBindVertexArray(light_vao);

		for (int i = 0; i < 4; i++) {
			model = glm::mat4();
			model = glm::translate(model, point_light_positions[i]);
			model = glm::scale(model, glm::vec3(0.2f));
			
			glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
			glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
			glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &box_vao);
	glDeleteVertexArrays(1, &light_vao);
	glDeleteBuffers(1, &vbo);

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	WindowInfo::width = width;
	WindowInfo::height = height;

	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	Camera::process_mouse_input(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	if (Camera::fov >= 40.0f && y_offset < 0) {
		Camera::fov -= y_offset;
	} else if (Camera::fov <= 50.0f&&y_offset > 0) {
		Camera::fov += y_offset;
	}
}

void process_input(GLFWwindow* window) {
	float camera_speed = 2.5f;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::FORWARD, get_delta_time());
	} if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::BACKWARD, get_delta_time());
	}  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::LEFT, get_delta_time());
	} if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::RIGHT, get_delta_time());
	}  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::UP, get_delta_time());
	}  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		Camera::process_keyboard_input(Camera::CAMERA_DIRECTIONS::DOWN, get_delta_time());
	}
}

float get_delta_time() {
	return current_frame_time - last_frame_time;
}