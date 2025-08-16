#pragma once

#include "guis/modelGUIs/intersectableGUI.hpp"
#include "guis/modelGUIs/modelGUI.hpp"

class Torus;

class TorusGUI : public ModelGUI
{
public:
	TorusGUI(Torus& torus);
	virtual ~TorusGUI() = default;
	virtual void update() override;

private:
	Torus& m_torus;
	IntersectableGUI m_intersectableGUI;

	void updatePos();
	void updateOrientation();
	void updateScale();
	void updateRadii();
	void updateGrid();
};
