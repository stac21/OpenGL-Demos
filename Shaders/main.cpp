#include <glad/glad.h>
#include <glfw3.h>

#include <iostream>
#include <cmath>

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);
constexpr int window_width = 800;
constexpr int window_height = 600;

/*
Modern OpenGL requires that we at least set up a vertex and fragment shader if we want to do
some rendering
*/
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Triangle", NULL, NULL);
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

	Shader shader("C:\\Users\\Grant\\LearnOpenGL Files\\vertexShader.txt", "C:\\Users\\Grant\\LearnOpenGL Files\\fragmentShader.txt");

	float vertices[] = {	// colors
		0.5f, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
		0.0f,  0.5f, 0.0f,	0.0f, 0.0f, 1.0f   // top 
	};
	/*
	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 3,  // first Triangle
		1, 2, 3   // second Triangle
	};
	*/

	// Generatea vertex buffer with a buffer ID
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	// bind the VAO first, then bind and set vertex buffer(s), then congigure vertex attribute(s)
	glBindVertexArray(VAO);

	// bind the newly created buffer to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
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
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(0);

	shader.use();

	while (!glfwWindowShouldClose(window)) {
		// input
		process_input(window);

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// output (draw the triangle)
		shader.use(); // be sure to activate the shader before any calls to glUniform
		glBindVertexArray(VAO);

		/*
		float time_value = glfwGetTime();
		float green_value = sin(time_value) / 2.0f + 0.5f;
		int vertex_color_location = glGetUniformLocation(shader_program, "ourColor");
		glUniform4f(vertex_color_location, 0.0f, green_value, 0.0f, 1.0f);
		*/

		
		glDrawArrays(GL_TRIANGLES, 0, 3);
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
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}