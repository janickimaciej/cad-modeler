#pragma once

#include "guis/valueWindows/valueWindow.hpp"
#include "scene.hpp"

#include <array>
#include <string>

#include <glm/glm.hpp>

class RenamingWindow : public ValueWindow
{
public:
	RenamingWindow(Scene& scene, const glm::ivec2& viewportSize);
	virtual ~RenamingWindow() = default;

	void startRenaming();
	void apply();

private:
	Scene& m_scene;

	static constexpr int maxNameLength = 32;
	std::array<char, maxNameLength> m_name{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
