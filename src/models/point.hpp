#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "guis/modelGUIs/pointGUI.hpp"
#include "models/model.hpp"
#include "renderMode.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <vector>

class Point : public Model
{
public:
	using Callback = std::function<void(Point*)>;

	Point(const ShaderProgram& pointShaderProgram, glm::vec3 position, bool isVirtual = false);
	~Point();

	virtual void render(RenderMode renderMode) const override;
	virtual void updateGUI() override;

	virtual void setPosition(const glm::vec3& position) override;
	virtual void setScreenPosition(const glm::vec2& screenPosition, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize) override;

	std::shared_ptr<Callback> registerForMoveNotification(const Callback& callback);
	std::shared_ptr<Callback> registerForDestroyNotification(const Callback& callback);

	bool isReferenced();

private:
	static int m_count;
	int m_id{};

	const ShaderProgram& m_pointShaderProgram;
	PointGUI m_gui;

	unsigned int m_VAO{};

	std::vector<std::weak_ptr<Callback>> m_moveNotifications{};
	std::vector<std::weak_ptr<Callback>> m_destroyNotifications{};

	virtual void updateShaders(RenderMode) const override;

	void notify(std::vector<std::weak_ptr<Callback>>& notifications);
	void clearExpiredNotifications();
};
