#include "shader.h"

/*
1. retrieve the vertex/fragment source code from filepath

2. compile the shaders
*/
Shader::Shader(const GLchar* vertex_path, const GLchar* fragment_path) {
	unsigned int vertex_shader = this->create_shader(vertex_path, GL_VERTEX_SHADER);
	unsigned int fragment_shader = this->create_shader(fragment_path, GL_FRAGMENT_SHADER);

	// shader program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex_shader);
	glAttachShader(this->ID, fragment_shader);
	glLinkProgram(this->ID);

	int success;
	char info_log[512];

	// print linking errors if they exist
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(this->ID, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// the shaders are already linked to our program and are no longer necessary so we delete them
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
}

Shader::Shader(const GLchar* vertex_path, const GLchar* fragment_path, const GLchar* geometry_path) {
	unsigned int vertex_shader = create_shader(vertex_path, GL_VERTEX_SHADER);
	unsigned int fragment_shader = create_shader(fragment_path, GL_FRAGMENT_SHADER);
	unsigned int geometry_shader = create_shader(geometry_path, GL_GEOMETRY_SHADER);

	this->ID = glCreateProgram();
	glAttachShader(this->ID, vertex_shader);
	glAttachShader(this->ID, fragment_shader);
	glAttachShader(this->ID, geometry_shader);
	glLinkProgram(this->ID);

	int success;
	char info_log[512];

	// print linking errors, if they exist
	glGetProgramiv(this->ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->ID, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
	}

	// the shaders are already linked to our program and are no longer necessary so we delete them
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteShader(geometry_shader);
}

unsigned int Shader::create_shader(const GLchar* path, GLenum type) {
	unsigned int reference;
	std::string code;
	std::ifstream file;

	// ensure the ifstream object can throw exceptions
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// open file
		file.open(path);

		// read file's buffer content into stream
		std::stringstream stream;

		stream << file.rdbuf();

		// close file handler
		file.close();

		// convert the file's content into a string
		code = stream.str();
	} catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}

	const char* c_code = code.c_str();

	// check for compile errors
	int success;
	char info_log[512];

	reference = glCreateShader(type);
	glShaderSource(reference, 1, &c_code, NULL);
	glCompileShader(reference);

	glGetShaderiv(reference, GL_COMPILE_STATUS, &success);
	if (!success) {
		std::string str;

		switch (type) {
		case GL_VERTEX_SHADER:
			str = "VERTEX";
			break;
		case GL_FRAGMENT_SHADER:
			str = "FRAGMENT";
			break;
		case GL_GEOMETRY_SHADER:
			str = "GEOMETRY";
			break;
		}

		glGetShaderInfoLog(reference, 512, NULL, info_log);
		std::cout << "ERROR::SHADER::" << str << "::COMPILATION_FAILED\n" << info_log << std::endl;
	}

	return reference;
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::set_bool(const std::string &name, bool value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::set_int(const std::string &name, int value) const {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::set_float(const std::string &name, float value) const {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}