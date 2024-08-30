#include "framebuffer.hpp"

#include <glad/glad.h>

Framebuffer::Framebuffer(const glm::ivec2& windowSize)
{
	glGenFramebuffers(1, &m_FBO);
	bind();

	createColorBuffer(windowSize);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorBuffer, 0);

	createDepthStencilBuffer(windowSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
		m_depthStencilBuffer);

	unbind();
}

Framebuffer::~Framebuffer()
{
	glDeleteRenderbuffers(1, &m_depthStencilBuffer);
	glDeleteTextures(1, &m_colorBuffer);
	glDeleteFramebuffers(1, &m_FBO);
}

void Framebuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
}

void Framebuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::bindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
}

void Framebuffer::resize(const glm::ivec2& windowSize) const
{
	resizeColorBuffer(windowSize);
	resizeDepthStencilBuffer(windowSize);
}

void Framebuffer::createColorBuffer(const glm::ivec2& windowSize)
{
	glGenTextures(1, &m_colorBuffer);
	resizeColorBuffer(windowSize);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Framebuffer::createDepthStencilBuffer(const glm::ivec2& windowSize)
{
	glGenRenderbuffers(1, &m_depthStencilBuffer);
	resizeDepthStencilBuffer(windowSize);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::resizeColorBuffer(const glm::ivec2& windowSize) const
{
	glBindTexture(GL_TEXTURE_2D, m_colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowSize.x, windowSize.y, 0, GL_RGB, GL_UNSIGNED_BYTE,
		nullptr);
}

void Framebuffer::resizeDepthStencilBuffer(const glm::ivec2& windowSize) const
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.x, windowSize.y);
}
