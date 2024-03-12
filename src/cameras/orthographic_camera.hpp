#pragma once

#include "camera_guis/camera_gui.hpp"
#include "camera_guis/orthographic_camera_gui.hpp"
#include "cameras/camera.hpp"

class OrthographicCamera : public Camera
{
public:
	OrthographicCamera(float viewWidth, float aspectRatio, float nearPlane, float farPlane,
		ShaderProgram& wireframeShaderProgram, ShaderProgram& solidShaderProgram);
	virtual CameraGUI& getGUI() override;
	float getViewWidth() const;
	void setViewWidth(float viewWidth);
	virtual void zoom(float zoom) override;

protected:
	virtual void updateProjectionMatrix() override;

private:
	OrthographicCameraGUI m_gui;

	float m_viewWidth{};
};
