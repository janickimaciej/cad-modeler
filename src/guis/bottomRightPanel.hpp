#pragma once

#include <glm/glm.hpp>

class BottomRightPanel
{
public:
	static constexpr int width = 250;

	BottomRightPanel(const glm::ivec2& windowSize);
	void update();

private:
	const glm::ivec2& m_windowSize;
};
