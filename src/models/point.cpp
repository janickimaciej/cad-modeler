#include "models/point.hpp"

#include <array>
#include <iterator>
#include <string>

Point::DeletabilityLock::DeletabilityLock(Point* point) :
	m_point{point}
{
	++m_point->m_deletabilityLockCounter;
}

Point::DeletabilityLock::DeletabilityLock(DeletabilityLock&& lock) noexcept
{
	m_point = lock.m_point;
	lock.m_point = nullptr;
}

Point::DeletabilityLock::~DeletabilityLock()
{
	if (m_point != nullptr)
	{
		--m_point->m_deletabilityLockCounter;
	}
}

Point::DeletabilityLock& Point::DeletabilityLock::operator=(DeletabilityLock&& lock) noexcept
{
	m_point = lock.m_point;
	lock.m_point = nullptr;
	return *this;
}

Point::Point(const glm::vec3& pos, bool isDeletable, bool isVirtual) :
	Model{pos, isVirtual ? "VirtualPoint " + std::to_string(m_virtualCount++) :
		"Point " + std::to_string(m_nonVirtualCount++), isDeletable, isVirtual}
{ }

Point::~Point()
{
	notifyDestroy();
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
	Model::setPos(pos);
	notifyMove();
}

std::shared_ptr<Point::MoveCallback> Point::registerForMoveNotification(
	const MoveCallback& callback)
{
	std::shared_ptr<MoveCallback> notification = std::make_shared<MoveCallback>(callback);
	m_moveNotifications.push_back(notification);
	return notification;
}

std::shared_ptr<Point::DestroyCallback> Point::registerForDestroyNotification(
	const DestroyCallback& callback)
{
	std::shared_ptr<DestroyCallback> notification = std::make_shared<DestroyCallback>(callback);
	m_destroyNotifications.push_back(notification);
	return notification;
}

std::shared_ptr<Point::RereferenceCallback> Point::registerForRereferenceNotification(
	const Point::RereferenceCallback& callback)
{
	std::shared_ptr<RereferenceCallback> notification =
		std::make_shared<RereferenceCallback>(callback);
	m_rereferenceNotifications.push_back(notification);
	return notification;
}

Point::DeletabilityLock Point::acquireDeletabilityLock()
{
	return DeletabilityLock(this);
}

void Point::tryMakeDeletable()
{
	if (m_deletabilityLockCounter == 0)
	{
		setDeletable(true);
	}
}

bool Point::isReferenced()
{
	clearExpiredNotifications();
	return m_moveNotifications.size() > 0 || m_destroyNotifications.size() > 0 ||
		m_deletabilityLockCounter > 0;
}

void Point::rereference(Point* newPoint)
{
	notifyRereference(newPoint);

	newPoint->m_moveNotifications.insert(newPoint->m_moveNotifications.end(),
		std::make_move_iterator(m_moveNotifications.begin()),
		std::make_move_iterator(m_moveNotifications.end()));
	m_moveNotifications.clear();

	newPoint->m_destroyNotifications.insert(newPoint->m_destroyNotifications.end(),
		std::make_move_iterator(m_destroyNotifications.begin()),
		std::make_move_iterator(m_destroyNotifications.end()));
	m_destroyNotifications.clear();

	newPoint->m_rereferenceNotifications.insert(newPoint->m_rereferenceNotifications.end(),
		std::make_move_iterator(m_rereferenceNotifications.begin()),
		std::make_move_iterator(m_rereferenceNotifications.end()));
	m_rereferenceNotifications.clear();
}

int Point::m_nonVirtualCount = 0;

int Point::m_virtualCount = 0;

void Point::updateShaders() const
{
	m_shaderProgram.use();
	m_shaderProgram.setUniform("posWorld", getPos());
	m_shaderProgram.setUniform("isDark", isVirtual());
	m_shaderProgram.setUniform("isSelected", isSelected());
}

void Point::notifyMove()
{
	clearExpiredNotifications();

	for (const std::weak_ptr<MoveCallback>& notification : m_moveNotifications)
	{
		std::shared_ptr<MoveCallback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this);
		}
	}
}

void Point::notifyDestroy()
{
	clearExpiredNotifications();

	for (const std::weak_ptr<DestroyCallback>& notification : m_destroyNotifications)
	{
		std::shared_ptr<DestroyCallback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this);
		}
	}
}

void Point::notifyRereference(Point* newPoint)
{
	clearExpiredNotifications();

	for (const std::weak_ptr<RereferenceCallback>& notification : m_rereferenceNotifications)
	{
		std::shared_ptr<RereferenceCallback> notificationShared = notification.lock();
		if (notificationShared)
		{
			(*notificationShared)(this, newPoint);
		}
	}
}

void Point::clearExpiredNotifications()
{
	std::erase_if
	(
		m_moveNotifications,
		[] (const std::weak_ptr<MoveCallback>& notification)
		{
			return notification.expired();
		}
	);

	std::erase_if
	(
		m_destroyNotifications,
		[] (const std::weak_ptr<DestroyCallback>& notification)
		{
			return notification.expired();
		}
	);

	std::erase_if
	(
		m_rereferenceNotifications,
		[] (const std::weak_ptr<RereferenceCallback>& notification)
		{
			return notification.expired();
		}
	);
}
