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
float get_delta_time();

float last_frame_time, current_frame_time = 0.0f;

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

	Shader nanosuit_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\model_loading.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\model_loading.fs",
		"C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\model_loading.gs");
	Shader lamp_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Assimp\\lamp.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Assimp\\lamp.fs");
	Shader border_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\model_loading.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\border.fs");
	Shader grass_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\grass.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\grass.fs");
	Shader window_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\grass.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\window.fs");
	Shader quad_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\quad.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\quad.fs");
	Shader skybox_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox.fs");
	Shader cube_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\reflection.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\reflection.fs");
	Shader nanosuit_reflection_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\nanosuit_reflection.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\nanosuit_reflection.fs");
	Shader points_shader("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\points.vs", "C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\points.fs", 
		"C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\points.gs");

	Model nanosuit("C:/Users/Grant/LearnOpenGL Files/Assimp/nanosuit/nanosuit.obj");
	Model reflective_nanosuit("C:/Users/Grant/LearnOpenGL Files/Depth Testing/nanosuit_reflection/nanosuit.obj");
	Model lamp("C:/Users/Grant/LearnOpenGL Files/Assimp/nanosuit/nanosuit.obj");

	Texture grass_texture(GL_TEXTURE_2D, Texture::DIFFUSE);
	grass_texture.bind();
	grass_texture.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\grass.png", GL_RGBA);
	grass_shader.use();
	grass_shader.set_int("mTexture", 0);

	Texture window_texture(GL_TEXTURE_2D, Texture::DIFFUSE);
	window_texture.bind();
	window_texture.generate_texture("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\blending_transparent_window.png", GL_RGBA);
	window_shader.use();
	window_shader.set_int("mTexture", 0);

	std::vector<std::string> faces;
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\right.jpg");
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\left.jpg");
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\top.jpg");
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\bottom.jpg");
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\front.jpg");
	faces.push_back("C:\\Users\\Grant\\LearnOpenGL Files\\Depth Testing\\skybox\\back.jpg");

	Texture skybox_cubemap(GL_TEXTURE_CUBE_MAP, Texture::DIFFUSE);
	skybox_cubemap.bind();
	skybox_cubemap.generate_cubemap(faces, GL_RGB);
	skybox_shader.use();
	skybox_shader.set_int("skybox", 0);
	cube_shader.use();
	cube_shader.set_int("skybox", 0);

	quad_shader.use();
	quad_shader.set_int("screenTexture", 0);

	glm::vec3 nanosuit_pos = glm::vec3(0.0f, -1.75f, 0.0f);
	glm::vec3 reflective_nanosuit_pos = glm::vec3(1.0f, -1.75, 3.0f);
	glm::vec3 lamp_pos = glm::vec3(-1.0f, 0.0f, 5.0f);
	glm::vec3 cube_pos = glm::vec3(-1.0f, 1.0f, 3.0f);
	std::vector<glm::vec3> grass_pos;
	std::vector<glm::vec3> window_pos;
	grass_pos.push_back(glm::vec3(-1.5f, 0.0f, -0.48f));
	grass_pos.push_back(glm::vec3(1.5f, 0.0f, 0.51f));
	grass_pos.push_back(glm::vec3(0.0f, 0.0f, 0.7f));
	grass_pos.push_back(glm::vec3(-0.3f, 0.0f, -2.3f));
	grass_pos.push_back(glm::vec3(0.5f, 0.0f, -0.6f));
	
	for (unsigned int i = 0; i < grass_pos.size(); i++) {
		glm::vec3 grass = grass_pos.at(i);
		glm::vec3 pos = glm::vec3(grass.x - 1, grass.y + 2, grass.z + 3);

		window_pos.push_back(pos);
	}

	float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // top-left
		0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // bottom-left
	};

	float grass_vertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

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


	// grass vao
	unsigned int grass_vao, grass_vbo;

	glGenVertexArrays(1, &grass_vao);
	glGenBuffers(1, &grass_vbo);

	glBindVertexArray(grass_vao);
	glBindBuffer(GL_ARRAY_BUFFER, grass_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(grass_vertices), grass_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

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

	// cube vao
	unsigned int cube_vao, cube_vbo;

	glGenVertexArrays(1, &cube_vao);
	glGenBuffers(1, &cube_vbo);

	glBindVertexArray(cube_vao);

	glBindBuffer(GL_ARRAY_BUFFER, cube_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// points vao
	unsigned int points_vao, points_vbo;

	glGenVertexArrays(1, &points_vao);
	glGenBuffers(1, &points_vbo);
	
	glBindVertexArray(points_vao);

	glBindBuffer(GL_ARRAY_BUFFER, points_vao);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

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

	glm::mat4 view, proj;

	glEnable(GL_MULTISAMPLE);

	while (!glfwWindowShouldClose(window)) {
		last_frame_time = current_frame_time;
		current_frame_time = (float)glfwGetTime();

		// input
		process_input(window);

		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glClearColor(0.001f, 0.001f, 0.001f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		proj = glm::perspective(Camera::fov, (float)WindowInfo::width / (float)WindowInfo::height, Camera::near_plane, Camera::far_plane);
		view = glm::lookAt(Camera::pos, Camera::pos + Camera::front, Camera::up);
		glm::mat4 skybox_view = glm::mat4(glm::mat3(view));

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);	// all fragments should update the stencil buffer
		glStencilMask(0xFF);				// enable writing to the stencil buffer

		nanosuit_shader.use();
		
		// translate it down so it's at the center of the screen
		glm::mat4 model = glm::mat4();
		model = glm::translate(model, nanosuit_pos);
		// scale it down so that it is not too big for our scene
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(glGetUniformLocation(nanosuit_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(nanosuit_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(nanosuit_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform1f(glGetUniformLocation(nanosuit_shader.ID, "time"), glfwGetTime());

		nanosuit.draw(nanosuit_shader);

		// make sure we don't update the stencil buffer while drawing the lamp
		glStencilMask(0x00);

		nanosuit_shader.use();

		model = glm::mat4();

		model = glm::translate(model, lamp_pos);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(lamp_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		lamp.draw(nanosuit_shader);

		// reflective nanosuit
		nanosuit_reflection_shader.use();

		model = glm::mat4();

		model = glm::translate(model, reflective_nanosuit_pos);
		model = glm::scale(model, glm::vec3(0.2f));

		glUniformMatrix4fv(glGetUniformLocation(nanosuit_reflection_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(nanosuit_reflection_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(nanosuit_reflection_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(nanosuit_reflection_shader.ID, "cameraPos"), 1, glm::value_ptr(Camera::pos));

		glActiveTexture(GL_TEXTURE3);
		cube_shader.set_int("skybox", 3);
		skybox_cubemap.bind();

		reflective_nanosuit.draw(nanosuit_reflection_shader);

		glDisable(GL_MULTISAMPLE);

		model = glm::mat4();
		model = glm::translate(model, glm::vec3(reflective_nanosuit_pos.x + 3, reflective_nanosuit_pos.y, reflective_nanosuit_pos.z));
		model = glm::scale(model, glm::vec3(0.2f));

		glUniformMatrix4fv(glGetUniformLocation(nanosuit_reflection_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		reflective_nanosuit.draw(nanosuit_reflection_shader);

		glEnable(GL_MULTISAMPLE);

		// grass
		glDisable(GL_CULL_FACE);

		grass_shader.use();

		glUniformMatrix4fv(glGetUniformLocation(grass_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(grass_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);

		glBindVertexArray(grass_vao);
		glActiveTexture(GL_TEXTURE0);
		grass_texture.bind();

		for (unsigned int i = 0; i < grass_pos.size(); i++) {
			model = glm::mat4();
			model = glm::translate(model, grass_pos.at(i));

			glUniformMatrix4fv(glGetUniformLocation(grass_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		// points
		/*
		points_shader.use();

		glBindVertexArray(points_vao);
		glDrawArrays(GL_POINTS, 0, 4);
		*/

		// window
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		window_shader.use();

		glBindVertexArray(grass_vao);
		glActiveTexture(GL_TEXTURE0);
		window_texture.bind();

		glUniformMatrix4fv(glGetUniformLocation(window_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(window_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);

		/*
		In order to prevent the window fragments behind other windows from being discarded by the depth
		test, we must sort the windows by their distance from the camera then draw them from
		farthest to closest
		*/
		std::map<float, glm::vec3> sorted;

		for (unsigned int i = 0; i < window_pos.size(); i++) {
			float distance = glm::length(Camera::pos - window_pos.at(i));
			sorted[distance] = window_pos.at(i);
		}

		for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
			model = glm::mat4();

			model = glm::translate(model, it->second);

			glUniformMatrix4fv(glGetUniformLocation(window_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}

		glDisable(GL_BLEND);

		// reflective cube
		cube_shader.use();

		model = glm::mat4();
		model = glm::translate(model, cube_pos);

		glUniformMatrix4fv(glGetUniformLocation(cube_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(cube_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(cube_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(glGetUniformLocation(cube_shader.ID, "cameraPos"), 1, glm::value_ptr(Camera::pos));

		glBindVertexArray(cube_vao);
		skybox_cubemap.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// draw the skybox last
		glDepthFunc(GL_LEQUAL);

		skybox_shader.use();

		glUniformMatrix4fv(glGetUniformLocation(skybox_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(skybox_shader.ID, "view"), 1, GL_FALSE, &skybox_view[0][0]);

		glBindVertexArray(skybox_vao);
		glActiveTexture(GL_TEXTURE0);
		skybox_cubemap.bind();
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthFunc(GL_LESS);

		/*
		Makes sure that we're only drawing parts of the containers that are not equal to 1,
		thus only draw the part of the containers that are outside the previously drawn containers.

		Needs to be drawn last otherwise it only shows up behind other models, for some reason
		*/
		glEnable(GL_CULL_FACE);
		glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
		glStencilMask(0x00);
		glDisable(GL_DEPTH_TEST);

		border_shader.use();

		model = glm::mat4();

		model = glm::translate(model, nanosuit_pos);
		model = glm::scale(model, glm::vec3(0.21f, 0.21f, 0.21f));

		glUniformMatrix4fv(glGetUniformLocation(border_shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
		glUniformMatrix4fv(glGetUniformLocation(border_shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(border_shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

		nanosuit.draw(border_shader);

		glEnable(GL_DEPTH_TEST);
		glStencilMask(0xFF);

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
	glDeleteVertexArrays(1, &grass_vao);
	glDeleteVertexArrays(1, &skybox_vao);
	glDeleteVertexArrays(1, &quad_vao);
	glDeleteVertexArrays(1, &points_vao);
	glDeleteBuffers(1, &grass_vbo);
	glDeleteBuffers(1, &skybox_vbo);
	glDeleteBuffers(1, &quad_vbo);
	glDeleteBuffers(1, &points_vbo);

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