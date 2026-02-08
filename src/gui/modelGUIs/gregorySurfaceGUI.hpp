#pragma once

#include "gui/modelGUIs/modelGUI.hpp"

class GregorySurface;

class GregorySurfaceGUI : public ModelGUI
{
public:
	GregorySurfaceGUI(GregorySurface& surface);
	virtual ~GregorySurfaceGUI() = default;

	virtual void update() override;

private:
	GregorySurface& m_surface;
};
