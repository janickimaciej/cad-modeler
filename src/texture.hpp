#pragma once

#include <glm/glm.hpp>

class Texture
{
public:
	Texture(const glm::ivec2& size, unsigned char* data);
	Texture(const Texture&) = delete;
	Texture(Texture&& texture) noexcept;
	~Texture();

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& texture) noexcept;

	unsigned int getId() const;

private:
	bool m_invalid = false;

	unsigned int m_id{};
	glm::ivec2 m_size{};
};
