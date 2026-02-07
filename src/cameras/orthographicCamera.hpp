#pragma once

#include "guis/cameraGUIs/orthographicCameraGUI.hpp"
#include "cameras/camera.hpp"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(const glm::ivec2& viewportSize, float viewHeight, float nearPlane,
		float farPlane);
	virtual ~OrthographicCamera() = default;
	virtual void updateGUI() override;
	float getViewHeight() const;
	void setViewHeight(float viewHeight);
	virtual void zoom(float zoom) override;

protected:
	virtual void updateProjectionMatrix() override;

private:
	OrthographicCameraGUI m_gui;

	float m_viewHeight{};
};
