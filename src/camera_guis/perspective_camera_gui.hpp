#pragma once

#include "camera_guis/camera_gui.hpp"

class PerspectiveCamera;

class PerspectiveCameraGUI : public CameraGUI
{
public:
	PerspectiveCameraGUI(PerspectiveCamera& camera);
	virtual void update() override;

private:
	PerspectiveCamera& m_camera;

	int m_fovDeg{};

	void getValues();
	void setValues();
};
