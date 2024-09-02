#pragma once

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
	friend class PointSerializer;

public:
	class DeletabilityLock
	{
	public:
		DeletabilityLock(Point* point);
		DeletabilityLock(const DeletabilityLock&) = delete;
		DeletabilityLock(DeletabilityLock&& lock) noexcept;
		~DeletabilityLock();

		DeletabilityLock& operator=(const DeletabilityLock&) = delete;
		DeletabilityLock& operator=(DeletabilityLock&& lock) noexcept;

	private:
		Point* m_point{};
	};

	using Callback = std::function<void(const Point*)>;
	using RereferenceCallback = std::function<void(const Point*, Point*)>;

	Point(const ShaderProgram& shaderProgram, const glm::vec3& pos, bool isDeletable = true,
		bool isVirtual = false);
	virtual ~Point();

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3& pos) override;
	virtual void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize) override;

	std::shared_ptr<Callback> registerForMoveNotification(const Callback& callback);
	std::shared_ptr<Callback> registerForDestroyNotification(const Callback& callback);
	std::shared_ptr<RereferenceCallback> registerForRereferenceNotification(
		const RereferenceCallback& callback);
	DeletabilityLock getDeletabilityLock();
	void tryMakeDeletable();

	bool isReferenced();
	void rereference(Point* newPoint);

private:
	static int m_nonVirtualCount;
	static int m_virtualCount;

	const ShaderProgram& m_shaderProgram;
	PointGUI m_gui{*this};

	PointMesh m_mesh{};

	std::vector<std::weak_ptr<Callback>> m_moveNotifications{};
	std::vector<std::weak_ptr<Callback>> m_destroyNotifications{};
	std::vector<std::weak_ptr<RereferenceCallback>> m_rereferenceNotifications{};
	int m_deletabilityLockCounter = 0;

	virtual void updateShaders() const override;

	void notify(std::vector<std::weak_ptr<Callback>>& notifications);
	void notify(std::vector<std::weak_ptr<RereferenceCallback>>& notifications, Point* newPoint);
	void clearExpiredNotifications();
};
