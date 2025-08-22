#include "texture.hpp"

#include <glad/glad.h>

Texture::Texture(int width, int height, unsigned char* data) :
	m_width{width},
	m_height{height}
{
	glGenTextures(1, &m_id);
	glBindTexture(GL_TEXTURE_2D, m_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(Texture&& texture) noexcept :
	m_id{texture.m_id},
	m_width{texture.m_width},
	m_height{texture.m_height}
{
	texture.m_invalid = true;
}

Texture::~Texture()
{
	if (!m_invalid)
	{
		glDeleteTextures(1, &m_id);
	}
}

Texture& Texture::operator=(Texture&& texture) noexcept
{
	m_id = texture.m_id;
	m_width = texture.m_width;
	m_height = texture.m_height;
	texture.m_invalid = true;
	return *this;
}

unsigned int Texture::getId() const
{
	return m_id;
}
