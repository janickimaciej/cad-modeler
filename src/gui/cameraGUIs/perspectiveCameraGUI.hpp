#pragma once

#include "gui/cameraGUIs/cameraGUI.hpp"

class PerspectiveCamera;

class PerspectiveCameraGUI : public CameraGUI
{
public:
	PerspectiveCameraGUI(PerspectiveCamera& camera);
	virtual ~PerspectiveCameraGUI() = default;

	virtual void update() override;

private:
	PerspectiveCamera& m_camera;
};
