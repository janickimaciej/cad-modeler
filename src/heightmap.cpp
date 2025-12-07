#include "heightmap.hpp"

#include <glad/glad.h>

Heightmap::Heightmap(const glm::ivec2& size) :
	m_size{size}
{
	glGenFramebuffers(1, &m_FBO);
	bind();

	createColorBuffer(size);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	createDepthStencilBuffer(size);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
		m_depthStencilBuffer);

	unbind();
}

Heightmap::~Heightmap()
{
	glDeleteRenderbuffers(1, &m_depthStencilBuffer);
	glDeleteTextures(1, &m_colorBuffer);
	glDeleteFramebuffers(1, &m_FBO);
}

void Heightmap::bind()
{
	bind({0, 0}, m_size);
}

void Heightmap::bind(const glm::ivec2& viewportOffset, const glm::ivec2& viewportSize)
{
	glGetIntegerv(GL_VIEWPORT, m_previousViewport.data());
	glViewport(viewportOffset.x, viewportOffset.y, viewportSize.x, viewportSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Heightmap::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(m_previousViewport[0], m_previousViewport[1], m_previousViewport[2],
		m_previousViewport[3]);
}

void Heightmap::bindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
}

void Heightmap::resize(const glm::ivec2& size) const
{
	resizeColorBuffer(size);
	resizeDepthStencilBuffer(size);
}

void Heightmap::getTextureData(float* output) const
{
	glReadPixels(0, 0, m_size.x, m_size.y, GL_RGB, GL_FLOAT, output);
}

void Heightmap::createColorBuffer(const glm::ivec2& size)
{
	glGenTextures(1, &m_colorBuffer);
	resizeColorBuffer(size);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = {0, 0, 0, 1};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Heightmap::createDepthStencilBuffer(const glm::ivec2& size)
{
	glGenRenderbuffers(1, &m_depthStencilBuffer);
	resizeDepthStencilBuffer(size);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Heightmap::resizeColorBuffer(const glm::ivec2& size) const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, size.x, size.y, 0, GL_RGB, GL_FLOAT, nullptr);
}

void Heightmap::resizeDepthStencilBuffer(const glm::ivec2& size) const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
}
