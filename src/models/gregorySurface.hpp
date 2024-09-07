#pragma once

#include "models/model.hpp"
#include "models/point.hpp"
#include "shaderProgram.hpp"

#include <array>
#include <functional>
#include <memory>
#include <vector>

class GregorySurface : public Model
{
public:
	using SelfDestructCallback = std::function<void(const GregorySurface*)>;

	GregorySurface(const ShaderProgram& surfaceShaderProgram,
		const std::array<std::array<std::array<Point*, 4>, 2>, 3>& bezierPoints,
		const SelfDestructCallback& selfDestructCallback);
	virtual ~GregorySurface() = default;
	virtual void render() const override;
	virtual void updateGUI() override;

private:
	static int m_count;

	const ShaderProgram& m_surfaceShaderProgram;
	std::array<std::array<std::array<Point*, 4>, 2>, 3> m_bezierPoints{};

	std::vector<std::shared_ptr<Point::MoveCallback>> m_pointMoveNotifications{};
	std::vector<std::shared_ptr<Point::DestroyCallback>> m_pointDestroyNotifications{};
	std::vector<std::shared_ptr<Point::RereferenceCallback>> m_pointRereferenceNotifications{};

	SelfDestructCallback m_selfDestructCallback;
	
	virtual void updateShaders() const override;

	void registerForNotifications();
	void registerForNotifications(Point* point);
	void pointMoveNotification();
	void pointDestroyNotification(Point::DestroyCallback* notification);
	void pointRereferenceNotification(Point::RereferenceCallback* notification, Point* newPoint);

	void updateGeometry();
};
