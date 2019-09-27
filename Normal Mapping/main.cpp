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
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
void render_plane();
float get_delta_time();

float last_frame_time, current_frame_time = 0.0f;

unsigned int plane_vao, plane_vbo;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
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
	glfwSetScrollCallback(window, scroll_callback);
	// prevents the mouse cursor from leaving the window and hides the cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	Camera::last_x = WindowInfo::width / 2.0f;
	Camera::last_y = WindowInfo::height / 2.0f;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}

	Shader plane_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\plane.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\plane.fs");
	Shader quad_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\quad.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\quad.fs");
	Shader lamp_shader("C:\\Users\\Grant\\LearnOpenGL FIles\\Normal Mapping\\lamp.vs", "C:\\Users\\Grant\\LearnOpenGL FIles\\Normal Mapping\\lamp.fs");

	Texture brick_wall(GL_TEXTURE_2D, Texture::DIFFUSE);
	brick_wall.bind();
	brick_wall.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\brickwall.jpg", GL_RGB);
	
	Texture brick_wall_normal(GL_TEXTURE_2D, Texture::NORMAL);
	brick_wall_normal.bind();
	brick_wall_normal.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Normal Mapping\\brickwall_normal.jpg", GL_RGB);

	plane_shader.use();
	plane_shader.set_int("texture_diffuse1", 0);
	plane_shader.set_int("texture_normal1", 1);
	quad_shader.use();
	quad_shader.set_int("screenTexture", 0);

	glm::vec3 light_pos = glm::vec3(2.0f, 2.0f, 2.0f);
	glm::vec3 light_color = glm::vec3(1.0f);

	float quad_vertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
							  // positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	float cube_vertices[] = {
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

	float lamp_vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
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

	// lamp vao
	unsigned int lamp_vao, lamp_vbo;

	glGenVertexArrays(1, &lamp_vao);
	glGenBuffers(1, &lamp_vbo);

	glBindVertexArray(lamp_vao);

	glBindBuffer(GL_ARRAY_BUFFER, lamp_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lamp_vertices), lamp_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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
	glDisable(GL_CULL_FACE);

	// to draw the nanosuit in wireframe, uncomment this
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 view, proj;

	glEnable(GL_MULTISAMPLE);

	while (!glfwWindowShouldClose(window)) {
		last_frame_time = current_frame_time;
		current_frame_time = (float)glfwGetTime();

		// input
		process_input(window);

		// render scene
		glViewport(0, 0, WindowInfo::width, WindowInfo::height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		proj = glm::perspective(Camera::fov, (float)WindowInfo::width / (float)WindowInfo::height, Camera::near_plane, Camera::far_plane);
		view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);

		plane_shader.use();

		glm::mat4 model = glm::mat4();
		//model = glm::rotate(model, glm::radians(90.0f), glm::normalize(glm::vec3(0.0, 1.0, 0.0)));
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * -10.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0))); // rotate the quad to show normal mapping from multiple directions
		//model = glm::scale(model, glm::vec3(0.5f));

		plane_shader.set_mat4("model", model);
		plane_shader.set_mat4("projection", proj);
		glUniformMatrix4fv(glGetUniformLocation(plane_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		plane_shader.set_vec3("viewPos", Camera::pos);
		plane_shader.set_vec3("lightPos", light_pos);
		plane_shader.set_vec3("lightColor", light_color);
	
		glActiveTexture(GL_TEXTURE0);
		brick_wall.bind();
		glActiveTexture(GL_TEXTURE1);
		brick_wall_normal.bind();

		render_plane();

		lamp_shader.use();

		model = glm::mat4();
		model = glm::translate(model, light_pos);

		lamp_shader.set_mat4("projection", proj);
		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		lamp_shader.set_mat4("model", model);

		glBindVertexArray(lamp_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);

		// quad
		glViewport(0, 0, WindowInfo::width, WindowInfo::height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);

		quad_shader.use();
		glBindVertexArray(quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteVertexArrays(1, &lamp_vao);
	glDeleteBuffers(1, &quad_vbo);
	glDeleteBuffers(1, &lamp_vbo);

	glfwTerminate();

	return 0;
}

void render_plane() {
	if (plane_vao == 0) {
		// positions
		glm::vec3 pos1(-1.0, 1.0, 0.0);
		glm::vec3 pos2(-1.0, -1.0, 0.0);
		glm::vec3 pos3(1.0, -1.0, 0.0);
		glm::vec3 pos4(1.0, 1.0, 0.0);
		// texture coordinates
		glm::vec2 uv1(0.0, 1.0);
		glm::vec2 uv2(0.0, 0.0);
		glm::vec2 uv3(1.0, 0.0);
		glm::vec2 uv4(1.0, 1.0);
		// normal vector
		glm::vec3 normal = glm::vec3(0.0, 0.0, 1.0);

		// calculate the triangle's edges and delta UV coordinates
		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 delta_uv1 = uv2 - uv1;
		glm::vec2 delta_uv2 = uv3 - uv1;

		// calculate the tangents and bitangets for the first triangle in the plane using the equation given
		float f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);
		glm::vec3 tangent1, bitangent1;

		tangent1.x = f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
		tangent1.y = f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
		tangent1.z = f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);
		tangent1 = glm::normalize(tangent1);

		bitangent1.x = f * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x);
		bitangent1.y = f * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y);
		bitangent1.z = f * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z);
		bitangent1 = glm::normalize(bitangent1);

		// now calculate the second triangle's edges and delta UV coordinates
		edge1 = pos3 - pos1;
		edge2 = pos4 - pos1;

		delta_uv1 = uv3 - uv1;
		delta_uv2 = uv4 - uv1;

		// now calculate the tangets and bitangets for the second triangle in the plane
		f = 1.0f / (delta_uv1.x * delta_uv2.y - delta_uv2.x * delta_uv1.y);
		glm::vec3 tangent2, bitangent2;

		tangent2.x = f * (delta_uv2.y * edge1.x - delta_uv1.y * edge2.x);
		tangent2.y = f * (delta_uv2.y * edge1.y - delta_uv1.y * edge2.y);
		tangent2.z = f * (delta_uv2.y * edge1.z - delta_uv1.y * edge2.z);
		tangent2 = glm::normalize(tangent2);

		bitangent2.x = f * (-delta_uv2.x * edge1.x + delta_uv1.x * edge2.x);
		bitangent2.y = f * (-delta_uv2.x * edge1.y + delta_uv1.x * edge2.y);
		bitangent2.z = f * (-delta_uv2.x * edge1.z + delta_uv1.x * edge2.z);
		bitangent2 = glm::normalize(bitangent2);

		float plane_vertices[] = {
			// positions            // normal         // texcoords  // tangent                          // bitangent
			pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos2.x, pos2.y, pos2.z, normal.x, normal.y, normal.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
			pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

			pos1.x, pos1.y, pos1.z, normal.x, normal.y, normal.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos3.x, pos3.y, pos3.z, normal.x, normal.y, normal.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
			pos4.x, pos4.y, pos4.z, normal.x, normal.y, normal.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
		};
		// configure plane VAO
		glGenVertexArrays(1, &plane_vao);
		glGenBuffers(1, &plane_vbo);
		glBindVertexArray(plane_vao);
		glBindBuffer(GL_ARRAY_BUFFER, plane_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(plane_vertices), &plane_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
	}

	glBindVertexArray(plane_vao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	WindowInfo::width = width;
	WindowInfo::height = height;

	std::cout << "framebuffer_size_callback called with width = " << width << " and height = " << height;

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
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {

		GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* vid_mode = glfwGetVideoMode(primary_monitor);

		glfwSetWindowMonitor(window, primary_monitor, 0, 0, 1920, 1080, vid_mode->refreshRate);

		framebuffer_size_callback(window, 1920, 1080);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
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