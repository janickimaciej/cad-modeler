#pragma once

#include "guis/modelGUIs/modelGUI.hpp"
#include "guis/modelGUIs/pointGUI.hpp"
#include "meshes/pointMesh.hpp"
#include "models/model.hpp"
#include "shaderProgram.hpp"

#include <glm/glm.hpp>

#include <functional>
#include <memory>
#include <vector>

class Point : public Model
{
public:
	using Callback = std::function<void(Point*)>;

	Point(const ShaderProgram& shaderProgram, glm::vec3 pos, bool isVirtual = false);
	~Point();

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3& pos) override;
	virtual void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize) override;

	std::shared_ptr<Callback> registerForMoveNotification(const Callback& callback);
	std::shared_ptr<Callback> registerForDestroyNotification(const Callback& callback);

	bool isReferenced();

private:
	static int m_nonVirtualCount;
	static int m_virtualCount;

	const ShaderProgram& m_shaderProgram;
	PointGUI m_gui;

	PointMesh m_mesh{};

	std::vector<std::weak_ptr<Callback>> m_moveNotifications{};
	std::vector<std::weak_ptr<Callback>> m_destroyNotifications{};

	virtual void updateShaders() const override;

	void notify(std::vector<std::weak_ptr<Callback>>& notifications);
	void clearExpiredNotifications();
};
