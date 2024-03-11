#pragma once

#include "camera_guis/camera_gui.hpp"

class OrthographicCamera;

class OrthographicCameraGUI : public CameraGUI
{
public:
	OrthographicCameraGUI(OrthographicCamera& camera);
	virtual void update() override;

private:
	OrthographicCamera& m_camera;

	float m_viewWidth{};

	void getValues();
	void setValues();
};
