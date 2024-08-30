#pragma once

#include <glm/glm.hpp>

class Framebuffer
{
public:
	Framebuffer(const glm::ivec2& windowSize);
	~Framebuffer();
	void bind() const;
	void unbind() const;
	void bindTexture() const;
	void resize(const glm::ivec2& windowSize) const;

private:
	unsigned int m_FBO{};
	unsigned int m_colorBuffer{};
	unsigned int m_depthStencilBuffer{};

	void createColorBuffer(const glm::ivec2& windowSize);
	void createDepthStencilBuffer(const glm::ivec2& windowSize);

	void resizeColorBuffer(const glm::ivec2& windowSize) const;
	void resizeDepthStencilBuffer(const glm::ivec2& windowSize) const;
};
