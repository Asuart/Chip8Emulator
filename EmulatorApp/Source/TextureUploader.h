#pragma once
#include <GL/glew.h>
#include <iostream>
#include "Texture.h"

template <typename T>
class TextureUploader {
public:
	TextureUploader(const Texture<T>& _texture)
		: texture(_texture) {}

	void Upload() {
		if (id == 0) {
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		glBindTexture(GL_TEXTURE_2D, id);
		TexImage2D();
	}

	void Bind(GLuint activeTexture = GL_TEXTURE0) {
		if (id == 0) {
			std::cout << "Binding not loaded texture.\n";
			return;
		}
		glActiveTexture(activeTexture);
		glBindTexture(GL_TEXTURE_2D, id);
	}

private:
	const Texture<T>& texture;
	GLuint id = 0;

	void TexImage2D();
};

