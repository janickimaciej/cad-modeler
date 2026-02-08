#pragma once

#include "gui/cameraGUIs/cameraGUI.hpp"

class OrthographicCamera;

class OrthographicCameraGUI : public CameraGUI
{
public:
	OrthographicCameraGUI(OrthographicCamera& camera);
	virtual ~OrthographicCameraGUI() = default;

	virtual void update() override;

private:
	OrthographicCamera& m_camera;
};
