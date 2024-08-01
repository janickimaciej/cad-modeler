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

	float m_x{};
	float m_y{};
	float m_z{};

	float m_yawDeg{};
	float m_pitchDeg{};
	float m_rollDeg{};

	float m_scaleX{};
	float m_scaleY{};
	float m_scaleZ{};

	float m_majorRadius{};
	float m_minorRadius{};

	int m_major{};
	int m_minor{};

	void getValues();
	void setValues();
};
