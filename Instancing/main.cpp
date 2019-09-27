#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <map>

#include <iostream>

#include "stb_image.h"

#include "shader.h"
#include "window_info.h"
#include "texture.h"
#include "camera.h"
#include "model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
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
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	// prevents the mouse cursor from leaving the window and hides the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera::last_x = WindowInfo::width / 2.0f;
	Camera::last_y = WindowInfo::height / 2.0f;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	Shader quad_shader("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\quad.vs", "C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\quad.fs");
	Shader skybox_shader("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox.vs", "C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox.fs");
	Shader planet_shader("C:\\Users\\MyNam\\LearnOpenGL Files\\Instancing\\planet_shader.vs", "C:\\Users\\MyNam\\LearnOpenGL Files\\Instancing\\model_shader.fs");
	Shader asteroid_shader("C:\\Users\\MyNam\\LearnOpenGL Files\\Instancing\\asteroid_shader.vs", "C:\\Users\\MyNam\\LearnOpenGL FIles\\Instancing\\model_shader.fs");

	Model planet("C:/Users/MyNam/LearnOpenGL Files/Instancing/planet/planet.obj");
	Model asteroid("C:/Users/MyNam/LearnOpenGL Files/Instancing/rock/rock.obj");

	std::vector<std::string> faces;
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\right.jpg");
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\left.jpg");
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\top.jpg");
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\bottom.jpg");
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\front.jpg");
	faces.push_back("C:\\Users\\MyNam\\LearnOpenGL Files\\Depth Testing\\skybox\\back.jpg");

	quad_shader.use();
	quad_shader.set_int("screenTexture", 0);

	float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							  // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	float skybox_vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// screen quad vao
	unsigned int quad_vao, quad_vbo;

	glGenVertexArrays(1, &quad_vao);
	glGenBuffers(1, &quad_vbo);

	glBindVertexArray(quad_vao);

	glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// configure the uniform buffer object
	// 1. get the relevant block indices
	unsigned int uniform_block_index_planet = glGetUniformBlockIndex(planet_shader.ID, "Matrices");
	unsigned int uniform_block_index_asteroid = glGetUniformBlockIndex(asteroid_shader.ID, "Matrices");

	// 2. link each shader's uniform block to this uniform binding point
	glUniformBlockBinding(planet_shader.ID, uniform_block_index_planet, 0);
	glUniformBlockBinding(asteroid_shader.ID, uniform_block_index_asteroid, 0);

	// 3. now actually create the buffer
	unsigned int ubo_matrices;
	glGenBuffers(1, &ubo_matrices);

	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	// data is NULL because we are merely allocating the memory in the buffer for the data, and will fill the memory with data later
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_matrices, 0, 2 * sizeof(glm::mat4));

	/*
	We need to attach one or more attachments to the framebuffer. An attachment is a memory
	location that can act as a buffer for the framebuffer. When creating an attachment we
	have the otion to take textures or renderbuffer objects

	When attaching a texture to the framebuffer, all rendering commands will write to the texture
	as if it was a normal color/depth or stencil buffer. The advantage of using textures is that
	the result of all rendering operations will be stored as a texture image that we can then easily
	use in shaders
	*/
	unsigned int texture_color_buffer;
	glGenTextures(1, &texture_color_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

	/*
	the main differences are that we set the dimensions to the screen size (although not required)
	and we pass NULL as the texture's data parameter since we're only allocating memory with this texture
	and not filling it. Filling the texture will happen as soon as we render to the framebuffer.
	Note that we do not care about mipmapping or any of the wrapping methods since we won't be needing
	those in most cases
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WindowInfo::width, WindowInfo::height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*
	Since renderbuffer objects are generally write-only they are often used as depth and stencil
	attachments, since most of the time we don't really need to read values from the depth and stencil
	buffers but still care about depth and stencil testing. We need the depth and stencil values
	for testing, but don't need to sample these values so a renderbuffer object suits this perfectly.
	Thus, when we are not sampling from these buffers, a renderbuffer object is generally
	preferred since it's more optimized
	*/
	/*
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);

	glBindRenderbuffer(GL_RENDERBUFFER, rbo);

	// create a depth and stencil renderbuffer object
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WindowInfo::width, WindowInfo::height);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	*/

	// create a depth and stencil attachment for the framebuffer
	unsigned int texture_depth_stencil_buffer;
	glGenTextures(1, &texture_depth_stencil_buffer);
	glBindTexture(GL_TEXTURE_2D, texture_depth_stencil_buffer);

	/*
	Attach both a depth buffer and a stencil buffer as a single texture. Each 32 bit value of the texture
	then consists of 24 bits of depth information and 8 bits for the stencil information.
	*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, WindowInfo::width, WindowInfo::height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

	/*
	The general rule is that if you never need to sample data from a specific buffer, it is wise
	to use a renderbuffer object for that specific buffer. If you need to someday sample data from
	a specific buffer like colors or depth buffers, you should use a texture attachment instead.
	Performance-wise it doesn't have an enormous impact though
	*/
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	/*
	target: the framebuffer type we're targeting (draw, read or both)

	attachment: the type of attachment we're going to attach. Note that the 0 at the end suggest that we
	can attach more than 1 color attachment

	textarget: the type of texture we want to attach

	texture: the actual texture to attach

	level: the mipmap level. We keep this at 0
	*/
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, texture_depth_stencil_buffer, 0);

	if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER::Framebuffer is not complete!" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// to draw the nanosuit in wireframe, uncomment this
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	constexpr unsigned int ASTEROID_AMOUNT = 3000;
	constexpr float RADIUS = 15.0f;
	constexpr float OFFSET = 2.5f;

	glm::mat4 model_matrices[ASTEROID_AMOUNT];
	srand(glfwGetTime());

	for (unsigned int i = 0; i < ASTEROID_AMOUNT; i++) {
		glm:: mat4 model = glm::mat4();

		// 1. translation: displace along circle with 'radius' in the range [-offset, offset]
		float angle = (float)i / (float)ASTEROID_AMOUNT * 360.0f;
		float displacement = (rand() % (int)(2 * OFFSET * 100)) / 100.0f - OFFSET;
		float x = sin(angle) * RADIUS + displacement;
		displacement = (rand() % (int)(2 * OFFSET * 100)) / 100.0f - OFFSET;
		float y = displacement * 0.4f;		// keep height of field smaller in comparision to width of x and z
		displacement = (rand() % (int)(2 * OFFSET * 100)) / 100.0f - OFFSET;
		float z = cos(angle) * RADIUS + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotation_angle = (rand() % 360);
		model = glm::rotate(model, rotation_angle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. add to array
		model_matrices[i] = model;
	}

	unsigned int asteroid_buffer;
	glGenBuffers(1, &asteroid_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, asteroid_buffer);
	glBufferData(GL_ARRAY_BUFFER, ASTEROID_AMOUNT * sizeof(glm::mat4), &model_matrices[0], GL_STATIC_DRAW);
	
	GLsizei vec4_size = sizeof(glm::vec4);

	for (unsigned int i = 0; i < asteroid.meshes.size(); i++) {
		unsigned int asteroid_vao = asteroid.meshes.at(i).VAO;

		glBindVertexArray(asteroid_vao);

		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)0);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(vec4_size));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(2 * vec4_size));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4_size, (void*)(3 * vec4_size));
		glEnableVertexAttribArray(6);

		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);

		glBindVertexArray(0);
	}

	// the projection matrix can be set once because the FOV will now remain constant
	glm::mat4 view, proj = glm::perspective(Camera::fov, (float)WindowInfo::width / (float)WindowInfo::height, Camera::near_plane, Camera::far_plane);
	
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(proj));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	while (!glfwWindowShouldClose(window)) {
		last_frame_time = current_frame_time;
		current_frame_time = (float)glfwGetTime();

		// input
		process_input(window);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.001f, 0.001f, 0.001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f));
		
		view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);

		glBindBuffer(GL_UNIFORM_BUFFER, ubo_matrices);
		glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		// draw planet
		planet_shader.use();

		glUniformMatrix4fv(glGetUniformLocation(planet_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
		planet.draw(planet_shader);

		// draw asteroids
		asteroid_shader.use();

		for (unsigned int i = 0; i < asteroid.meshes.size(); i++) {
			glBindVertexArray(asteroid.meshes.at(i).VAO);
			glDrawElementsInstanced(GL_TRIANGLES, asteroid.meshes.at(i).indices.size(), GL_UNSIGNED_INT, 0, ASTEROID_AMOUNT);
			glBindVertexArray(0);
		}

		// return to the normal framebuffer, instead of our custom one, and render the quad
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_STENCIL_TEST);
		glDisable(GL_CULL_FACE);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		quad_shader.use();

		glBindVertexArray(quad_vao);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glEnable(GL_CULL_FACE);
		glStencilMask(0xFF);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteBuffers(1, &quad_vbo);


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

void process_input(GLFWwindow* window) {
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