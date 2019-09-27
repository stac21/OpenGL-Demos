#include <glad/glad.h>
#include <glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

float mixValue = 0.2f;

constexpr int window_width = 800;
constexpr int window_height = 600;

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Transformations", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initlalize GLAD" << std::endl;
		return -1;
	}

	float vertices[] = {
		// positions		// colors			// texture coords
		0.5f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0f, 1.0f,		// top right
		0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,		 // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f,		// bottom left
		-0.5f, 0.5f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0f		// top left
	};
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};

	Shader shader("C:\\Users\\Grant\\LearnOpenGL Files\\Transformations\\vertexShader.txt", "C:\\Users\\Grant\\LearnOpenGL Files\\Transformations\\fragmentShader.txt");

	// load the image of the container and assign the widht, height, and num_channels to those of the image
	int width, height, num_channels;
	unsigned char* data1 = stbi_load("C:\\Users\\Grant\\LearnOpenGL Files\\Transformations\\container.jpg", &width, &height, &num_channels, 0);

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
		operation will generate a texture on teh currently bound texture object at the same target
		(so any textures bound to targets GL_TEXTURE_1D or GL_TEXTURE_1D will not be affected

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
	unsigned char* data2 = stbi_load("C:\\Users\\Grant\\LearnOpenGL Files\\Transformations\\awesomeface.png", &width, &height, &num_channels, 0);

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
	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// bind the VAO first, then bind and set vertex buffer(s), then congigure vertex attribute(s)
	glBindVertexArray(VAO);

	// bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	/*
	Vertex attributes are disabled by default so we have to enable it here. The argument is the
	shader's position (0 in this case)
	*/
	glEnableVertexAttribArray(0);

	// color attribute
	/*
	The last argument, the offset, is 3 times  sizeof(float) because the position values come before
	the color values
	*/
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	/*
	Tell OpenGL which texture unit each shader sampler belongs to by setting each sampler
	using glUniform1i. Only needs to be set once so we can do this before we enter the render loop
	*/
	shader.use();
	glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);	// set it manually
	shader.set_int("texture2", 1);									// set it with the function in the shader class (no difference in outcome, just done different ways for example)

	while (!glfwWindowShouldClose(window)) {
		// input
		process_input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

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
		
		glm::mat4 trans;
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		shader.use(); // be sure to activate the shader before any calls to glUniform
		shader.set_float("mixValue", mixValue);
		
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		trans=glm::mat4();
		trans = glm::translate(trans, glm::vec3(-0.5f, 0.5f, 0.0f));
		trans = glm::scale(trans, (float)sin(glfwGetTime()) * glm::vec3(1.0f, 1.0f, 1.0f));

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved, etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	} else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		if (mixValue < 1.0f) {
			mixValue += 0.01f;
		}
	} else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		if (mixValue > 0.0f) {
			mixValue -= 0.01f;
		}
	}
}