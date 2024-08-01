#pragma once

#include "guis/cameraGUIs/cameraGUI.hpp"

class PerspectiveCamera;

class PerspectiveCameraGUI : public CameraGUI
{
public:
	PerspectiveCameraGUI(PerspectiveCamera& camera);
	virtual void update() override;

private:
	PerspectiveCamera& m_camera;

	int m_fovYDeg{};

	void getValues();
	void setValues();
};
