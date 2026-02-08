#pragma once

class CameraGUI
{
public:
	virtual ~CameraGUI() = default;

	virtual void update() = 0;
};
