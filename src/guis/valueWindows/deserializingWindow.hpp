#pragma once

#include "guis/valueWindows/valueWindow.hpp"
#include "scene.hpp"

#include <array>
#include <string>

#include <glm/glm.hpp>

class DeserializingWindow : public ValueWindow
{
public:
	DeserializingWindow(Scene& scene, const glm::ivec2& viewportSize);
	virtual ~DeserializingWindow() = default;
	void startDeserializing();
	void apply();

private:
	Scene& m_scene;

	static constexpr int maxPathLength = 128;
	std::array<char, maxPathLength> m_path{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
