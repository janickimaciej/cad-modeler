#pragma once

#include "guis/guiMode.hpp"
#include "guis/valueWindows/valueWindow.hpp"
#include "models/Model.hpp"
#include "scene.hpp"

#include <glm/glm.hpp>

#include <string>

class ScalingWindow : public ValueWindow
{
public:
	ScalingWindow(Scene& scene, const glm::ivec2& viewportSize);
	virtual ~ScalingWindow() = default;
	void startScaling();
	void apply(GUIMode mode);

private:
	Scene& m_scene;

	float m_scale{};

	virtual std::string name() override;
	virtual int width() override;
	virtual void input() override;
};
