#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierSurface;

class BezierSurfaceGUI : public ModelGUI
{
public:
	BezierSurfaceGUI(BezierSurface& surface);
	virtual ~BezierSurfaceGUI() = default;
	virtual void update() override;

private:
	BezierSurface& m_surface;

	void updateRenderGrid();
	void updateLineCount();
	void updateIntersectionCurves();
};
