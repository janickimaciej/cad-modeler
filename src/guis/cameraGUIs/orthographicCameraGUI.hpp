#pragma once

#include "guis/cameraGUIs/cameraGUI.hpp"

class OrthographicCamera;

class OrthographicCameraGUI : public CameraGUI
{
public:
	OrthographicCameraGUI(OrthographicCamera& camera);
	virtual void update() override;

private:
	OrthographicCamera& m_camera;
};
