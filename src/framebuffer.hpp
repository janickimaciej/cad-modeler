#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <array>

template <typename T>
class Framebuffer
{
public:
	Framebuffer(GLenum type, GLint internalFormat, const glm::ivec2& size);
	~Framebuffer();
	void bind();
	void bind(const glm::ivec2& viewportOffset, const glm::ivec2& viewportSize);
	void unbind() const;
	void bindTexture() const;
	void resize(const glm::ivec2& size);
	void getTextureData(T* output) const;

private:
	GLenum m_type{};
	GLint m_internalFormat{};

	unsigned int m_FBO{};
	unsigned int m_colorBuffer{};
	unsigned int m_depthStencilBuffer{};
	glm::ivec2 m_size{};
	std::array<int, 4> m_previousViewport{};

	void createColorBuffer();
	void createDepthStencilBuffer();

	void resizeColorBuffer() const;
	void resizeDepthStencilBuffer() const;
};

template <typename T>
Framebuffer<T>::Framebuffer(GLenum type, GLint internalFormat, const glm::ivec2& size) :
	m_type{type},
	m_internalFormat{internalFormat},
	m_size{size}
{
	glGenFramebuffers(1, &m_FBO);
	bind();

	createColorBuffer();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	createDepthStencilBuffer();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
		m_depthStencilBuffer);

	unbind();
}

template <typename T>
Framebuffer<T>::~Framebuffer()
{
	glDeleteRenderbuffers(1, &m_depthStencilBuffer);
	glDeleteTextures(1, &m_colorBuffer);
	glDeleteFramebuffers(1, &m_FBO);
}

template <typename T>
void Framebuffer<T>::bind()
{
	bind({0, 0}, m_size);
}

template <typename T>
void Framebuffer<T>::bind(const glm::ivec2& viewportOffset, const glm::ivec2& viewportSize)
{
	glGetIntegerv(GL_VIEWPORT, m_previousViewport.data());
	glViewport(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

template <typename T>
void Framebuffer<T>::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(m_previousViewport[0], m_previousViewport[1], m_previousViewport[2],
		m_previousViewport[3]);
}

template <typename T>
void Framebuffer<T>::bindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
}

template <typename T>
void Framebuffer<T>::resize(const glm::ivec2& size)
{
	m_size = size;
	resizeColorBuffer();
	resizeDepthStencilBuffer();
}

template <typename T>
void Framebuffer<T>::getTextureData(T* output) const
{
	glReadPixels(0, 0, m_size.x, m_size.y, GL_RGB, m_type, output);
}

template <typename T>
void Framebuffer<T>::createColorBuffer()
{
	glGenTextures(1, &m_colorBuffer);
	resizeColorBuffer();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {0, 0, 0, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
}

template <typename T>
void Framebuffer<T>::createDepthStencilBuffer()
{
	glGenRenderbuffers(1, &m_depthStencilBuffer);
	resizeDepthStencilBuffer();
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

template <typename T>
void Framebuffer<T>::resizeColorBuffer() const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, m_internalFormat, m_size.x, m_size.y, 0, GL_RGB, m_type,
		nullptr);
}

template <typename T>
void Framebuffer<T>::resizeDepthStencilBuffer() const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_size.x, m_size.y);
}
