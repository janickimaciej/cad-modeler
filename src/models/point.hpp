#pragma once

#include "guis/model_guis/model_gui.hpp"
#include "guis/model_guis/point_gui.hpp"
#include "models/model.hpp"
#include "render_mode.hpp"
#include "scene.hpp"
#include "shader_program.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <vector>

class Point : public Model
{
public:
	using Callback = std::function<void(Point*)>;

	Point(const Scene& scene, const ShaderProgram& wireframePointShaderProgram,
		const ShaderProgram& solidPointShaderProgram, glm::vec3 position);
	~Point();
	virtual void render(RenderMode renderMode) const override;
	virtual ModelGUI& getGUI() override;

	virtual void setPosition(const glm::vec3& position) override;
	virtual void setScreenPosition(const glm::vec2& screenPosition) override;

	std::shared_ptr<Callback> registerForMoveNotification(const Callback& callback);
	std::shared_ptr<Callback> registerForDestroyNotification(const Callback& callback);

private:
	static int m_count;
	int m_id{};

	const ShaderProgram& m_wireframePointShaderProgram;
	const ShaderProgram& m_solidPointShaderProgram;
	PointGUI m_gui;

	unsigned int m_VAO{};

	std::vector<std::weak_ptr<Callback>> m_moveNotifications{};
	std::vector<std::weak_ptr<Callback>> m_destroyNotifications{};

	virtual void updateShaders(RenderMode renderMode) const override;

	void notify(std::vector<std::weak_ptr<Callback>>& notifications);
};
