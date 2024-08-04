#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class Torus;

class TorusGUI : public ModelGUI
{
public:
	TorusGUI(Torus& torus);
	virtual void update() override;

private:
	Torus& m_torus;

	void updatePos();
	void updateOrientation();
	void updateScale();
	void updateRadii();
	void updateGrid();
};
