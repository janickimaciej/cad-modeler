#pragma once

#include "gui/modelGUIs/intersectableGUI.hpp"
#include "gui/modelGUIs/modelGUI.hpp"

class BezierSurface;

class BezierSurfaceGUI : public ModelGUI
{
public:
	BezierSurfaceGUI(BezierSurface& surface);
	virtual ~BezierSurfaceGUI() = default;

	virtual void update() override;

private:
	BezierSurface& m_surface;
	IntersectableGUI m_intersectableGUI;

	void updateRenderGrid();
	void updateLineCount();
};
