#pragma once

#include "guis/valueWindows/valueWindow.hpp"
#include "models/Model.hpp"
#include "scene.hpp"

#include <array>
#include <string>

#include <glm/glm.hpp>

class RenamingWindow : public ValueWindow
{
public:
	RenamingWindow(Scene& scene, const glm::ivec2& windowSize);
	bool startRenaming();
	void cancel();
	void apply();

private:
	Scene& m_scene;

	Model* m_uniqueActiveModel = nullptr;
	static constexpr int maxNameLength = 32;
	std::array<char, maxNameLength> m_name{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
