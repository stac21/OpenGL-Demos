#include <glad/glad.h>
#include <glfw3.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void process_input(GLFWwindow* window);

int main() {
	constexpr int WINDOW_WIDTH = 800;
	constexpr int WINDOW_HEIGHT = 600;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	/*
	Create a window object. If the window is null, exit the program and give an error message.
	*/
	GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;

		glfwTerminate();

		return -1;
	}

	/*
	Make the context of the window the main context on the current thread
	*/
	glfwMakeContextCurrent(window);

	/*
	initialize GLAD before we calll any OpenGL function because GLAD manages function pointers
	for OpenGL.

	We pass GLAD the function to load the address of the OpenGL function pointers which is OS-specific.
	GLFW gives glfwGetProcAdress which gives us the correct function based on the OS we're compiling
	for.
	*/
	if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;

		return -1;
	}

	/*
	Tell OpenGL the size of the rendering window so OpenGL knows how we want to display the data
	and coordinates with respect to the window.

	First and second arguments set the location of the lower left corner of the window.
	Third and fourth arguments set the width and height of the rendering window in pixels.

	Note: Behind the scenes OpenGL uses the data specified via glViewport to transform the 2D 
	coordinates it processed to coordinates on your screen. For example, a processed point of location 
	(-0.5,0.5) would (as its final transformation) be mapped to (200,450) in screen coordinates. 
	Note that processed coordinates in OpenGL are between -1 and 1 so we effectively map from the 
	range (-1 to 1) to (0, 800) and (0, 600). 
	*/
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	/*
	Register the automatic window resize function to GLFW
	*/
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	/*
	Standard single threaded game loop
	*/
	while (!glfwWindowShouldClose(window)) {
		// input
		process_input(window);

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		/*
		Double buffer
		When an application draws in a single buffer the resulting image might display flickering issues. 
		This is because the resulting output image is not drawn in an instant, but drawn pixel by pixel and
		usually from left to right and top to bottom. Because these images are not displayed at an instant to
		the user, but rather via a step by step generation the result may contain quite a few artifacts. To 
		circumvent these issues, windowing applications apply a double buffer for rendering. The front buffer 
		contains the final output image that is shown at the screen, while all the rendering commands draw to 
		the back buffer. As soon as all the rendering commands are finished we swap the back buffer to the 
		front buffer so the image is instantly displayed to the user, removing all the aforementioned 
		artifacts. */
		glfwSwapBuffers(window);
		/*
		checks if any events are triggered such as keyboard or mouse input, updates the window state,
		and calls the corresponding functions which can be set using callback functions
		*/
		glfwPollEvents();
	}

	/*
	Clean up all the resources and properly exit the application
	*/
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