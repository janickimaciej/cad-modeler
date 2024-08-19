#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierSurfaceC0;

class BezierSurfaceC0GUI : public ModelGUI
{
public:
	BezierSurfaceC0GUI(BezierSurfaceC0& surface);
	virtual void update() override;

private:
	BezierSurfaceC0& m_surface;
};
