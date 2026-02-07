#pragma once

#include "guis/guiMode.hpp"
#include "guis/valueWindows/valueWindow.hpp"
#include "models/Model.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

#include <string>

class RotatingWindow : public ValueWindow
{
public:
	RotatingWindow(Scene& scene, const glm::ivec2& viewportSize);
	virtual ~RotatingWindow() = default;
	void startRotating();
	void apply(GUIMode mode);

private:
	Scene& m_scene;

	float m_rotationDeg{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
