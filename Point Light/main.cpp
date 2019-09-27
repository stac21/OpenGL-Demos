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
void render_cube();
void render_basic_scene(const Shader& shader);
void render_scene(const Shader& shader);
float get_delta_time();

float last_frame_time, current_frame_time = 0.0f;

glm::vec3 nanosuit_pos = glm::vec3(0.0f, -1.75f, 0.0f);
glm::vec3 cube_pos = glm::vec3(0.0f, 1.0f, 0.0f);

unsigned int cube_vao;

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

	Shader object_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\model_loading.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\model_loading.fs");
	Shader depth_shader("C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\depth_map.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\depth_map.fs", 
		"C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\depth_map.gs");
	Shader quad_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\quad.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Point Lights\\quad.fs");
	Shader lamp_shader("C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\lamp.vs", "C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\lamp.fs");
	Shader cube_shader("C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\cube.vs", "C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\cube.fs");

	Model nanosuit("C:/Users/Grant/LearnOpenGL Files/Assimp/nanosuit/nanosuit.obj");

	Texture wall(GL_TEXTURE_2D, Texture::DIFFUSE);
	wall.bind();
	wall.generate_texture("C:\\Users\\Grant\\LearnOpenGL FIles\\Point Lights\\wall.jpg", GL_RGB);

	object_shader.use();
	object_shader.set_int("diffuse_texture1", 0);
	object_shader.set_int("shadowMap", 1);
	quad_shader.use();
	quad_shader.set_int("screenTexture", 0);
	cube_shader.use();
	cube_shader.set_int("wallTexture", 0);

	glm::vec3 light_pos = glm::vec3(0.0f, 0.0f, 0.0f);

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

	// skybox vao
	unsigned int skybox_vao, skybox_vbo;

	glGenVertexArrays(1, &skybox_vao);
	glGenBuffers(1, &skybox_vbo);

	glBindVertexArray(skybox_vao);

	glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

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

	// cube vao
	unsigned int cube_vbo;

	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindVertexArray(cube_vao);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	/*
	depth buffer to create the depth map. The depth map is the depth texture as rendered from the
	light's perspective that we'll be using for calculating shadows.
	*/
	const unsigned int SHADOW_WIDTH = 1024;
	const unsigned int SHADOW_HEIGHT = 1024;

	unsigned int depth_cubemap;
	glGenTextures(1, &depth_cubemap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);

	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_WIDTH, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
	Create a framebuffer object for rendering the depth map and attach the generated depth texture
	to the framebuffer

	glDrawBuffer(GL_NONE) and glReadBuffer(GL_NONE) are necessary because a framebuffer object
	is not complete without a color buffer so we need to explicitly tell OpenGL
	that we aren't going to render any color data
	*/
	unsigned int depth_map_fbo;
	glGenFramebuffers(1, &depth_map_fbo);

	glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		// move light position over time
		light_pos.z = sin(glfwGetTime() * 0.5) * 3.0;

		// input
		process_input(window);

		// render to the depth map
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		depth_shader.use();

		float light_near_plane = 1.0f;
		float light_far_plane = 25.0f;

		/*
		For point lights, a perspective matrix is used because we must set the fov parameter
		to 90 degrees. By setting the fov to 90 degrees, we make sure the viewing field is exactly large
		enough to properly fill a single face of the cubemap such that all faces align correctly
		to each other at the edges.

		Because the projection matrix does not change per direction we can re-use it for all six
		faces of the cubemap, though we do need a different view matrix per direction. Using
		six lookAt matrices, we creates view matrices in six different view directions, each looking
		at a single direction of the cubemap in the order: right, left, top, bottom, near and far
		*/
		glm::mat4 shadow_proj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, light_near_plane, light_far_plane);
		std::vector<glm::mat4> shadow_transforms;
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
		shadow_transforms.push_back(shadow_proj * glm::lookAt(light_pos, light_pos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

		glm::mat4 light_space_matrix = shadow_proj;

		for (unsigned int i = 0; i < 6; i++) {
			depth_shader.set_mat4(("lightSpaceMatrices[" + std::to_string(i) + "]").c_str(), shadow_transforms.at(i));
		}

		depth_shader.set_vec3("lightPos", light_pos);
		depth_shader.set_float("farPlane", light_far_plane);

		glActiveTexture(GL_TEXTURE0);
		wall.bind();

		render_basic_scene(depth_shader);

		// render scene
		glViewport(0, 0, WindowInfo::width, WindowInfo::height);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		proj = glm::perspective(Camera::fov, (float)WindowInfo::width / (float)WindowInfo::height, Camera::near_plane, Camera::far_plane);
		view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);

		object_shader.use();

		glActiveTexture(GL_TEXTURE0);
		wall.bind();
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);

		glUniformMatrix4fv(glGetUniformLocation(object_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(object_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniform3fv(glGetUniformLocation(object_shader.ID, "lightPos"), 1, glm::value_ptr(light_pos));
		glUniform3fv(glGetUniformLocation(object_shader.ID, "viewPos"), 1, glm::value_ptr(Camera::pos));
		object_shader.set_float("farPlane", light_far_plane);

		render_basic_scene(object_shader);

		lamp_shader.use();

		glm::mat4 model;
		model = glm::translate(model, light_pos);

		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		render_cube();

		// quad
		glViewport(0, 0, WindowInfo::width, WindowInfo::height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);

		quad_shader.use();
		quad_shader.set_float("nearPlane", light_near_plane);
		quad_shader.set_float("farPlane", light_far_plane);

		glBindVertexArray(quad_vao);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_color_buffer);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteFramebuffers(1, &fbo);
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteVertexArrays(1, &lamp_vao);
	glDeleteBuffers(1, &skybox_vbo);
	glDeleteBuffers(1, &quad_vbo);
	glDeleteBuffers(1, &lamp_vbo);

	glfwTerminate();

	return 0;
}

void render_cube() {
	glBindVertexArray(cube_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void render_basic_scene(const Shader& shader) {
	// small cube
	glm::mat4 model = glm::mat4();
	model = glm::translate(model, cube_pos);
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	shader.set_bool("reverseNormals", 0);

	render_cube();

	// large cube
	model = glm::scale(model, glm::vec3(8.0f));
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	shader.set_bool("reverseNormals", 1);

	render_cube();

	shader.set_bool("reverseNormals", 0);
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