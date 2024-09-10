#pragma once

#include "guis/modelGUIs/modelGUI.hpp"

class BezierPatch;

class BezierPatchGUI : public ModelGUI
{
public:
	BezierPatchGUI(BezierPatch& patch);
	virtual ~BezierPatchGUI() = default;
	virtual void update() override;

private:
	BezierPatch& m_patch;
};
