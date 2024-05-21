#include "TextureUploader.h"

template <>
void TextureUploader<uint8_t>::TexImage2D() {
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB32F,
		texture.width, texture.height,
		0, GL_RED, GL_UNSIGNED_BYTE,
		&texture.pixels[0]
	);
}