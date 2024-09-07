#include "models/gregorySurface.hpp"

#include <string>

GregorySurface::GregorySurface(const ShaderProgram& surfaceShaderProgram,
		const std::array<std::array<std::array<Point*, 4>, 2>, 3>& bezierPoints,
		const SelfDestructCallback& selfDestructCallback) :
	Model{{}, "Gregory surface " + std::to_string(m_count++)},
	m_surfaceShaderProgram{surfaceShaderProgram},
	m_bezierPoints{bezierPoints},
	m_selfDestructCallback{selfDestructCallback}
{
	// TODO
}

void GregorySurface::render() const
{
	// TODO
}

void GregorySurface::updateGUI()
{
	// TODO
}

int GregorySurface::m_count = 0;

void GregorySurface::updateShaders() const
{
	// TODO
}

void GregorySurface::registerForNotifications()
{
	for (const std::array<std::array<Point*, 4>, 2>& patchPoints : m_bezierPoints)
	{
		for (const std::array<Point*, 4>& row : patchPoints)
		{
			for (Point* point : row)
			{
				registerForNotifications(point);
			}
		}
	}
}

void GregorySurface::registerForNotifications(Point* point)
{
	m_pointMoveNotifications.push_back(point->registerForMoveNotification
		(
			[this] (void*)
			{
				pointMoveNotification();
			}
		));

	m_pointDestroyNotifications.push_back(point->registerForDestroyNotification
		(
			[this] (void* notification)
			{
				pointDestroyNotification(static_cast<Point::DestroyCallback*>(notification));
			}
		));

	m_pointRereferenceNotifications.push_back(point->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		));
}

void GregorySurface::pointMoveNotification()
{
	updateGeometry();
}

void GregorySurface::pointDestroyNotification(Point::DestroyCallback* notification)
{
	m_pointMoveNotifications.clear();
	m_pointDestroyNotifications.clear();
	m_pointRereferenceNotifications.clear();
	m_selfDestructCallback(this);
}

void GregorySurface::pointRereferenceNotification(Point::RereferenceCallback* notification,
	Point* newPoint)
{
	auto iterator = std::find_if
	(
		m_pointRereferenceNotifications.begin(), m_pointRereferenceNotifications.end(),
		[notification] (const std::shared_ptr<Point::RereferenceCallback>& sharedNotification)
		{
			return sharedNotification.get() == notification;
		}
	);
	std::size_t notificationIndex = iterator - m_pointRereferenceNotifications.begin();
	std::size_t patchIndex = notificationIndex / 8;
	std::size_t rowIndex = (notificationIndex % 8) / 4;
	std::size_t pointIndex = notificationIndex % 4;

	m_bezierPoints[patchIndex][rowIndex][pointIndex] = newPoint;

	m_pointMoveNotifications[notificationIndex] = newPoint->registerForMoveNotification
		(
			[this] (void*)
			{
				pointMoveNotification();
			}
		);

	m_pointDestroyNotifications[notificationIndex] = newPoint->registerForDestroyNotification
		(
			[this] (void* notification)
			{
				pointDestroyNotification(static_cast<Point::DestroyCallback*>(notification));
			}
		);

	m_pointRereferenceNotifications[notificationIndex] =
		newPoint->registerForRereferenceNotification
		(
			[this] (void* notification, Point* newPoint)
			{
				pointRereferenceNotification(static_cast<Point::RereferenceCallback*>(notification),
					newPoint);
			}
		);

	updateGeometry();
}

void GregorySurface::updateGeometry()
{
	// TODO
}
