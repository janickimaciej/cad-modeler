#include "models/point.hpp"

#include <glad/glad.h>

#include <array>
#include <string>

Point::Point(const ShaderProgram& pointShaderProgram, glm::vec3 position, bool isVirtual) :
	Model{position, (isVirtual ? "VirtualPoint " : "Point ") + std::to_string(m_count), isVirtual},
	m_id{m_count++},
	m_pointShaderProgram{pointShaderProgram},
	m_gui{*this}
{
	glGenVertexArrays(1, &m_VAO);
}

Point::~Point()
{
	notify(m_destroyNotifications);
}

void Point::render(RenderMode renderMode) const
{
	updateShaders(renderMode);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}

void Point::updateGUI()
{
	m_gui.update();
}

void Point::setPosition(const glm::vec3& position)
{
	if (position != m_position)
	{
		Model::setPosition(position);
		notify(m_moveNotifications);
	}
}

void Point::setScreenPosition(const glm::vec2& screenPosition, const glm::mat4& cameraMatrix,
	const glm::ivec2& windowSize)
{
	Model::setScreenPosition(screenPosition, cameraMatrix, windowSize);
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

int Point::m_count = 0;

void Point::updateShaders(RenderMode) const
{
	m_pointShaderProgram.use();
	m_pointShaderProgram.setUniform("posWorld", m_position);
	m_pointShaderProgram.setUniform("isVirtual", isVirtual());
	m_pointShaderProgram.setUniform("isActive", isActive());
}

void Point::notify(std::vector<std::weak_ptr<Callback>>& notifications)
{
	clearExpiredNotifications();

	for (const std::weak_ptr<std::function<void(Point*)>>& notification : notifications)
	{
		std::shared_ptr<std::function<void(Point*)>> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this);
		}
	}
}

void Point::clearExpiredNotifications()
{
	std::erase_if(m_moveNotifications,
		[] (const std::weak_ptr<std::function<void(Point*)>>& notification)
		{
			return notification.expired();
		}
	);

	std::erase_if(m_destroyNotifications,
		[] (const std::weak_ptr<std::function<void(Point*)>>& notification)
		{
			return notification.expired();
		}
	);
}
