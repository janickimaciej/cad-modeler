#include "models/point.hpp"

#include <array>
#include <string>

Point::Point(const ShaderProgram& shaderProgram, glm::vec3 pos, bool isVirtual) :
	Model{pos, isVirtual ? "VirtualPoint " + std::to_string(m_virtualCount++) :
		"Point " + std::to_string(m_nonVirtualCount++), isVirtual},
	m_shaderProgram{shaderProgram},
	m_gui{*this}
{ }

Point::~Point()
{
	notify(m_destroyNotifications);
}

void Point::render() const
{
	updateShaders();
	m_mesh.render();
}

void Point::updateGUI()
{
	m_gui.update();
}

void Point::setPos(const glm::vec3& pos)
{
	if (pos != m_pos)
	{
		Model::setPos(pos);
		notify(m_moveNotifications);
	}
}

void Point::setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize)
{
	Model::setScreenPos(screenPos, cameraMatrix, windowSize);
	notify(m_moveNotifications);
}

std::shared_ptr<Point::Callback> Point::registerForMoveNotification(const Callback& callback)
{
	std::shared_ptr<Callback> notification = std::make_shared<Callback>(callback);
	m_moveNotifications.push_back(notification);
	return notification;
}

std::shared_ptr<Point::Callback> Point::registerForDestroyNotification(const Callback& callback)
{
	std::shared_ptr<Callback> notification = std::make_shared<Callback>(callback);
	m_destroyNotifications.push_back(notification);
	return notification;
}

bool Point::isReferenced()
{
	clearExpiredNotifications();
	return m_moveNotifications.size() != 0 || m_destroyNotifications.size() != 0;
}

int Point::m_nonVirtualCount = 0;

int Point::m_virtualCount = 0;

void Point::updateShaders() const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("posWorld", m_pos);
	m_shaderProgram.setUniform("isVirtual", isVirtual());
	m_shaderProgram.setUniform("isSelected", isSelected());
}

void Point::notify(std::vector<std::weak_ptr<Callback>>& notifications)
{
	clearExpiredNotifications();

	for (const std::weak_ptr<Callback>& notification : notifications)
	{
		std::shared_ptr<Callback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this);
		}
	}
}

void Point::clearExpiredNotifications()
{
	std::erase_if
	(
		m_moveNotifications,
		[] (const std::weak_ptr<Callback>& notification)
		{
			return notification.expired();
		}
	);

	std::erase_if
	(
		m_destroyNotifications,
		[] (const std::weak_ptr<Callback>& notification)
		{
			return notification.expired();
		}
	);
}
