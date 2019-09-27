#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset);
void process_input(GLFWwindow* window);
float get_delta_time();

int window_width = 800;
int window_height = 600;

constexpr float PLANE_DISTANCE = 100.0f;

float mixValue = 0.2f;

constexpr float camera_speed = 2.5f;
float sensitivity = 0.05f;

glm::vec3 camera_pos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

float current_frame_time = 0.0f;
float last_frame_time = 0.0f;

float last_x, last_y;

float pitch = 0.0f;
/*
yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to 
the right so we initially rotate a bit to the left
*/
float yaw = -90.0f;

float fov = 45.0f;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Coordinate Systems", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	int temp_x, temp_y;

	glfwGetFramebufferSize(window, &temp_x, &temp_y);

	last_x = (float)temp_x;
	last_y = (float)temp_y;

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initlalize GLAD" << std::endl;
		return -1;
	}

	// Hides the mouse cursor and prevents it from leaving the window
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	glm::vec3 cubePositions[] = {
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

	Shader shader("C:\\Users\\Grant\\LearnOpenGL Files\\CoordinateSystems\\vertexShader.txt", "C:\\Users\\Grant\\LearnOpenGL Files\\CoordinateSystems\\fragmentShader.txt");

	// load the image of the container and assign the widht, height, and num_channels to those of the image
	int width, height, num_channels;
	unsigned char* data1 = stbi_load("C:\\Users\\Grant\\LearnOpenGL Files\\CoordinateSystems\\container.jpg", &width, &height, &num_channels, 0);

	unsigned int texture1;
	glGenTextures(1, &texture1);	// just like any other objects in OpenGL, textures are referenced with an ID
	glBindTexture(GL_TEXTURE_2D, texture1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set the texture filtering method for downscaling then upscaling (don't know if GL_NEAREST_MIPMAP_LINEAR is good to use)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data1) {
		/*
		now that the texture is bound, we can start generating a texture using the previously loaded
		image data

		The first argument specifies the texture target; setting this to GL_TEXTURE_2D means that this
		operation will generate a texture on the currently bound texture object at the same target
		(so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_3D will not be affected

		The second argument specifies the mipmap level for which we want to create a texture for if you
		want to set each mipmap level manually, but we'll leave it at the base level which is 0

		The third argument tells OpenGL in what kind of format we want to store the texture. Our image
		has only RGB values so we'll store the texture with RGB values as well

		The 4th and 5th arguments set the width and height of the resulting texture. We stored
		those earlier when loading the image so we'll use the corresponding variables

		The next argument should always be 0 (some legacy stuff, apparently)

		The 7th and 8th arguments specify the format and datatype of the source image. We loaded the
		image with RGB values and stored them as chars (bytes) so we'll pass in the corresponding values

		The final argument is the actual image data
		*/
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	// free the image memory, since we don't need it anymore after generating the texture
	stbi_image_free(data1);

	unsigned int texture2;
	// otherwise the image of the face will be upsidedown (just how it is, yaknow)
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data2 = stbi_load("C:\\Users\\Grant\\LearnOpenGL Files\\CoordinateSystems\\awesomeface.png", &width, &height, &num_channels, 0);

	glGenTextures(1, &texture2);
	glBindTexture(GL_TEXTURE_2D, texture2);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (data2) {
		// awesomeface.png has transparency and thus an alpha channel, so the data type is GL_RGBA
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}

	// free the image memory, because we don't need it anymore after generating the texture
	stbi_image_free(data2);

	// Generatea vertex buffer with a buffer ID
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// bind the VAO first, then bind and set vertex buffer(s), then congigure vertex attribute(s)
	glBindVertexArray(VAO);

	// bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	/*
	glVertexAttribPointer tells OpenGL how it should interpret the vertex data per vertex attribute

	The first argument specifies which vertex attribute we want to configure (the vertex shader's
	position was set in the shading language with layout (location = 0)

	Second argument specifies the size of the vertex attribute. Since the vertex attribute is a
	vec3 in the shading language, it is composed of 3 values

	Third argument specifies the type of data which is GL_FLOAT, because a pointer to a vec in GLSL
	consists of floating point values

	Fourth argument specifies if we want the data to be normalized. If we set this to GL_TRUE,
	all the data that has a value not between 0 (or -1 for signed data) and 1 will be mapped to those
	values.

	Fifth argument is known as the stride and tells us the space between consecutive vertex attribute
	sets. We just multiply the size of the vertex attributes by the size of their type, in this case
	6 * sizeof(float) (6 because we have to move six floats to the right to obtain the next
	vertex attribute (three for the position values and three for the color values))

	Sixth argument is the offset of where the position data begins in the buffer
	*/
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	/*
	Vertex attributes are disabled by default so we have to enable it here. The argument is the
	shader's position (0 in this case)
	*/
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/*
	Tell OpenGL which texture unit each shader sampler belongs to by setting each sampler
	using glUniform1i. Only needs to be set once so we can do this before we enter the render loop
	*/
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);	// set it manually
	shader.set_int("texture2", 1);									// set it with the function in the shader class (no difference in outcome, just done different ways for example)

	/*
	Create the model matrix, which consists of translations, scalings, and/or rotations we'd like
	to apply to transform all object's vertices to the global world space. Here we transform
	our plane a bit by rotating it on the x-axis so it looks like it's laying on the floor

	By multiplying the vertex coordinates with this model matrix we're transforming the vertex
	coordinates to world coordinates. Our plane that is slightly on the floor thus represents
	the plane in the global world
	*/
	glm::mat4 view, proj;
	
	/*
	We want to move the camera in the positive z-direction (since we're moving backwards),
	so it is good to note that we're translating the scene in the reverse direction of where
	we want to move because the world's objects move in the opposite direction of the camera
	*/
	
	
	
	// enable depth testing, since it is disabled by OpenGL by default
	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		last_frame_time = current_frame_time;
		current_frame_time = glfwGetTime();

		// input
		process_input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		/*
		Since we're using a depth buffer we also want to clear the depth buffer before each
		render iteration (otherwise the depth information of the previous frame stays in the buffer)
		in addition to clearing the last frame's color buffer
		*/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// output (draw the shape)
		/*
		float time_value = glfwGetTime();
		float green_value = sin(time_value) / 2.0f + 0.5f;
		int vertex_color_location = glGetUniformLocation(shader_program, "ourColor");
		glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);
		*/

		// bind the textures to their corresponding texture unit
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		shader.use(); // be sure to activate the shader before any calls to glUniform
		shader.set_float("mixValue", mixValue);
		glBindVertexArray(VAO);
		
		/*
		setting the direction to cameraPos + cameraFront ensures that however we move, the camera
		keeps looking at the target direction
		*/
		view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
		/*
		create a perspective projection matrix

		First parameter determines the FOV (field of view)

		Second parameter determines the aspect ratio, so just divide the width by the height of the window

		Third and fourth parameters determine the near and far planes of the frustum
		*/
		proj = glm::perspective(glm::radians(fov), (float)window_width / (float)window_height, 0.1f, PLANE_DISTANCE);
		/*
		if we do not reset the values then the cube will continue to translate by the value
		of the variable every frame
		*/

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, 
			glm::value_ptr(proj));

		for (int i = 0; i < 10; i++) {
			glm::mat4 model;
			 
			model = glm::translate(model, cubePositions[i]);
			model = glm::rotate(model, (float)sin(glfwGetTime()) * glm::radians(50.0f), 
				glm::vec3(0.5f, 1.0f, 0.0f));

			glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, 
				glm::value_ptr(model));
			
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved, etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	window_width = width;
	window_height = height;

	glViewport(0, 0, width, height);
}

/*
When handling mouse input for an FPS style camera there are several steps we must take before
eventually retrieving the direction vector

1. Calculate the mouse's offset since the last frame
2. Add the offset values to the camera's yaw and pitch values
3. Add some constraints to the max/min yaw/pitch values
4. Calculate the direction vector
*/
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// 1. Calculate the mouse's offset since the last frame
	float x_offset = sensitivity * ((float)xpos - last_x);
	float y_offset = sensitivity * (last_y - (float)ypos); // reversed since y-coordinates range from bottom to top

	last_x = (float)xpos;
	last_y = (float)ypos;

	// 2. Add the offset values to teh camera's yaw and pitch values
	// 3. Add some constraints to the min/max yaw/pitch values
	yaw += x_offset;
	// prevents the user from going past the very top of the sky/their feet
	if (pitch < 89.0f && pitch > -89.0f) {
		pitch += y_offset;
	}

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	camera_front = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset) {
	if (fov >= 1.0f) {
		fov += y_offset;
	} else if (fov <= 60.0f) {
		fov -= y_offset;
	}
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && mixValue < 1.0f) {
		mixValue += 0.05f;
	} else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && mixValue > 0.0f) {
		mixValue -= 0.05f;
	} else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera_pos += camera_speed * get_delta_time() * camera_front;
	} else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera_pos -= camera_speed * get_delta_time() * camera_front;
	} else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera_pos -= camera_speed * get_delta_time() * glm::normalize(glm::cross(camera_front, camera_up));
	} else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera_pos += camera_speed * get_delta_time() * glm::normalize(glm::cross(camera_front, camera_up));
	} else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera_pos += camera_speed * get_delta_time() * camera_up;
	} else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera_pos -= camera_speed * get_delta_time() * camera_up;
	}
}

float get_delta_time() {
	return current_frame_time - last_frame_time;
}