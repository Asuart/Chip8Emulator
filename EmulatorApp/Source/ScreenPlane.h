#pragma once
#include <GL/glew.h>
#include "ShaderLibrary.h"

class ScreenPlane {
public:
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint shader = 0;

	ScreenPlane();
};

