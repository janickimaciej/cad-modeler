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
	friend class BezierSurfaceC0Serializer;
	friend class BezierSurfaceC2Serializer;
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

	using MoveCallback = std::function<void(void*)>;
	using DestroyCallback = std::function<void(void*)>;
	using RereferenceCallback = std::function<void(void*, Point*)>;

	Point(const ShaderProgram& shaderProgram, const glm::vec3& pos, bool isDeletable = true,
		bool isVirtual = false);
	virtual ~Point();

	virtual void render() const override;
	virtual void updateGUI() override;

	virtual void setPos(const glm::vec3& pos) override;
	virtual void setScreenPos(const glm::vec2& screenPos, const glm::mat4& cameraMatrix,
		const glm::ivec2& windowSize) override;

	std::shared_ptr<MoveCallback> registerForMoveNotification(const MoveCallback& callback);
	std::shared_ptr<DestroyCallback> registerForDestroyNotification(
		const DestroyCallback& callback);
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
	PointMesh m_mesh{};
	PointGUI m_gui{*this};

	std::vector<std::weak_ptr<MoveCallback>> m_moveNotifications{};
	std::vector<std::weak_ptr<DestroyCallback>> m_destroyNotifications{};
	std::vector<std::weak_ptr<RereferenceCallback>> m_rereferenceNotifications{};
	int m_deletabilityLockCounter = 0;

	virtual void updateShaders() const override;

	void notifyMove();
	void notifyDestroy();
	void notifyRereference(Point* newPoint);
	void clearExpiredNotifications();
};
