#pragma once

class Texture
{
public:
	Texture(int width, int height, unsigned char* data);
	Texture(const Texture&) = delete;
	Texture(Texture&& texture) noexcept;
	~Texture();

	Texture& operator=(const Texture&) = delete;
	Texture& operator=(Texture&& texture) noexcept;

	unsigned int getId() const;

private:
	bool m_invalid = false;

	unsigned int m_id;
	int m_width{};
	int m_height{};
};
