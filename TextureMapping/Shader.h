#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

#include <GL/glew.h>

class Shader
{
public:
	GLuint Program;
	// Constructor generates the shader on the fly
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader::Shader(const GLchar* computePath);
	// Uses the current shader
	void Use() { glUseProgram(this->Program); }
	void Disable() { glUseProgram(0); }
};