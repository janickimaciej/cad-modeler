#pragma once

#include "camera_guis/camera_gui.hpp"
#include "camera_guis/perspective_camera_gui.hpp"
#include "cameras/camera.hpp"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fovRad, float aspectRatio, float nearPlane, float farPlane,
		ShaderProgram& solidShaderProgram, ShaderProgram& wireframeShaderProgram);
	virtual CameraGUI& getGUI() override;
	float getFOVRad() const;
	void setFOVRad(float viewWidth);
	virtual void zoom(float zoom) override;

protected:
	virtual void updateProjectionMatrix() override;

private:
	PerspectiveCameraGUI m_gui;

	float m_fovRad{};
};
