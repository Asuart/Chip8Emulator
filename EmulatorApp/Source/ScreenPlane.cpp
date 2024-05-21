#include "ScreenPlane.h"

ScreenPlane::ScreenPlane() {
	const float vertexes[] = {
		-1.0, -1.0,
		-1.0, 1.0,
		1.0, 1.0,
		-1.0, -1.0,
		1.0, 1.0,
		1.0, -1.0
	};
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, vertexes, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader = ShaderLibrary::CreateShaderProgramm(
		"#version 330 core\n"
		"layout(location = 0) in vec2 position;\n"
		"out vec2 fTexCoord;\n"
		"uniform float aspect;"
		"void main() {\n"
		"	float xScale = 1.0;\n"
		"	float yScale = 1.0;\n"
		"	if(aspect > 1.0f) {\n"
		"		yScale /= aspect;"
		"	} else {\n"
		"		xScale *= aspect;"
		"	}\n"
		"   fTexCoord = vec2(position.x + 1.0, 1.0 - position.y) / 2.0;\n"
		"	vec2 transformedPosition = vec2(position.x * xScale, position.y * yScale);"
		"   gl_Position = vec4(transformedPosition,0.0, 1.0);\n"
		"}"
		, 
		"#version 330 core\n"
		"out vec4 color;"
		"uniform sampler2D ourTexture;"
		"in vec2 fTexCoord;"
		"void main() {"
		"color = vec4(texture(ourTexture, fTexCoord).rrr, 1.0);"
		"}\n"
	);
}