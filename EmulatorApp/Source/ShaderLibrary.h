#pragma once
#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class ShaderLibrary {
public:
	static GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
	static GLuint CreateShaderProgramm(const std::string& vsSource, const std::string& fsSource);
	static GLuint LoadComputeShader(const char* path);
};

