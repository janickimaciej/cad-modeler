#pragma once

#include "guis/cameraGUIs/perspectiveCameraGUI.hpp"
#include "cameras/camera.hpp"
#include "shaderPrograms.hpp"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fovYDeg, float aspectRatio, float nearPlane, float farPlane,
		const ShaderPrograms& shaderPrograms);
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
