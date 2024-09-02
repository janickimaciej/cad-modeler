#pragma once

#include "guis/valueWindows/valueWindow.hpp"
#include "scene.hpp"

#include <array>
#include <string>

#include <glm/glm.hpp>

class SerializingWindow : public ValueWindow
{
public:
	SerializingWindow(Scene& scene, const glm::ivec2& windowSize);
	virtual ~SerializingWindow() = default;
	void startSerializing();
	void apply();

private:
	Scene& m_scene;

	static constexpr int maxPathLength = 128;
	std::array<char, maxPathLength> m_path{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
