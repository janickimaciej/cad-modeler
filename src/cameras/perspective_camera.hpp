#pragma once

#include "camera_guis/camera_gui.hpp"
#include "camera_guis/perspective_camera_gui.hpp"
#include "cameras/camera.hpp"

class PerspectiveCamera : public Camera
{
public:
	PerspectiveCamera(float fovYDeg, float aspectRatio, float nearPlane, float farPlane,
		ShaderProgram& wireframeShaderProgram, ShaderProgram& solidShaderProgram);
	virtual CameraGUI& getGUI() override;
	float getFOVYDeg() const;
	void setFOVYDeg(float fovYDeg);
	virtual void zoom(float zoom) override;

protected:
	virtual void updateProjectionMatrix() override;

private:
	PerspectiveCameraGUI m_gui;

	float m_fovYDeg{};
};
