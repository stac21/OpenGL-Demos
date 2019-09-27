#pragma once

#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
private:
	unsigned int create_shader(const GLchar* path, GLenum type);

public:
	// the program ID
	unsigned int ID;

	// constructor builds and rebuilds the shader
	Shader(const GLchar* vertex_path, const GLchar* fragment_path);
	Shader(const GLchar* vertex_path, const GLchar* fragment_path, const GLchar* geometry_path);
	// use/activate the shader
	void use();
	// utility uniform functions
	void set_bool(const std::string &name, bool value) const;
	void set_int(const std::string &name, int value) const;
	void set_float(const std::string &name, float value) const;
};