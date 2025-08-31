#pragma once

#include "guis/cameraGUIs/perspectiveCameraGUI.hpp"
#include "cameras/camera.hpp"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(const glm::ivec2& windowSize, float fovYDeg, float nearPlane, float farPlane);
	virtual ~PerspectiveCamera() = default;
	virtual void updateGUI() override;
	float getFOVYDeg() const;
	void setFOVYDeg(float fovYDeg);
	virtual void zoom(float zoom) override;

protected:
	virtual void updateProjectionMatrix() override;

private:
	PerspectiveCameraGUI m_gui;

	float m_fovYDeg{};
};
