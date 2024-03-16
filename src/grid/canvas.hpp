#pragma once

class Canvas
{
public:
	Canvas();
	void render() const;

private:
	unsigned int m_VBO{};
	unsigned int m_VAO{};
};
